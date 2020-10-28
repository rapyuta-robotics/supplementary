#include "essentials/Configuration.h"

namespace essentials
{
Configuration::Configuration()
        : filename()
        , configRoot(new ConfigNode("root"))
{
}

Configuration::Configuration(std::string filename)
        : filename(filename)
        , configRoot(new ConfigNode("root"))
{
    load(filename);
}

Configuration::Configuration(std::string filename, const std::string content)
        : filename(filename)
        , configRoot(new ConfigNode("root"))
{
    load(filename, std::shared_ptr<std::istream>(new std::istringstream(content)));
}

ConfigNode& Configuration::operator[](const std::string key)
{
    auto vec = configRoot->findChildren(key);
    if (vec.empty()) {
        std::string errMsg = "SC-Conf: Could not find key: " + key;
        std::cerr << errMsg << std::endl;
        throw std::runtime_error(errMsg);
    }
    return *(vec[0].get());
}

ConfigNode* Configuration::findNode(ConfigNode& node, const char* path, va_list args)
{
    ConfigNode* configNode = &node;
    std::vector<std::string> params;

    const char* temp = path;
    while (temp != NULL) {
        configNode = &(configNode->operator[](trim(temp)));
        temp = va_arg(args, const char*);
    }
    return configNode;
}

void Configuration::load(std::string filename, std::shared_ptr<std::istream> content)
{
    this->filename = filename;

    int linePos = 0;
    int chrPos = 0;

    std::string line;

    ConfigNode* currentNode = this->configRoot.get();

    while (content->good()) {
        getline(*content, line);
        line = Configuration::trimLeft(line);

        int lineLen = line.length(); // size();
        if (lineLen == 0)
            continue;
        chrPos = 1;

        linePos++;

        while (chrPos < lineLen - 1) // -1 for ignoring newline or null-terminating character
        {
            switch (line[0]) {
            case '#': {
                std::string comment = line.substr(1, line.size() - 1);

                comment = Configuration::trim(comment);
                currentNode->create(ConfigNode::Comment, comment);

                chrPos += line.size() - 1;
            }
                continue;

            case '<':
            case '[': {
                size_t end = line.find(']');

                if (end == std::string::npos) {
                    end = line.find('>');
                }

                if ((line.size() < 2) || (end == std::string::npos)) {
                    std::cerr << "Parse error in " << filename << ", line " << linePos << " character " << chrPos << ": malformed tag!" << std::endl;
                    throw std::exception();
                }

                if (end - 1 == 0) {
                    std::cerr << "Parse error in " << filename << ", line " << linePos << " character " << chrPos << ": malformed tag, tag name empty!"
                              << std::endl;
                    throw std::exception();
                }

                std::string name = line.substr(1, end - 1);
                if ((name[0] == '/') || (name[0] == '!')) {
                    if (currentNode == NULL) {
                        std::cerr << "Parse error in " << filename << ", line " << linePos << " character " << chrPos << ": no opening tag found!" << std::endl;
                        throw std::exception();
                    }

                    if (name.compare(1, name.size() - 1, currentNode->getName()) != 0) {
                        std::cerr << "Parse error in " << filename << ", line " << linePos << " character " << chrPos
                                  << ": closing tag does not match opening tag!" << std::endl;
                        throw std::exception();
                    }

                    currentNode = currentNode->getParent();
                } else {
                    ConfigNode* x = currentNode->create(name);
                    currentNode = x;
                }

                if (end < line.size() - 1) {
                    line = line.substr(end + 1, line.size() - end - 1);
                }

                chrPos += (end + 1);
            } break;

            default:
                chrPos++;

                if ((line[0] != ' ') && (line[0] != '\t')) {
                    size_t curPos = 0;
                    bool inString = false;

                    std::ostringstream ss;

                    while (curPos < line.size()) {
                        if (line[curPos] == '"') {
                            inString = !inString;
                            curPos++;
                        }

                        if (curPos < line.size()) {
                            ss << line[curPos];
                            curPos++;
                        }
                    }

                    line = (curPos >= line.size() - 1 ? "" : line.substr(curPos + 1, line.size() - curPos - 1));

                    chrPos += (curPos - 1);

                    std::string element = ss.str();
                    std::string key;
                    std::string value;

                    size_t eq = element.find('=');

                    if (eq != std::string::npos) {
                        key = element.substr(0, eq);
                        value = element.substr(eq + 1, element.size() - eq - 1);

                        key = Configuration::trim(key);
                        value = Configuration::trim(value);
                    }

                    currentNode->create(key, value);
                } else {
                    line = line.substr(1, line.size() - 1);
                }

                break;
            }
        }
    }

    if (this->configRoot.get() != currentNode) {
        std::cout << "Parse error in " << filename << ", line " << linePos << " character " << line.size() << ": no closing tag found!" << std::endl;
        throw std::exception();
    }
}

void Configuration::serialize_internal(std::ostringstream* ss, ConfigNode* node)
{
    if (node == NULL)
        return;

    if (node->getType() == ConfigNode::Node) {
        *ss << std::string(node->getDepth(), '\t') << "[" << node->getName() << "]" << std::endl;

        for (std::vector<ConfigNodePtr>::iterator itr = node->getChildren()->begin(); itr != node->getChildren()->end(); itr++) {
            serialize_internal(ss, (*itr).get());
        }

        *ss << std::string(node->getDepth(), '\t') << "[!" << node->getName() << "]" << std::endl;

    } else if (node->getType() == ConfigNode::Leaf) {
        *ss << std::string(node->getDepth(), '\t') << node->getName() << " = " << node->getValue() << std::endl;

    } else { // Comment

        *ss << std::string(node->getDepth(), '\t') << "# " << node->getName() << std::endl;
    }
}

void Configuration::store()
{
    if (this->filename.size() > 0) {
        store(this->filename);
    }
}

void Configuration::store(std::string filename)
{
    std::ostringstream ss;
    std::ofstream os(filename.c_str(), std::ios_base::out);

    serialize_without_root(&ss, this->configRoot.get());

    os << ss.str();
}

std::string Configuration::serialize()
{
    std::ostringstream ss;
    serialize_internal(&ss, this->configRoot.get());
    return ss.str();
}

void Configuration::serialize_without_root(std::ostringstream* ss, ConfigNode* node)
{
    if (node == NULL)
        return;

    if (node->getType() == ConfigNode::Node) {
        //*ss << string(node->getDepth(), '\t') << "[" << node->getName() << "]" << endl;

        for (std::vector<ConfigNodePtr>::iterator itr = node->getChildren()->begin(); itr != node->getChildren()->end(); itr++) {
            serialize_internal(ss, (*itr).get());
        }

        //*ss << string(node->getDepth(), '\t') << "[!" << node->getName() << "]" << endl;

    } else if (node->getType() == ConfigNode::Leaf) {
        *ss << std::string(node->getDepth(), '\t') << node->getName() << " = " << node->getValue() << std::endl;

    } else { // Comment

        *ss << std::string(node->getDepth(), '\t') << "# " << node->getName() << std::endl;
    }
}

void Configuration::collect(ConfigNode* node, std::vector<std::string>* params, size_t offset, std::vector<ConfigNode*>* result)
{
    std::vector<ConfigNodePtr>* children = node->getChildren();
    if (offset == params->size()) {
        result->push_back(node);
        return;
    }
    for (size_t i = offset; i < params->size(); i++) {
        bool found = false;

        for (size_t j = 0; j < children->size(); j++) {
            if ((*children)[j]->getName().compare((*params)[i]) == 0) {
                collect((*children)[j].get(), params, offset + 1, result);
                found = true;
            }
        }

        if (!found) {
            return;
        }
    }
}

void Configuration::collectSections(ConfigNode* node, std::vector<std::string>* params, size_t offset, std::vector<ConfigNode*>* result)
{
    std::vector<ConfigNodePtr>* children = node->getChildren();

    if (offset == params->size()) {
        for (unsigned int i = 0; i < children->size(); i++) {
            result->push_back((*children)[i].get());
        }
        return;
    }

    for (size_t i = offset; i < params->size(); i++) {
        bool found = false;
        for (size_t j = 0; j < children->size(); j++) {
            if ((*children)[j]->getName().compare((*params)[i]) == 0) {
                collectSections((*children)[j].get(), params, offset + 1, result);
                found = true;
            }
        }
        if (!found)
            return;
    }
}

/**
 * Creates a suitable error message, if the given path was not found.
 * @param params The path which does not exist.
 * @return The corresponding error message.
 */
std::string Configuration::pathNotFound(std::vector<std::string>* params)
{
    std::ostringstream os;
    if ((params == NULL) || (params->size() == 0)) {
        os << "Empty path not found in " << this->filename << "!" << std::endl;
    } else {
        os << "Configuration: Path '" << (*params)[0];
        for (size_t i = 1; i < params->size(); i++) {
            os << "." << (*params)[i];
        }
        os << "' not found in " << this->filename << "!" << std::endl;
    }
    return os.str();
}

std::string Configuration::tryGetString(std::string d, const char* path, ...)
{
    va_list args;
    va_start(args, path);
    return tryGet<std::string>(d, path, args);
}

int Configuration::tryGetInt(int d, const char* path, ...)
{
    va_list args;
    va_start(args, path);
    return tryGet<int>(d, path, args);
}

bool Configuration::tryGetBool(bool d, const char* path, ...)
{
    va_list args;
    va_start(args, path);
    return tryGet<bool>(d, path, args);
}

float Configuration::tryGetFloat(float d, const char* path, ...)
{
    va_list args;
    va_start(args, path);
    return tryGet<float>(d, path, args);
}

double Configuration::tryGetDouble(double d, const char* path, ...)
{
    va_list args;
    va_start(args, path);
    return tryGet<double>(d, path, args);
}

unsigned short Configuration::tryGetUShort(unsigned short d, const char* path, ...)
{
    va_list args;
    va_start(args, path);
    return tryGet<unsigned short>(d, path, args);
}

void Configuration::setCreateIfNotExistentString(std::string value, const char* path, ...)
{
    va_list args;
    va_start(args, path);
    setCreateIfNotExistent<std::string>(value, path, args);
}

void Configuration::setCreateIfNotExistentInt(int value, const char* path, ...)
{
    va_list args;
    va_start(args, path);
    setCreateIfNotExistent<int>(value, path, args);
}

void Configuration::setCreateIfNotExistentBool(bool value, const char* path, ...)
{
    va_list args;
    va_start(args, path);
    setCreateIfNotExistent<bool>(value, path, args);
}

void Configuration::setCreateIfNotExistentUShort(unsigned short value, const char* path, ...)
{
    va_list args;
    va_start(args, path);
    setCreateIfNotExistent<unsigned short>(value, path, args);
}

void Configuration::setCreateIfNotExistentFloat(float value, const char* path, ...)
{
    va_list args;
    va_start(args, path);
    setCreateIfNotExistent<float>(value, path, args);
}

void Configuration::setCreateIfNotExistentDouble(double value, const char* path, ...)
{
    va_list args;
    va_start(args, path);
    setCreateIfNotExistent<double>(value, path, args);
}

/**
 * Allows to collect the names of all sections in the given path. It does not include the keys of key-value pair in the
 * sections.
 * @param path
 * @return A vector with the names of all sections in the given path.
 */
std::shared_ptr<std::vector<std::string>> Configuration::getSections(const char* path)
{
    IAlicaConfigUtil util;
    std::shared_ptr<std::vector<std::string>> params = util.getParams('.', path);

    std::vector<ConfigNode*> nodes;

    collectSections(this->configRoot.get(), params.get(), 0, &nodes);

    std::shared_ptr<std::vector<std::string>> result(new std::vector<std::string>());

    if (nodes.size() == 0) {
        std::cerr << pathNotFound(params.get()) << std::endl;
        throw std::exception();
    }

    for (unsigned int i = 0; i < nodes.size(); i++) {
        if (nodes[i]->getType() == ConfigNode::Node) {
            result->push_back(nodes[i]->getName());
        }
    }

    return result;
}

/**
 * Allows to collect all keys in the given path. It does not include section names.
 * @param path
 * @return A vector with all keys or names of the given path.
 */
std::shared_ptr<std::vector<std::string>> Configuration::getNames(const char* path)
{
    IAlicaConfigUtil util;
    std::shared_ptr<std::vector<std::string>> params = util.getParams('.', path);

    std::vector<ConfigNode*> nodes;

    collectSections(this->configRoot.get(), params.get(), 0, &nodes);

    std::shared_ptr<std::vector<std::string>> result(new std::vector<std::string>());

    if (nodes.size() == 0) {
        std::cerr << pathNotFound(params.get()) << std::endl;
        throw std::exception();
    }

    for (size_t i = 0; i < nodes.size(); i++) {
        if (nodes[i]->getType() == ConfigNode::Leaf) {
            result->push_back(nodes[i]->getName());
        }
    }
    return result;
}

std::shared_ptr<std::vector<std::string>> Configuration::tryGetSections(std::string d, const char* path)
{
    IAlicaConfigUtil util;
    std::shared_ptr<std::vector<std::string>> params = util.getParams('.', path);

    std::vector<ConfigNode*> nodes;

    collectSections(this->configRoot.get(), params.get(), 0, &nodes);

    std::shared_ptr<std::vector<std::string>> result(new std::vector<std::string>());

    if (nodes.size() == 0) {
        result->push_back(d);
        return result;
    }

    for (size_t i = 0; i < nodes.size(); i++) {
        if (nodes[i]->getType() == ConfigNode::Node) {
            result->push_back(nodes[i]->getName());
        }
    }

    return result;
}

/**
 * Allows to collect all keys in the given path. Does not include child section names.
 * @param d the default value which should be returned if the given path does not exist.
 * @param path Specifies the level inside the configuration, at which the names of the child sections should be
 * collected.
 * @return The names of the sections at the given path
 */
std::shared_ptr<std::vector<std::string>> Configuration::tryGetNames(std::string d, const char* path)
{
    IAlicaConfigUtil util;
    std::shared_ptr<std::vector<std::string>> params = util.getParams('.', path);

    std::vector<ConfigNode*> nodes;

    collectSections(this->configRoot.get(), params.get(), 0, &nodes);

    std::shared_ptr<std::vector<std::string>> result(new std::vector<std::string>());

    if (nodes.size() == 0) {
        result->push_back(d);
        return result;
    }

    for (size_t i = 0; i < nodes.size(); i++) {
        if (nodes[i]->getType() == ConfigNode::Leaf) {
            result->push_back(nodes[i]->getName());
        }
    }
    return result;
}

/**
 * Removes the given whitespaces at the beginning of the string.
 * @param str The string which should be trimmed.
 * @param whitespace The whitespaces which should be removed.
 * @return The trimmed string.
 */
std::string Configuration::trimLeft(const std::string& str, const std::string& whitespace)
{
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos) {
        return ""; // no content
    }
    return str.substr(strBegin, str.length() - strBegin);
}

/**
 * Removes the given whitespaces at the beginning and the end of the string.
 * @param str The string which should be trimmed.
 * @param whitespace The whitespaces which should be removed.
 * @return The trimmed string.
 */
std::string Configuration::trim(const std::string& str, const std::string& whitespace)
{
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos) {
        return ""; // no content
    }
    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}


} // namespace essentials
