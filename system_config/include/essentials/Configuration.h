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
        load(filename, std::shared_ptr<std::ifstream>(new std::ifstream(filename.c_str(), std::ifstream::in)), false, false);
    }

    void load(std::string filename, std::shared_ptr<std::istream> content, bool create, bool replace);

    void store();
    void store(std::string filename);

    std::string serialize();

    std::string trimLeft(const std::string& str, const std::string& whitespace = " \t");
    static std::string trim(const std::string& str, const std::string& whitespace = " \t");

    std::shared_ptr<std::vector<std::string>> getSections(const char* path, ...);
    std::shared_ptr<std::vector<std::string>> tryGetSections(std::string d, const char* path, ...);
    std::shared_ptr<std::vector<std::string>> getNames(const char* path, ...);
    std::shared_ptr<std::vector<std::string>> tryGetNames(std::string d, const char* path, ...);

    std::string get(const char* path) {
        IAlicaConfigUtil util;
        std::shared_ptr<std::vector<std::string>> params = util.getParams('.', path);
        std::vector<ConfigNode*> nodes;

        collect(this->configRoot.get(), params.get(), 0, &nodes);

        if (nodes.size() == 0) {
            std::string errMsg = "SC-Conf: " + pathNotFound(params.get());
            std::cerr << errMsg << std::endl;
            throw std::runtime_error(errMsg);
        } else {
            return nodes[0]->getValue();
        }
    }
}
