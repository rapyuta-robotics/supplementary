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

namespace essentials
{
    class IAlicaConfig
    {
    public:
        /**
         * Create config object with empty filename.
         */
        IAlicaConfig();

        /**
        * Destroys AlicaContext object.
        */
        virtual ~IAlicaConfig();

        /**
         * Create a config object and load the configuration file with the given filename.
         *
         * @param filename The name of the configuration file to be loaded.
         */
        IAlicaConfig(std::string filename);

        /**
         * Create a configuration with the given filename and the given config content.
         *
         * @param filename The name of the configuration.
         * @param content The content of the configuration.
         */
        IAlicaConfig(std::string filename, const std::string content);

        /**
         * @param path The path where to look for a value.
         *
         * @return Value with type T with the given path in the config file. Only return the first value with the given path.
         */
        template <typename T>
        T get (const char* path);

        /**
         * @param path to the value
         *
         * @return A vector of values with type T, with the exact path in the config file (first value with matching path)
         */
        template <typename T>
        std::vector<T> getList(const char* path);

        /**
         * @param path The path where to look for a value.
         *
           @return A pointer to a vector with all values with type T, with the exact path in the config file (all values with matching path).
         */
        template <typename T>
        std::shared_ptr<std::vector<T>> getAll(const char* path);

       /**
        * @param d value with type T to return if no value with the provided path is present in the config.
        * @param path The path to the value.
        *
        * @return Value of type T with the exact path in the config file or d.
        */
        template <typename T>
        T tryGet(T d, const char* path);

        /**
         * @param d Value with type T to return in a vector if no value with the given path is present in the config.
         * @param path The path where to look for the value.
         *
         * @return A pointer to a vector with all values with a matching path or a vector with only d inside.
         */
        template <typename T>
        std::shared_ptr<std::vector<T>> tryGetAll(T d, const char* path);

        /**
         * Sets a value at the desired path if the path already exists.
         *
         * @param value The value to set in the config.
         * @param path The path where to set the given value.
         */
        template <typename T>
        void set(T value, const char* path);

        /**
         * Creates a path with the given value if the path does not already exist.
         *
         * @param value The value to set in the config.
         * @param path The path to create if not existent.
         */
        template <typename T>
        void setCreateIfNotExistent(T value, const char* path);

        /**
         * Load a configuration with the given filename.
         *
         * @param filename Name of the configuration file.
         */
        virtual void load(std::string filename) = 0;

        /**
         * Load a configuration file with the given filename and the given content.
         *
         * @param filename The name of the configuration file.
         * @param content The content of the configuration file.
         */
        virtual void load(std::string filename, std::shared_ptr<std::istream> content) = 0;

        /**
         * Store config status in a file with the filename already stored in the object.
         */
        virtual void store() = 0;

        /**
         * Store config status in a file with the given filename.
         *
         * @param filename The name of the configuration file.
         */
        virtual void store(std::string filename) = 0;

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
    };

    //Implementations
    template <typename T>
    T get (const char* path) {

    }

    template <typename T>
    std::vector<T> getList(const char* path) {

    }

    template <typename T>
    std::shared_ptr<std::vector<T>> getAll(const char* path) {

    }

    template <typename T>
    T tryGet(T d, const char* path) {

    }

    template <typename T>
    std::shared_ptr<std::vector<T>> tryGetAll(T d, const char* path) {

    }

    template <typename T>
    void setCreateIfNotExistent(T value, const char* path) {

    }

    template <typename T>
    void set(T value, const char* path) {

    }
} /* namespace essentials */