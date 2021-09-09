#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>
//#include <filesystem>

#include "ConfigReader.hpp"
//#include "logging/loguru.hpp"

ConfigReader::ConfigReader(std::string& workDirPathArg)
{
    // Set work dir path as attribute of ConfigReader
    workDirPath = workDirPathArg;

    // Set full path to config file
    pathToConfig = workDirPath + "/" + configFileName;
}

void ConfigReader::readConfigFile(std::string module)
{
    // logging
    std::ostringstream log;
    log << "\n** " <<  __DATE__ << " | " << __TIME__ << " | " << __FILE__ << "[" << __func__ << "]" << ":" << __LINE__ << " | ";
    log << "Reading " << module << " config file: " << pathToConfig;
    std::cout << log.str();
    //LOG_F(INFO, &(log.str()[0]));
    
    // open file
    std::ifstream configFile(pathToConfig);

    // Extract information of the corresponding module
    bool inSection = false;

    // build customized regex
    std::ostringstream streamRE;
    streamRE << "^\\[" << module << "\\]$";
    std::regex reSection (streamRE.str()); // check if where are in module section
    std::regex reOutSection ("^\\[[\\w\\s]+\\]$"); // check if leave module section
    std::regex reGetParam ("^(.+) = (.+)$"); // capture key and value

    std::string line;
    while(std::getline(configFile, line))
    {
        // if enter in the corresponding section --> inSection = true and continue
        if (std::regex_search(line, reSection))
        {
            inSection = true;
            continue;
        }

        // if inSection is true but entered in other section, break while
        if (inSection && std::regex_search(line, reOutSection))
        {
            break;
        }

        // if inSection == true, get information
        if (inSection && std::regex_search(line, reGetParam))
        {
            std::smatch matchObject;
            std::regex_search(line, matchObject, reGetParam);
            params.insert(std::pair<std::string, std::string>(matchObject[1], matchObject[2]));

            // logging parameters
            log.str("");
            log << matchObject[1] << ": " << matchObject[2];
            //LOG_F(INFO, &(log.str()[0]));

            continue;
        }
    }

    log.str("");
    log << "\n** " <<  __DATE__ << " | " << __TIME__ << " | " << __FILE__ << "[" << __func__ << "]" << ":" << __LINE__ << " | ";
    std::cout << log.str();
    log << "Config file was read";
    //LOG_F(INFO, &(log.str()[0]));
}

std::string ConfigReader::getValue(std::string key)
{
    std::string value = params[key];
    return value;
}