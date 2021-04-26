#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <math.h>
#include <map>
#include <filesystem>

#include "../../lib/logging/loguru.hpp"

#include "TagList.hpp"
#include "../../lib/Tokenizer.hpp"

#define DELIM "\t"

TagList::TagList(std::filesystem::path relativePath)
{
    // std::filesystem::path relativePath = strRelativePath;

    // read file in relative path
    readList(relativePath);
}

void TagList::readList(std::filesystem::path relativePath)
{
    // logging
    std::stringstream log;
    log << "Reading Tag file: " << relativePath;
    LOG_F(INFO, &(log.str()[0]));

    std::ifstream listFile(relativePath);

    // object to split lines
    //Tokenizer tokenizer(DELIM);

    // start by getting column names
    std::string line;
    std::getline(listFile, line);
    //columns = tokenizer.tokenize(line);
    
    // read all compounds
    list.reserve(1000000); // reserve space to avoid memory reallocation
    //identifier.reserve(1000000);
    while(std::getline(listFile, line))
    {
        /*
        std::vector<std::string> tokens = tokenizer.tokenize(line);
        list.push_back(tokens[0]);
        identifier.push_back(tokens[1]);
        */
       list.push_back(line);
    }

    // logging
    log.str("");
    log << "Tag file was read";
    LOG_F(INFO, &(log.str()[0]));
}


void TagList::buildIndex()
{
    // std::vector<std::string> p = {"CCCC", "AAAAA", "BBBBB"};
    // Transform compounds in tag list to lower case
    for (std::string& listItem : list)
    {
        std::transform(listItem.begin(), listItem.end(), listItem.begin(), [](unsigned char c) {
            return std::tolower(c);});
    }

    // Sort list compounds alphabetically
    std::sort(list.begin(), list.end());

    // Get number of elements in index
    int indexN = std::sqrt(list.size());

    // Get number of batch per index
    int indexBatchN = list.size()/indexN;

    // Get compounds from list used as index and their posisition
    for (int i=0; i<list.size(); i=i+indexBatchN)
    {
        index.push_back(list[i]);
        indexMap.push_back(i);
    }

    // logging
    std::stringstream log;
    log << "Index was built";
    LOG_F(INFO, &(log.str()[0]));

}

std::vector<std::string> TagList::addTag(std::vector<std::string>& compoundNamesColumn, std::string tag)
{
    // Create vector where tags will be added
    std::vector<std::string> tagColumn;
    tagColumn.reserve(compoundNamesColumn.size());

    // Loop over each compound
    for (std::string& compound : compoundNamesColumn)
    {
        int compoundIndex = getSortedPos(compound);

        
        bool isCompound;
        if (compoundIndex == index.size()) // if compound is in the last position
        {
            isCompound = checkPresence(compound, indexMap[compoundIndex-1], list.size());
        } else if (compoundIndex == 0) // if compoundIndex is 0, it is not in tag list --> False
        {
            isCompound = false;
        } else
        {
            isCompound = checkPresence(compound, indexMap[compoundIndex-1], indexMap[compoundIndex]);
        }

        isCompound ? tagColumn.push_back(tag) : tagColumn.push_back("");
        
    }

    std::stringstream log;
    log << "Tag was added";
    LOG_F(INFO, &(log.str()[0]));

    return tagColumn;
}

// ADD TAG WITHOUT INDEX (CHECK IF IT IS FASTER)
std::vector<std::string> TagList::addTagNoIndex(std::vector<std::string>& compoundNamesColumn, std::string tag)
{
    std::vector<std::string> tagColumn;
    tagColumn.reserve(compoundNamesColumn.size());

    for (std::string& compound : compoundNamesColumn)
    {
        std::string compoundLower = compound;
        std::transform(compound.begin(), compound.end(), compoundLower.begin(), [](unsigned char c) { return std::tolower(c); });
        std::vector<std::string>::iterator it = std::find(list.begin(), list.end(), compoundLower);
        it == list.end() ? tagColumn.push_back("") : tagColumn.push_back(tag);
    }

    return tagColumn;
}

int TagList::getSortedPos(std::string compound)
{
    // work with compound to lower
    std::string compoundLower = compound;
    std::transform(compound.begin(), compound.end(), compoundLower.begin(), [](unsigned char c) { 
        return std::tolower(c); 
        });

    // Get vector with indexes and the compound name
    std::vector<std::string> indexAndName = index;
    indexAndName.push_back(compoundLower);

    // Sort the name of the compounds in the vector
    std::sort(indexAndName.begin(), indexAndName.end());

    // Get iterator to position of compound
    std::vector<std::string>::iterator it = std::find(indexAndName.begin(), indexAndName.end(), compoundLower);

    // Get index of the compound after sorting
    int compoundIndex = it - indexAndName.begin();

    return compoundIndex;
}

bool TagList::checkPresence(std::string compound, int init, int last)
{
    std::string compoundLower = compound;
    std::transform(compound.begin(), compound.end(), compoundLower.begin(), [](unsigned char c) {
        return std::tolower(c);});

    // find compound in that position and return true or false
    std::vector<std::string>::iterator it = std::find(list.begin()+init, list.begin()+last, compoundLower);

    if (it == list.begin()+last)
    {
        return false;
    } else
    {
        return true;
    }
    
}