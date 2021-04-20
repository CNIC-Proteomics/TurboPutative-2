#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <filesystem>
#include <algorithm>
#include <cctype>

#include "LipidList_and_PreProcess.hpp"
#include "boost/regex.hpp"
#include "../../lib/logging/loguru.hpp"

//
// LIPID LIST
//

LipidList::LipidList()
{
    std::ifstream lipidListFile(lipidListPath);

    std::string line;
    while(std::getline(lipidListFile, line))
    {
        lipidList.push_back(line);
    }
}


std::vector<bool> LipidList::findLipids(std::vector<std::string>& compoundNames)
{
    std::stringstream log;
    log << "Find Goslin lipids";
    LOG_F(INFO, &(log.str()[0]));

    std::vector<bool> isGoslinLipidVector;

    for (std::string& compound : compoundNames)
    {
        bool isGoslinLipid = false;
        for (std::string& lipid : lipidList)
        {
            boost::regex lipidRE("^" + lipid, boost::regex::icase);
            
            if (boost::regex_search(compound, lipidRE))
            {
                isGoslinLipid = true;
                break;
            }
        }

        isGoslinLipidVector.push_back(isGoslinLipid);
    }

    return isGoslinLipidVector;
}


//
//  PRE_PROCESS_LIPID
//

void PreProcessLipids::preProcessLipids(std::vector<std::string>& compoundNames)
{
    std::stringstream log;
    log << "Pre-process Goslin Lipids";
    LOG_F(INFO, &(log.str()[0]));

    for (std::string& compound : compoundNames)
    {
        for (std::map<std::string, std::string>& RE : REs)
        {
            boost::regex detect (RE["regex"]);
            compound = boost::regex_replace(compound, detect, RE["replace"]);
        }
    }
}