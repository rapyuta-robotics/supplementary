#pragma once

//#include "Configuration.h"
#include "IAlicaConfig.h"

#include <essentials/FileSystem.h>

#include <atomic>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unistd.h>

using std::cerr;
using std::cout;
using std::endl;
using std::mutex;
using std::shared_ptr;
using std::string;
using std::vector;

const std::string DOMAIN_FOLDER = "DOMAIN_FOLDER";
const std::string DOMAIN_CONFIG_FOLDER = "DOMAIN_CONFIG_FOLDER";

namespace essentials
{
template <class ConfigType>
class SystemConfig
{
protected:
    std::string rootPath;
    std::string logPath;
    std::string configPath;
    std::string hostname;
    std::mutex configsMapMutex;
    std::map<std::string, std::shared_ptr<ConfigType>> configs;
    const char NODE_NAME_SEPERATOR = '_';

public:
    SystemConfig();
    ~SystemConfig(){};
    void shutdown();
    std::string robotNodeName(const std::string& nodeName);
    int getOwnRobotID();
    int getRobotID(const std::string& name);
    std::string getHostname();
    void setHostname(const std::string& newHostname);
    void resetHostname();

    ConfigType* operator[](const std::string& s);
    std::string getConfigFileName(const std::string& s);
    std::string getRootPath();
    std::string getConfigPath();
    std::string getLogPath();
    void setRootPath(std::string rootPath);
    void setConfigPath(std::string configPath);
    std::string getEnv(const std::string& var);
};

/**
 * The constructor of the SystemConfig class.
 */
 template <class ConfigType>
 SystemConfig<ConfigType>::SystemConfig()
{
    // set the domain folder (1. by env-variable 2. by cwd)
    char* x = ::getenv(DOMAIN_FOLDER.c_str());
    if (x == NULL) {
        char cwd[4096];
        if (::getcwd(cwd, 4096) == NULL) {
            cerr << "SystemConfig: Error while calling getcwd!" << endl;
        }
        rootPath = cwd;
    } else {
        rootPath = x;
    }

    // set the domain config folder (1. by env-variable 2. by <domain folder>/etc
    x = ::getenv(DOMAIN_CONFIG_FOLDER.c_str());

    if (x == NULL) {
        configPath = FileSystem::combinePaths(rootPath, "/etc");
    } else {
        configPath = x;
    }
    if (!essentials::FileSystem::endsWith(configPath, essentials::FileSystem::PATH_SEPARATOR)) {
        configPath = configPath + essentials::FileSystem::PATH_SEPARATOR;
    }
    if (!FileSystem::pathExists(configPath)) {
        cerr << "SC: Could not find config directory: \"" << configPath << "\"" << endl;
    }

    logPath = FileSystem::combinePaths(rootPath, "/log/temp");
    if (!FileSystem::pathExists(logPath)) {
        if (!FileSystem::createDirectory(logPath)) {
            cerr << "SC: Could not create log directory: \"" << logPath << "\"" << endl;
        }
    }

    // set the hostname (1. by env-variable 2. by gethostname)
    char* envname = ::getenv("ROBOT");
    if ((envname == NULL) || ((*envname) == 0x0)) {
        char hn[1024];
        hn[1023] = '\0';
        gethostname(hn, 1023);
        SystemConfig::hostname = hn;
    } else {
        hostname = envname;
    }

    cout << "SC: Root:           \"" << rootPath << "\"" << endl;
    cout << "SC: ConfigRoot:     \"" << configPath << "\"" << endl;
    cout << "SC: Hostname:       \"" << hostname << "\"" << endl;
    cout << "SC: Logging Folder: \"" << logPath << "\"" << endl;
}

 template <class ConfigType>
 void SystemConfig<ConfigType>::shutdown() {}

/**
 * The access operator for choosing the configuration according to the given string
 *
 * @param s The string which determines the used configuration.
 * @return The demanded configuration.
 */
 template <class ConfigType>
 ConfigType* SystemConfig<ConfigType>::operator[](const std::string& s)
{
    {
        std::lock_guard<mutex> lock(configsMapMutex);

        typename std::map<std::string, std::shared_ptr<ConfigType>>::iterator itr = configs.find(s);

        if (itr != configs.end()) {
            return itr->second.get();
        }
    }

    std::string file_name = getConfigFileName(s);
    if (file_name.empty()) {
        return nullptr;
    } else {
        std::lock_guard<mutex> lock(configsMapMutex);
        std::shared_ptr<ConfigType> result = std::make_shared<ConfigType>(file_name);
        configs[s] = result;

        return result.get();
    }
}

 template <class ConfigType>
 std::string SystemConfig<ConfigType>::getConfigFileName(const std::string& s) {
    string file = s + ".conf";
    // Check the host-specific config
    vector<string> files;
    string tempConfigPath = configPath;
    tempConfigPath = FileSystem::combinePaths(tempConfigPath, hostname);
    tempConfigPath = FileSystem::combinePaths(tempConfigPath, file);
    files.push_back(tempConfigPath);

    // Check the global config
    tempConfigPath = configPath;
    tempConfigPath = FileSystem::combinePaths(tempConfigPath, file);
    files.push_back(tempConfigPath);

    for (size_t i = 0; i < files.size(); i++) {
        if (FileSystem::pathExists(files[i])) {
            return files[i];
        }
    }

    // config-file not found, print error message
    cerr << "Configuration file " << file << " not found in either location:" << endl;
    for (size_t i = 0; i < files.size(); i++) {
        cerr << "- " << files[i] << endl;
    }
    return "";
}

/**
 * Looks up the own robot's ID with the system config's local hostname.
 * @return The own robot's ID
 * <deprecated>
 */
 template <class ConfigType>
 int SystemConfig<ConfigType>::getOwnRobotID()
{
    return SystemConfig::getRobotID(this->getHostname());
}

/**
 * Looks up the robot's ID with the given name.
 * @return The robot's ID
 * <deprecated>
 */
 template <class ConfigType>
 int SystemConfig<ConfigType>::getRobotID(const string& name)
{
    // TODO this should be optional for dynamic teams (is it ok to return ints?)
    ConfigType* tmp = this["Globals"];
    int ownRobotID = tmp.template get<int>("Globals", "Team", name.c_str(), "ID", NULL);
    return ownRobotID;
}

 template <class ConfigType>
 string SystemConfig<ConfigType>::getRootPath()
{
    return rootPath;
}

 template <class ConfigType>
 string SystemConfig<ConfigType>::getConfigPath()
{
    return configPath;
}

 template <class ConfigType>
 string SystemConfig<ConfigType>::getLogPath()
{
    return logPath;
}

 template <class ConfigType>
 string SystemConfig<ConfigType>::getHostname()
{
    return hostname;
}

 template <class ConfigType>
 void SystemConfig<ConfigType>::setHostname(const std::string& newHostname)
{
    hostname = newHostname;
    configs.clear();
    cout << "SC: Update Hostname:       \"" << hostname << "\"" << endl;
}

 template <class ConfigType>
 void SystemConfig<ConfigType>::setRootPath(string rootPath)
{
    if (!essentials::FileSystem::endsWith(rootPath, essentials::FileSystem::PATH_SEPARATOR)) {
        rootPath = rootPath + essentials::FileSystem::PATH_SEPARATOR;
    }
    this->rootPath = rootPath;
    cout << "SC: Update Root:           \"" << rootPath << "\"" << endl;
}

 template <class ConfigType>
 void SystemConfig<ConfigType>::setConfigPath(string configPath)
{
    if (!essentials::FileSystem::endsWith(configPath, essentials::FileSystem::PATH_SEPARATOR)) {
        configPath = configPath + essentials::FileSystem::PATH_SEPARATOR;
    }
    this->configPath = configPath;
    cout << "SC: Update ConfigRoot:     \"" << configPath << "\"" << endl;
}

 template <class ConfigType>
 void SystemConfig<ConfigType>::resetHostname()
{
    char* envname = ::getenv("ROBOT");
    if ((envname == NULL) || ((*envname) == 0x0)) {
        char hn[1024];
        hn[1023] = '\0';
        gethostname(hn, 1023);
        SystemConfig::hostname = hn;
    } else {
        hostname = envname;
    }
    configs.clear();
}

 template <class ConfigType>
 string SystemConfig<ConfigType>::robotNodeName(const string& nodeName)
{
    return SystemConfig::getHostname() + NODE_NAME_SEPERATOR + nodeName;
}

 template <class ConfigType>
 string SystemConfig<ConfigType>::getEnv(const string& var)
{
    const char* val = ::getenv(var.c_str());
    if (val == 0) {
        cerr << "SC: Environment Variable " << var << " is null" << endl;
        return "";
    } else {
        cout << "SC: Environment Variable " << var << " is " << val << endl;
        return val;
    }
}

}