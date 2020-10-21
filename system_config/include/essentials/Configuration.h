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

    std::string trimLeft(const std::string& str, const std::string& whitespace = " \t");
    static std::string trim(const std::string& str, const std::string& whitespace = " \t");

    ConfigNode& operator[](const std::string key)
    {
        auto vec = configRoot->findChildren(key);
        if (vec.empty()) {
            std::string errMsg = "SC-Conf: Could not find key: " + key;
            std::cerr << errMsg << std::endl;
            throw std::runtime_error(errMsg);
        }
        return *(vec[0].get());
    }

    template <typename T>
    T get(const char* path)
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
            return util.convert<T>(nodes[0]->getValue());
        }
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
    T tryGet(T d, const char* path)
    {
        IAlicaConfigUtil util;
        std::shared_ptr<std::vector<std::string>> params = util.getParams('.', path);

        std::vector<ConfigNode*> nodes;

        collect(this->configRoot.get(), params.get(), 0, &nodes);

        if (nodes.size() == 0) {
            return d;
        }

        return util.convert<T>(nodes[0]->getValue());
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

    template <typename T>
    void set(T value, const char* path)
    {
        IAlicaConfigUtil util;
        std::shared_ptr<std::vector<std::string>> params = util.getParams('.', path);

        std::vector<ConfigNode*> nodes;

        collect(this->configRoot.get(), params.get(), 0, &nodes);

        for (int i = 0; i < nodes.size(); i++) {
            if (nodes[i]->getType() == ConfigNode::Leaf) {
                nodes[i]->setValue(util.stringify(value));
            }
        }
    }


    /**
     * This method creates the configuration parameter if it not already exists
     */
    template <typename T>
    void setCreateIfNotExistent(T value, const char* path)
    {
        IAlicaConfigUtil util;
        std::shared_ptr<std::vector<std::string>> params_ptr = util.getParams('.', path);

        if (params_ptr->size() < 2) {
            std::string errMsg = "SC-Conf: path cannot be created, not enough params";
            std::cerr << errMsg << std::endl;
            throw std::runtime_error(errMsg);
        }

        std::vector<ConfigNode*> nodes;
        std::vector<std::string>& params = *params_ptr;
        collect(this->configRoot.get(), &params, 0, &nodes);

        if (!nodes.empty()) {
            return;
        }

        ConfigNode* currentNode = this->configRoot.get();
        size_t ind = 0;
        while (ind < params.size()) {
            auto children = currentNode->findChildren(params[ind]);
            if (children.empty()) {
                if (ind == params.size() - 1) {
                    currentNode = currentNode->create(params[ind], util.stringify(value));
                } else {
                    currentNode = currentNode->create(params[ind]);
                }
            } else {
                // We chose the first,  should be the only one !
                currentNode = children[0].get();
            }
            ++ind;
        }
    }
};
}