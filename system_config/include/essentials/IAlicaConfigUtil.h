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
    };
}
