#include "essentials/SystemConfig.h"

namespace essentials
{
/**
 * The constructor of the SystemConfig class.
 */
template <class T>
SystemConfig<T>::SystemConfig()
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

template <class T>
void SystemConfig<T>::shutdown() {}

/**
 * The access operator for choosing the configuration according to the given string
 *
 * @param s The string which determines the used configuration.
 * @return The demanded configuration.
 */
template <class T>
T& SystemConfig<T>::operator[](const std::string& s)
{
    {
        std::lock_guard<mutex> lock(configsMapMutex);

        typename std::map<std::string, std::shared_ptr<T>>::iterator itr = configs.find(s);

        if (itr != configs.end()) {
            return *(itr->second.get());
        }
    }

    std::string file_name = getConfigFileName(s);
    if (file_name.empty()) {
        std::string errMsg = "SC-Conf: Could not find config: " + s;
        std::cerr << errMsg << std::endl;
        throw std::runtime_error(errMsg);
    } else {
        std::lock_guard<mutex> lock(configsMapMutex);
        std::shared_ptr<T> result = std::make_shared<T>();
        result.get()->load(file_name);
        configs[s] = result;

        return *(result.get());
    }
}

template <class T>
std::string SystemConfig<T>::getConfigFileName(const std::string& s)
{
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
template <class T>
int SystemConfig<T>::getOwnRobotID()
{
    return SystemConfig::getRobotID(this->getHostname());
}

/**
 * Looks up the robot's ID with the given name.
 * @return The robot's ID
 * <deprecated>
 */
template <class T>
int SystemConfig<T>::getRobotID(const string& name)
{
    // TODO this should be optional for dynamic teams (is it ok to return ints?)
    IAlicaConfig& tmp = (*this)["Globals"];
    int ownRobotID = tmp["Globals"]["Team"][name]["ID"].as<int>();
    return ownRobotID;
}

template <class T>
string SystemConfig<T>::getRootPath()
{
    return rootPath;
}

template <class T>
string SystemConfig<T>::getConfigPath()
{
    return configPath;
}

template <class T>
string SystemConfig<T>::getLogPath()
{
    return logPath;
}

template <class T>
string SystemConfig<T>::getHostname()
{
    return hostname;
}

template <class T>
void SystemConfig<T>::setHostname(const std::string& newHostname)
{
    hostname = newHostname;
    configs.clear();
    cout << "SC: Update Hostname:       \"" << hostname << "\"" << endl;
}

template <class T>
void SystemConfig<T>::setRootPath(string rootPath)
{
    if (!essentials::FileSystem::endsWith(rootPath, essentials::FileSystem::PATH_SEPARATOR)) {
        rootPath = rootPath + essentials::FileSystem::PATH_SEPARATOR;
    }
    this->rootPath = rootPath;
    cout << "SC: Update Root:           \"" << rootPath << "\"" << endl;
}

template <class T>
void SystemConfig<T>::setConfigPath(string configPath)
{
    if (!essentials::FileSystem::endsWith(configPath, essentials::FileSystem::PATH_SEPARATOR)) {
        configPath = configPath + essentials::FileSystem::PATH_SEPARATOR;
    }
    this->configPath = configPath;
    cout << "SC: Update ConfigRoot:     \"" << configPath << "\"" << endl;
}

template <class T>
void SystemConfig<T>::resetHostname()
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

template <class T>
string SystemConfig<T>::robotNodeName(const string& nodeName)
{
    return SystemConfig::getHostname() + NODE_NAME_SEPERATOR + nodeName;
}

template <class T>
string SystemConfig<T>::getEnv(const string& var)
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
template class SystemConfig<Configuration>;
} //namespace essentials