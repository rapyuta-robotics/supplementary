#pragma once

#include "IAlicaConfig.h"
#include "Configuration.h"

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
class SystemConfig
{
protected:
    std::string rootPath;
    std::string logPath;
    std::string configPath;
    std::string hostname;
    std::mutex configsMapMutex;
    std::map<std::string, std::shared_ptr<IAlicaConfig*>> configs;
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

    IAlicaConfig* operator[](const std::string& s);
    std::string getConfigFileName(const std::string& s);
    std::string getRootPath();
    std::string getConfigPath();
    std::string getLogPath();
    void setRootPath(std::string rootPath);
    void setConfigPath(std::string configPath);
    std::string getEnv(const std::string& var);
};
}