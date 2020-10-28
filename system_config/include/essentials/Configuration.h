#pragma once

#include <algorithm>
#include <cstdarg>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <vector>

#include "IAlicaConfig.h"
#include "ConfigNode.h"
#include "IAlicaConfigUtil.h"

namespace essentials
{
class Configuration : public IAlicaConfig
{
protected:
    static const char LIST_ELEMENT_SEPERATOR = ',';
    std::string filename;
    void collect(ConfigNode* node, std::vector<std::string>* params, size_t offset, std::vector<ConfigNode*>* result);
    void collectSections(ConfigNode* node, std::vector<std::string>* params, size_t offset, std::vector<ConfigNode*>* result);
    std::string pathNotFound(std::vector<std::string>* params);

    ConfigNodePtr configRoot;

    void serialize_internal(std::ostringstream* ss, ConfigNode* node);
    void serialize_without_root(std::ostringstream* ss, ConfigNode* node);

public:
    Configuration();
    Configuration(std::string filename);
    Configuration(std::string filename, const std::string content);

    inline void load(std::string filename)
    {
        load(filename, std::shared_ptr<std::ifstream>(new std::ifstream(filename.c_str(), std::ifstream::in)));
    }

    void load(std::string filename, std::shared_ptr<std::istream> content);

    void store();
    void store(std::string filename);

    std::string serialize();

    std::shared_ptr<std::vector<std::string>> getSections(const char* path);
    std::shared_ptr<std::vector<std::string>> tryGetSections(std::string d, const char* path);
    std::shared_ptr<std::vector<std::string>> getNames(const char* path);
    std::shared_ptr<std::vector<std::string>> tryGetNames(std::string d, const char* path);

    std::string tryGetString(std::string d, const char* path, ...);
    bool tryGetBool(bool d, const char* path, ...);
    int tryGetInt(int d, const char* path, ...);
    float tryGetFloat(float d, const char* path, ...);
    double tryGetDouble(double d, const char* path, ...);
    unsigned short tryGetUShort(unsigned short d, const char* path, ...);

    void setCreateIfNotExistentString(std::string value, const char* path, ...);
    void setCreateIfNotExistentBool(bool value, const char* path, ...);
    void setCreateIfNotExistentInt(int value, const char* path, ...);
    void setCreateIfNotExistentFloat(float value, const char* path, ...);
    void setCreateIfNotExistentDouble(double value, const char* path, ...);
    void setCreateIfNotExistentUShort(unsigned short value, const char* path, ...);

    std::string trimLeft(const std::string& str, const std::string& whitespace = " \t");
    std::string trim(const std::string& str, const std::string& whitespace = " \t");

    ConfigNode& operator[](const std::string key);
    ConfigNode* findNode(ConfigNode& node, const char* path, va_list args);

    template <typename T>
    T tryGet(T d, const char* path, va_list args)
    {
        T result;
        try {
            result = findNode(*configRoot.get(), path, args)->as<T>();
        } catch (std::runtime_error) {
            result = d;
        }

        return result;
    }

    template <typename T>
    std::vector<T> getList(const char* path)
    {
        IAlicaConfigUtil util;
        std::shared_ptr<std::vector<std::string>> params = util.getParams('.', path);
        std::vector<ConfigNode*> nodes;

        collect(this->configRoot.get(), params.get(), 0, &nodes);

        if (nodes.size() == 0) {
            std::string errMsg = "SC-Conf: " + pathNotFound(params.get());
            std::cerr << errMsg << std::endl;
            throw std::runtime_error(errMsg);
        } else {
            return util.convertList<T>(nodes[0]->getValue());
        }
    }

    template <typename T>
    std::shared_ptr<std::vector<T>> getAll(const char* path)
    {
        IAlicaConfigUtil util;
        std::shared_ptr<std::vector<std::string>> params = util.getParams('.', path);
        std::vector<ConfigNode*> nodes;

        collect(this->configRoot.get(), params.get(), 0, &nodes);

        if (nodes.size() == 0) {
            std::string errMsg = "SC-Conf: " + pathNotFound(params.get());
            std::cerr << errMsg << std::endl;
            throw std::runtime_error(errMsg);
        } else {
            std::shared_ptr<std::vector<T>> result(new std::vector<T>());

            for (int i = 0; i < nodes.size(); i++) {
                result->push_back(util.convert<T>(nodes[i]->getValue()));
            }

            return result;
        }
    }

    template <typename T>
    std::shared_ptr<std::vector<T>> tryGetAll(T d, const char* path)
    {
        IAlicaConfigUtil util;
        std::shared_ptr<std::vector<std::string>> params = util.getParams('.', path);

        std::vector<ConfigNode*> nodes;

        collect(this->configRoot.get(), params.get(), 0, &nodes);

        std::shared_ptr<std::vector<T>> result(new std::vector<T>());

        if (nodes.size() == 0) {

            result->push_back(d);

            return result;
        }

        for (int i = 0; i < nodes.size(); i++) {
            result->push_back(util.convert<T>(nodes[i]->getValue()));
        }

        return result;
    }

    /**
     * This method creates the configuration parameter if it not already exists
     */
    template <typename T>
    void setCreateIfNotExistent(T value, const char* path, va_list args)
    {
        IAlicaConfigUtil util;
        ConfigNode* configNode = configRoot.get();

        const char* temp = path;
        while (temp != NULL) {
            if (!configNode->findChildren(temp).empty()) {
                configNode = &(configNode->operator[](trim(temp)));
                temp = va_arg(args, const char*);
            } else {
                configNode = configNode->create(temp);
                temp = va_arg(args, const char*);
            }
        }
        *configNode = util.stringify<T>(value);
    }
};
}