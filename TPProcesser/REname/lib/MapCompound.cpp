#include <iostream>
#include <fstream>
#include <sstream>
// #include <filesystem>
#include <vector>
#include <string>
#include <algorithm>

#include "MapCompound.hpp"
// #include "../../lib/logging/loguru.hpp"

// Constants
#define MAP_RESERVE 50000
#define INDEX_RESERVE 500
#define OUTFILE_NAME "mappedIndex.txt"

void MapCompound::readMapFile (std::string path)
{
    // logging
    std::stringstream log;
    log << "\n** " <<  __DATE__ << " | " << __TIME__ << " | " << __FILE__ << "[" << __func__ << "]" << ":" << __LINE__ << " | ";
    log << "Reading Map File";
    std::cout << log.str();
    // LOG_F(INFO, &(log.str()[0]));

    original.reserve(MAP_RESERVE);
    preProcessed.reserve(MAP_RESERVE);

    std::ifstream myFile (path);

    std::string line="";
    while (std::getline(myFile, line))
    {
        std::vector <std::string> tokens = parseBiTSV(line);
        original.push_back(tokens[0]);
        preProcessed.push_back(tokens[1]);
    }
}


std::vector<std::string> MapCompound::parseBiTSV (std::string line)
{
    std::vector <std::string> tokens={};
    int sepPos = line.find("\t");
    
    tokens.push_back(line.substr(0, sepPos));
    tokens.push_back(line.substr(sepPos+1));

    return tokens;
}

void MapCompound::mapCompounds (std::vector <std::string>& compoundList)
{
    // logging
    std::stringstream log;
    log << "\n** " <<  __DATE__ << " | " << __TIME__ << " | " << __FILE__ << "[" << __func__ << "]" << ":" << __LINE__ << " | ";
    log << "Mapping compounds";
    std::cout << log.str();
    // LOG_F(INFO, &(log.str()[0]));

    positionResult = {};
    positionResult.reserve(compoundList.size());

    for (int i=0; i<compoundList.size(); i++)
    {
        std::string compoundLower = compoundList[i];
        std::transform(compoundList[i].begin(), compoundList[i].end(), compoundLower.begin(), [](unsigned char c) {
            return std::tolower(c);
        });

        std::string binaryResult = binarySearch(0, original.size()-1, compoundLower);

        if (binaryResult != compoundLower)
        {
            compoundList[i] = binaryResult;
            positionResult.push_back(i);
        }
    
    }
    

    log.str("");
    log << "\n** " <<  __DATE__ << " | " << __TIME__ << " | " << __FILE__ << "[" << __func__ << "]" << ":" << __LINE__ << " | ";
    log << "Compounds were mapped";
    std::cout << log.str();
    //LOG_F(INFO, &(log.str()[0]));
}


std::string MapCompound::binarySearch(int l, int r, std::string& x) 
{
    if (r >= l) {
        int mid = l + (r - l) / 2;
  
        // If the element is present at the middle
        // itself
        if (original[mid] == x)
            return preProcessed[mid];
  
        // If element is smaller than mid, then
        // it can only be present in left subarray
        if (original[mid] > x)
            return binarySearch(l, mid - 1, x);
  
        // Else the element can only be present
        // in right subarray
        return binarySearch(mid + 1, r, x);
    }
  
    // We reach here when element is not
    // present in array
    return x;
}


void MapCompound::writeIndexResult (std::string path)
{
    // logging
    std::stringstream log;
    log << "\n** " <<  __DATE__ << " | " << __TIME__ << " | " << __FILE__ << "[" << __func__ << "]" << ":" << __LINE__ << " | ";
    log << "Write index of mapped compounds";
    std::cout << log.str();
    //LOG_F(INFO, &(log.str()[0]));

    std::ofstream myFile (path + "/" + OUTFILE_NAME);

    for (int& i : positionResult)
    {
        myFile << i << std::endl;
    }
}

std::vector <int>& MapCompound::getPositionResult ()
{
    return positionResult;
}