#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <math.h>
#include <map>
// #include <filesystem>

//#include "../../lib/logging/loguru.hpp"

#include "TagList.hpp"
#include "../../lib/Tokenizer.hpp"

#define DELIM "\t"

TagList::TagList(std::string relativePath)
{
    // std::filesystem::path relativePath = strRelativePath;

    // read file in relative path
    readList(relativePath);
}

void TagList::readList(std::string relativePath)
{
    // logging
    std::stringstream log;
    log << "\n** " <<  __DATE__ << " | " << __TIME__ << " | " << __FILE__ << "[" << __func__ << "]" << ":" << __LINE__ << " | ";
    log << "Reading Tag file: " << relativePath;
    std::cout << log.str();
    //LOG_F(INFO, &(log.str()[0]));

    std::ifstream listFile(relativePath);


    // start by getting column names
    std::string line;
    std::getline(listFile, line);
    
    // read all compounds
    list.reserve(1000000); // reserve space to avoid memory reallocation

    while(std::getline(listFile, line))
    {
        std::string lineLower = line;
        std::transform(line.begin(), line.end(), lineLower.begin(), [](unsigned char c) {
            return std::tolower(c); 
        });
        
        list.push_back(lineLower);
    }

    std::sort(list.begin(), list.end());

    // logging
    log.str("");
    log << "\n** " <<  __DATE__ << " | " << __TIME__ << " | " << __FILE__ << "[" << __func__ << "]" << ":" << __LINE__ << " | ";
    log << "Tag file was read";
    std::cout << log.str();
    //LOG_F(INFO, &(log.str()[0]));
}


std::vector<std::string> TagList::addTag(std::vector<std::string>& compoundNamesColumn, std::string tag)
{
    // Create vector where tags will be added
    std::vector<std::string> tagColumn;
    tagColumn.reserve(compoundNamesColumn.size());

    // Loop over each compound
    for (std::string& compound : compoundNamesColumn)
    {
        std::string compoundLower = compound;
        std::transform(compound.begin(), compound.end(), compoundLower.begin(), [](unsigned char c) {
            return std::tolower(c); 
        });

        bool isCompound = binarySearch(0, list.size()-1, compoundLower);

        isCompound ? tagColumn.push_back(tag) : tagColumn.push_back("");
    }

    std::stringstream log;
    log << "\n** " <<  __DATE__ << " | " << __TIME__ << " | " << __FILE__ << "[" << __func__ << "]" << ":" << __LINE__ << " | ";
    log << "Tag was added";
    std::cout << log.str();
    //LOG_F(INFO, &(log.str()[0]));

    return tagColumn;
}


bool TagList::binarySearch(int l, int r, std::string& x)
{
    if (r >= l) {
        int mid = l + (r - l) / 2;
  
        // If the element is present at the middle
        // itself
        if (list[mid] == x)
            return true;
  
        // If element is smaller than mid, then
        // it can only be present in left subarray
        if (list[mid] > x)
            return binarySearch(l, mid - 1, x);
  
        // Else the element can only be present
        // in right subarray
        return binarySearch(mid + 1, r, x);
    }
  
    // We reach here when element is not
    // present in array
    return false;
}
