#include <iostream>
#include <vector>
#include <string>
#include <regex>
#include <sstream>

// #include "../../lib/logging/loguru.hpp"
#include "RemoveRow.hpp"

RemoveRow::RemoveRow(std::string regexStr)
{
    removeRE.assign(regexStr, std::regex::ECMAScript | std::regex::icase);
}


std::vector<int> RemoveRow::getIndexes(std::vector<std::string> &compoundNamesColumn)
{
    // logging
    std::stringstream log;
    log << "\n** " <<  __DATE__ << " | " << __TIME__ << " | " << __FILE__ << "[" << __func__ << "]" << ":" << __LINE__ << " | ";
    log << "Analysing rows to be removed";
    std::cout << log.str();
    //LOG_F(INFO, &(log.str()[0]));
    
    std::vector<int> removeIndexes;
    removeIndexes.reserve(compoundNamesColumn.size());

    for (int i=0; i<compoundNamesColumn.size(); i++)
    {
        std::smatch matchObject;
        if (std::regex_search(compoundNamesColumn[i], matchObject, removeRE))
        {
            removeIndexes.push_back(i);
        }
    }

    return removeIndexes;
}