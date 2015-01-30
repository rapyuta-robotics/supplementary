/*
 * ManagedExecutable.cpp
 *
 *  Created on: Nov 4, 2014
 *      Author: Stephan Opfer
 */

#include "ManagedExecutable.h"
#include <map>
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <sstream>
#include <vector>
#include <fstream>
#include <string.h>
#include "SystemConfig.h"

namespace supplementary
{
	long ManagedExecutable::kernelPageSize = 0;

	ManagedExecutable::ManagedExecutable(string executable, int id, long pid) :
			managedPid(pid), executable(executable), id(id), state(UNDEFINED), cutime(0), cstime(0), utime(0), stime(0), memory(0), starttime(0), shouldRun(false)
	{
		SystemConfig* sc = supplementary::SystemConfig::getInstance();
		vector<string> defaultParams = (*sc)["Processes"]->getList<string>("Processes.ProcessDescriptions", executable.c_str(), "defaultParams", NULL);

#ifdef MGND_EXEC_DEBUG
		cout << "ME: Constructor of executable " << executable << endl;
		for (string s : defaultParams)
		{
			cout << "'" << s << "'" << endl;
		}
#endif

		// convert vector<string> to char*[] (where each element is null-terminated)
		std::vector<char*> vec;
		std::transform(defaultParams.begin(), defaultParams.end(), std::back_inserter(vec), [](std::string& s)
		{	s.push_back(0); return &s[0];});
		vec.push_back(nullptr);
		this->defaultParams = vec.data();

		this->desiredParams = nullptr;
		this->params = nullptr;
	}

	ManagedExecutable::~ManagedExecutable()
	{
		// TODO: check whether the cleanup is right -> valgrind
		delete[] defaultParams;
		free(desiredParams);
		free(params);

		if (this->managedPid != NOTHING_MANAGED)
		{
			/* TODO: more comprehensive process stopping
			 * - try several times
			 * - try harder signals, if necessary
			 * - give better feedback than true and false (maybe console output)
			 */
			kill(this->managedPid, SIGTERM);
		}
	}

	string ManagedExecutable::getExecutable() const
	{
		return string(executable);
	}

	void ManagedExecutable::queue4Update(long pid)
	{
		this->queuedPids4Update.push_back(pid);
	}

	void ManagedExecutable::update()
	{
		// If there is no process left, clean up
		if (this->queuedPids4Update.size() == 0 && this->managedPid != NOTHING_MANAGED)
		{
#ifdef MGND_EXEC_DEBUG
			cout << "ME: No " << this->executable << " running!" << endl;
#endif
			this->clear();
		}
		else if (this->queuedPids4Update.size() > 0)
		{
#ifdef MGND_EXEC_DEBUG
			/*cout << "ME: " << this->executable << " Queued PIDs:  ";
			 for (long curPid : this->queuedPids4Update)
			 {
			 cout << curPid << ", ";
			 }
			 cout << endl; */
#endif

			for (int i = 0; i < this->queuedPids4Update.size(); i++)
			{
				long pid = this->queuedPids4Update.at(i);
				if (this->managedPid == pid)
				{
					// erase own and kill all other processes
					this->queuedPids4Update.erase(begin(this->queuedPids4Update) + i);
					this->killOtherProcesses();
					this->queuedPids4Update.clear();

					// update own
					this->updateStats(true);
					break;
				}
			}

			if (this->queuedPids4Update.size() != 0) // we did not find our own process
			{
				// adapt to the first and kill the other processes
				this->managedPid = this->queuedPids4Update.at(0);
				this->queuedPids4Update.erase(this->queuedPids4Update.begin());
				this->killOtherProcesses();

				// update own
				this->updateStats(true);
			}
		}
#ifdef MGND_EXEC_DEBUG
		if (this->queuedPids4Update.size() != 0)
		{
			cout << "ME: ERROR Update queue not cleared! " << endl;
		}
#endif
	}

	/**
	 * Kills all processes with SIGKILL, which are still in the queuedPIids4Update list.
	 */
	void ManagedExecutable::killOtherProcesses()
	{
		/* TODO: more comprehensive process stopping
		 * - try several times
		 * - try harder signals, if necessary
		 * - give better feedback than true and false (maybe console output)
		 */
		for (auto& pid : this->queuedPids4Update)
		{
			kill(pid, SIGKILL);
		}
	}

	/**
	 * Updates the stats of the managed process (CPU, Memory, State, etc.)
	 */
	void ManagedExecutable::updateStats(bool readParams)
	{
#ifdef MGND_EXEC_DEBUG
		//cout << "ME: Updating " << this->executable << " (" << this->managedPid << ")" << endl;
#endif

		string procPidString = "/proc/" + to_string(this->managedPid);
		std::ifstream statFile(procPidString + "/stat", std::ifstream::in);
		string statLine;
		getline(statFile, statLine);

		//cout << "statline " << statLine << endl;
		string tmp;
		stringstream statStream(statLine);
		int i = 0;
		while (statStream.good() && i < 52)
		{
			if (i == 2)
			{ //state
				statStream >> this->state;
				i++;
			}
			else if (i == 13)
			{ // cpu time stuff (includes index 13-16)
				statStream >> this->utime;
				statStream >> this->stime;
				statStream >> this->cutime;
				statStream >> this->cstime;
				i += 4;
			}
			else if (i == 21)
			{
				statStream >> this->starttime;
				i++;
			}
			else if (i == 23)
			{
				statStream >> this->memory;
				break;
			}
			else
			{
				statStream >> tmp;
				i++;
				continue;
			}
		}

		if (readParams)
		{
			this->readProcParams(procPidString);
		}

#ifdef MGND_EXEC_DEBUG
		//this->printStats();
		//cout << "ME: Updated " << this->executable << " (" << this->managedPid << ")" << endl;
#endif

	}

	void ManagedExecutable::readProcParams(string procPidString)
	{
		std::ifstream cmdlineFile(procPidString + "/cmdline", std::ifstream::in);
		string line;
		getline(cmdlineFile, line);

		// determine the number of parameters and their length
		vector<char const *> argv;
		int startPos = 0;
		int endPos = 0;
		while (true) {
			endPos = line.find('\0',startPos);
			if (endPos != string::npos)
			{
				cout << "ME: Found '" << line.substr(startPos, endPos - startPos).c_str() << "' at (" << startPos << ", " << endPos << endl;
				argv.push_back(line.substr(startPos, endPos - startPos).c_str());
				startPos = endPos+1;
			}
			else
			{
				break;
			}
		}

	}

	void ManagedExecutable::printStats()
	{
		stringstream ss;
		ss << "ME: Stats of " << this->executable << " (" << this->managedPid << ")" << endl;
		ss << "ME: State: '" << this->state << "' StartTime: " << this->starttime << endl;
		ss << "ME: Times: u '" << this->utime << "' s '" << this->stime << "' cu '" << this->cutime << "' cs '" << this->cstime << endl;
		ss << "ME: Memory: " << this->memory * kernelPageSize / 1024.0 / 1024.0 << "MB" << endl;
		ss << "ME: Parameters: '" << this->params << endl;
		cout << ss.str();
	}

	/**
	 * Clears information about the former managed process.
	 */
	void ManagedExecutable::clear()
	{
		this->managedPid = NOTHING_MANAGED;
		this->state = UNDEFINED;
		free(params);
		this->params = nullptr;
	}

	void ManagedExecutable::changeDesiredState(bool shouldRun)
	{
		if (this->managedPid != NOTHING_MANAGED && shouldRun)
		{
			cout << "ME: Wont start executable, as it is already running! PID: " << this->managedPid << endl;
			return;
		}

		if (this->managedPid == NOTHING_MANAGED && !shouldRun)
		{
			cout << "ME: Can not stop executable, as nothing is running!" << endl;
			return;
		}

		// remember desired executable state
		this->lastCommandTime = chrono::steady_clock::now();
		this->shouldRun = shouldRun;
	}

	/**
	 * Starts a process with the given parameters.
	 * The started process replaces already running processes.
	 * @param params The parameters given to the started process.
	 */
	void ManagedExecutable::startProcess(char* const * params)
	{

		pid_t pid = fork();
		if (pid == 0) // child process
		{
			int execReturn = execv(this->executable.c_str(), params);
			if (execReturn == -1)
			{
				cout << "ME: Failure! execve error code = " << errno << " - " << strerror(errno) << endl;
			}
		}
		else if (pid > 0) // parent process
		{
			this->managedPid = pid;
		}
		else if (pid < 0)
		{
			cout << "ME: Failed to fork!" << endl;
			this->managedPid = NOTHING_MANAGED;
		}
	}

	/**
	 * Starts a process with the default parameters.
	 * The started process replaces already running processes.
	 */
	void ManagedExecutable::startProcess()
	{
		this->startProcess(this->defaultParams);
	}

	bool ManagedExecutable::stopProcess()
	{
		/* TODO: more comprehensive process stopping
		 * - try several times
		 * - try harder signals, if necessary
		 * - give better feedback than true and false (maybe console output)
		 */
		if (this->managedPid != 0)
		{
			kill(this->managedPid, SIGTERM);
			return true;
		}
		else
		{
			return false;
		}
	}

} /* namespace supplementary */
