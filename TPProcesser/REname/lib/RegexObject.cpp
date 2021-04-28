#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <filesystem>
#include "boost/regex.hpp"
#include "RegexObject.hpp"
#include "../../lib/logging/loguru.hpp"


#define REGEX_INI_PATH "TPProcesser\\REname\\data\\regex.ini"

void RegexObject::readRegexINI()
{
    // logging
    std::stringstream log;
    log << "Reading regex.ini";
    LOG_F(INFO, &(log.str()[0]));

    std::filesystem::path regexINIPath = REGEX_INI_PATH;

    // open regex.ini file and extract information
    std::ifstream regexFile(regexINIPath);
    std::string line;
    int p=0;
    boost::regex getValues("^\\w+\\s*=\\s*(.*)$");

    while(std::getline(regexFile, line))
    {
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
    log << "Applying regular expressions";
    LOG_F(INFO, &(log.str()[0]));

    int index = -1;
    for (std::string& compound : compoundNamesColumn)
    {
        // Apply regular expression to compounds that were not mapped nor processed by Goslin
        index++;
        if (std::find(mappedIndex.begin(), mappedIndex.end(), index) != mappedIndex.end()) continue;

        for (int  i=0; i<REs.size(); i++)
        {
            compound = boost::regex_replace(compound, REs[i], replaces[i], boost::format_perl);
        }

        // Strip compound name to remove whitespace
        compound = boost::regex_replace(compound, boost::regex("^\\s+"), "", boost::format_perl);
        compound = boost::regex_replace(compound, boost::regex("\\s+$"), "", boost::format_perl);       
    }

    log.str("");
    log << "Regular expressions were applied";
    LOG_F(INFO, &(log.str()[0]));

}