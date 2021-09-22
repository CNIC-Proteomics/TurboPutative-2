#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
// #include <filesystem>
#include "boost/regex.hpp"
#include "RegexObject.hpp"
//#include "../../lib/logging/loguru.hpp"


// #define REGEX_INI_PATH "./src/TurboPutative-2.0-built/TPProcesser/REname/data/regex.ini"

void RegexObject::readRegexINI(std::string regex_ini_path)
{
    // logging
    std::stringstream log;
    log << "\n** " <<  __DATE__ << " | " << __TIME__ << " | " << __FILE__ << "[" << __func__ << "]" << ":" << __LINE__ << " | ";
    log << "Reading " << regex_ini_path;
    std::cout << log.str();
    //LOG_F(INFO, &(log.str()[0]));

    std::string regexINIPath = regex_ini_path;

    // open regex.ini file and extract information
    std::ifstream regexFile(regexINIPath);
    std::string line;
    int p=0;
    boost::regex getValues("^\\w+\\s*=\\s*(.*)$");

    while(std::getline(regexFile, line))
    {

        //std::cout << line << std::endl;
        boost::smatch matchObject;

        // skip commented lines
        if (boost::regex_search(line, boost::regex("^\\s*#"))) continue;

        // if find section opening [section] --> p = 0
        if (p == 0 && boost::regex_search(line, boost::regex("^\\[[\\w\\s-]+\\]")))
        {
            p = 1;
            continue;
        }

        // if p == 1 and find = --> Save regex
        if (p == 1 && boost::regex_search(line, matchObject, getValues))
        {
            std::string REstr = matchObject[1];
            boost::regex RE_tmp(REstr, boost::regex::perl);
            REs.push_back(RE_tmp);
            p = 2;
            continue;
        }

        // p == 2 and find = --> Save replace
        if (p == 2 && boost::regex_search(line, matchObject, getValues))
        {
            std::string replace_str = matchObject[1];
            replaces.push_back(replace_str);
            p = 0;
            continue;
        }
    }
}


void RegexObject::applyRegex(std::vector<std::string>& compoundNamesColumn, std::vector<int>& mappedIndex)
{
    // logging
    std::stringstream log;
    log << "\n** " <<  __DATE__ << " | " << __TIME__ << " | " << __FILE__ << "[" << __func__ << "]" << ":" << __LINE__ << " | ";
    log << "Applying regular expressions";
    std::cout << log.str();
    // LOG_F(INFO, &(log.str()[0]));

    int index = -1;
    for (std::string& compound : compoundNamesColumn)
    {
        std::string compoundOriginal = compound;

        // Apply regular expression to compounds that were not mapped nor processed by Goslin
        index++;
        if (std::find(mappedIndex.begin(), mappedIndex.end(), index) != mappedIndex.end()) continue;

        for (int  i=0; i<REs.size(); i++)
        {
            compound = boost::regex_replace(compound, REs[i], replaces[i], boost::format_perl);
        }

        // if regex was applied to compound, add to map index to avoid future regex set
        if (compound != compoundOriginal) mappedIndex.push_back(index);

        // Strip compound name to remove whitespace
        compound = boost::regex_replace(compound, boost::regex("^\\s+"), "", boost::format_perl);
        compound = boost::regex_replace(compound, boost::regex("\\s+$"), "", boost::format_perl);       
    }

    log.str("");
    log << "\n** " <<  __DATE__ << " | " << __TIME__ << " | " << __FILE__ << "[" << __func__ << "]" << ":" << __LINE__ << " | ";
    log << "Regular expressions were applied";
    std::cout << log.str();
    // LOG_F(INFO, &(log.str()[0]));

}