#pragma once

namespace essentials
{
    class IAlicaConfigUtil
    {
    public:
        /**
         * Splits the given strings if it finds the given seperator.
         * @param seperator
         * @param path
         * @return The list of strings after everything was splitted.
         */
        std::shared_ptr<std::vector<std::string>> getParams(char seperator, const char* path)
        {
            std::shared_ptr<std::vector<std::string>> params = std::make_shared<std::vector<std::string>>();
            if (path != NULL) {
                const char* temp = path;
                std::string::size_type p = 0;
                std::string::size_type q;
                std::string charString = temp;
                while ((q = charString.find(seperator, p)) != std::string::npos) {
                    //					cout << "SC-Conf: Adding-InLoop: '" << string(temp, p, q-p) <<
                    //"'"
                    //<<  endl;
                    params->emplace_back(temp, p, q - p);
                    p = q + 1;
                }
                //				cout << "SC-Conf: Adding-AfterLoop: '" << string(temp, p, charString.length()-p)
                //<<
                //"'"
                //<<  endl;
                params->emplace_back(temp, p, charString.length() - p);
            }
            return params;
        }

        template <typename Target>
        Target convert(std::string value)
        {
            std::string errMsg = "Configuration: Type not handled! Value to be converted was: " + value;
            std::cerr << errMsg << std::endl;
            throw std::runtime_error(errMsg);
        }

    };

template <>
inline short IAlicaConfigUtil::convert<short>(std::string value)
{
    return stoi(value);
}

template <>
inline unsigned short IAlicaConfigUtil::convert<unsigned short>(std::string value)
{
    return stoul(value);
}

template <>
inline int IAlicaConfigUtil::convert<int>(std::string value)
{
    return stoi(value);
}

template <>
inline unsigned int IAlicaConfigUtil::convert<unsigned int>(std::string value)
{
    return stoul(value);
}

template <>
inline long IAlicaConfigUtil::convert<long>(std::string value)
{
    return stol(value);
}

template <>
inline long double IAlicaConfigUtil::convert<long double>(std::string value)
{
    return stold(value);
}

template <>
inline long long IAlicaConfigUtil::convert<long long>(std::string value)
{
    return stoll(value);
}

template <>
inline unsigned long IAlicaConfigUtil::convert<unsigned long>(std::string value)
{
    return stoul(value);
}

template <>
inline unsigned long long IAlicaConfigUtil::convert<unsigned long long>(std::string value)
{
    return stoull(value);
}

template <>
inline float IAlicaConfigUtil::convert<float>(std::string value)
{
    return stof(value);
}

template <>
inline double IAlicaConfigUtil::convert<double>(std::string value)
{
    return stod(value);
}

template <>
inline std::string IAlicaConfigUtil::convert<std::string>(std::string value)
{
    return value;
}

template <>
inline bool IAlicaConfigUtil::convert<bool>(std::string value)
{
    if ("false" == value || value == "False" || value == "0" || value == "FALSE") {
        return false;
    } else if ("true" == value || value == "True" || value == "1" || value == "TRUE") {
        return true;
    }
    std::string errMsg = "Configuration: unable to parse boolean. Value is: " + value;
    std::cerr << errMsg << std::endl;
    throw std::runtime_error(errMsg);
}
}
