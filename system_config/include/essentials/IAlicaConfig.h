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

#include "IAlicaConfigUtil.h"
#include "ConfigNode.h"

namespace essentials
{
    class IAlicaConfig
    {
    public:
        /**
         * Create config object with empty filename.
         */
        IAlicaConfig(){};

        /**
        * Destroys AlicaContext object.
        */
        virtual ~IAlicaConfig(){};

        /**
         * Create a config object and load the configuration file with the given filename.
         *
         * @param filename The name of the configuration file to be loaded.
         */
        IAlicaConfig(std::string filename){};

        /**
         * @param path to the value
         *
         * @return A vector of values with type T, with the exact path in the config file (first value with matching path)
         */
        template <typename T>
        std::vector<T> getList(const char* path...);

        /**
         * @param path The path where to look for a value.
         *
           @return A pointer to a vector with all values with type T, with the exact path in the config file (all values with matching path).
         */
        template <typename T>
        std::shared_ptr<std::vector<T>> getAll(const char* path...);

       /**
        * @param d Value with type T to return if no value with the provided path is present in the config.
        * @param path The path to the value.
        *
        * @return Value of type T with the exact path in the config file or d.
        */
        virtual std::string tryGetString(std::string d, const char* path, ...) = 0;
        virtual bool tryGetBool(bool d, const char* path, ...) = 0;
        virtual int tryGetInt(int d, const char* path, ...) = 0;
        virtual float tryGetFloat(float d, const char* path, ...) = 0;
        virtual double tryGetDouble(double d, const char* path, ...) = 0;
        virtual unsigned short tryGetUShort(unsigned short d, const char* path, ...) = 0;

        /**
         * @param d Value with type T to return in a vector if no value with the given path is present in the config.
         * @param path The path where to look for the value.
         *
         * @return A pointer to a vector with all values with a matching path or a vector with only d inside.
         */
        template <typename T>
        std::shared_ptr<std::vector<T>> tryGetAll(T d, const char* path...);

        /**
         * Creates a path with the given value if the path does not already exist.
         *
         * @param value The value to set in the config.
         * @param path The path to create if not existent.
         */
        virtual void setCreateIfNotExistentString(std::string value, const char* path, ...) = 0;
        virtual void setCreateIfNotExistentBool(bool value, const char* path, ...) = 0;
        virtual void setCreateIfNotExistentInt(int value, const char* path, ...) = 0;
        virtual void setCreateIfNotExistentFloat(float value, const char* path, ...) = 0;
        virtual void setCreateIfNotExistentDouble(double value, const char* path, ...) = 0;
        virtual void setCreateIfNotExistentUShort(unsigned short value, const char* path, ...) = 0;

        /**
         * Load a configuration with the given filename.
         *
         * @param filename Name of the configuration file.
         */
        virtual void load(std::string filename) = 0;

        /**
         * Store config status in a file with the filename already stored in the object.
         */
        virtual void store() = 0;

        /**
         * Get a list with the names of all sections in the given path.
         *
         * @param path The path of the section.
         *
         * @return A pointer to a vector of names of all elements of the section.
         */
        virtual std::shared_ptr<std::vector<std::string>> getSections(const char* path) = 0;

        /**
         * Get a list with the names of all sections in the given path.
         * If there are no sections in the given path, a vector with the
         * given String d is returned.
         *
         * @param d String value to return in a vector if there are no sections in the given path.
         * @param const char* path The path to look for sections.
         *
         * @return A vector with the names of all sections in the given path or a vector with d.
         */
        virtual std::shared_ptr<std::vector<std::string>> tryGetSections(std::string d, const char* path) = 0;

        /**
         * Collect all keys in a given path. It does not include section names.
         *
         * @param path The path where keys should be collected.
         *
         * @return A vector with all keys of the given path.
         */
        virtual std::shared_ptr<std::vector<std::string>> getNames(const char* path) = 0;

        /**
         * Collect all keys in a given path. It does not include section names.
         * If the path does not exist, a pointer to a vector with d is returned.
         *
         * @param d Default value to return if the given path does not exist.
         * @param path The path where to look for keys.
         *
         * @return The name of all keys at the given path or a vector with d.
         */
        virtual std::shared_ptr<std::vector<std::string>> tryGetNames(std::string d, const char* path) = 0;

        virtual ConfigNode& operator[](const std::string key) = 0;
    };
} /* namespace essentials */