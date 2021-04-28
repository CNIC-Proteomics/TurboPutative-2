#include <fstream>
#include <sstream>
// #include <filesystem>
#include <vector>
#include <string>
#include <algorithm>

#include "MapCompound.hpp"
#include "../../lib/logging/loguru.hpp"

// Constants
#define MAP_RESERVE 50000
#define INDEX_RESERVE 500
#define OUTFILE_NAME "mappedIndex.txt"

void MapCompound::readMapFile (std::string path)
{
    // logging
    std::stringstream log;
    log << "Reading Map File";
    LOG_F(INFO, &(log.str()[0]));

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

void MapCompound::readIndex (std::string path)
{
    // logging
    std::stringstream log;
    log << "Reading Map File Index";
    LOG_F(INFO, &(log.str()[0]));

    indexKey.reserve(INDEX_RESERVE);
    indexValue.reserve(INDEX_RESERVE);

    std::ifstream myFile (path);

    std::string line="";
    while (std::getline(myFile, line))
    {
        std::vector <std::string> tokens = parseBiTSV(line);
        indexKey.push_back(tokens[0]);
        
        int indexValue_tmp = std::stoi(tokens[1]);
        indexValue.push_back(indexValue_tmp);
    }

    // Add to indexValue a last element with size of the vector. This makes easier the search
    indexValue.push_back(original.size());
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
    log << "Mapping compounds";
    LOG_F(INFO, &(log.str()[0]));

    positionResult = {};
    positionResult.reserve(compoundList.size());

    for (int i=0; i<compoundList.size(); i++)
    {
        std::string resultName = compoundList[i];
        int sortedPos = getSortedPos (compoundList[i]);

        if (sortedPos != 0) 
        {
            int startIndex = indexValue[sortedPos-1], endIndex = indexValue[sortedPos];
            int posInMap = getPosInMap(compoundList[i], startIndex, endIndex);
            if (posInMap != -1) 
            {
                resultName = preProcessed[posInMap];
                positionResult.push_back(i);
            }
        }

        compoundList[i] = resultName;
    }

    log.str("");
    log << "Compounds were mapped";
    LOG_F(INFO, &(log.str()[0]));
}

int MapCompound::getSortedPos (std::string compound)
{
    std::string compoundLower = compound;
    std::transform(compound.begin(), compound.end(), compoundLower.begin(), [](unsigned char c){return std::tolower(c);});

    // Initialize vector with all names
    std::vector <std::string> compoundNames = indexKey;
    compoundNames.push_back(compoundLower);

    // Sort and find position
    std::sort(compoundNames.begin(), compoundNames.end());
    std::vector <std::string>::iterator it = std::find(compoundNames.begin(), compoundNames.end(), compoundLower);
    int sortedPos = it - compoundNames.begin();

    return sortedPos;
}

int MapCompound::getPosInMap (std::string compound, int startIndex, int endIndex)
{
    std::string compoundLower = compound;
    std::transform(compound.begin(), compound.end(), compoundLower.begin(), [](unsigned char c){ return std::tolower(c); });

    std::vector <std::string>::iterator it = std::find(original.begin()+startIndex, original.begin()+endIndex, compoundLower);
    int posInMap = it == original.begin()+endIndex ? -1 : it - original.begin();

    return posInMap;
}

void MapCompound::writeIndexResult (std::string path)
{
    // logging
    std::stringstream log;
    log << "Write index of mapped compounds";
    LOG_F(INFO, &(log.str()[0]));

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