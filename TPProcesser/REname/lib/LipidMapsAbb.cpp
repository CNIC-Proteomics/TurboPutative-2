#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "LipidMapsAbb.hpp"

#define SEP "\t"


void LipidMapsAbb::readLMSDTable(std::string filePath)
{
    std::stringstream log;
    log << "\n** " <<  __DATE__ << " | " << __TIME__ << " | " << __FILE__ << "[" << __func__ << "]" << ":" << __LINE__ << " | ";
    log << "Reading LMSD table: " << filePath;
    std::cout << log.str();

    std::ifstream myFile(filePath);

    std::string line = "";
    std::vector <std::pair <std::string, std::string>> pairVector;

    while (std::getline(myFile, line))
    {
        // Extract original and abbreviation
        int pos = line.find(SEP);
        std::string original = line.substr(0, pos);
        std::string abb = line.substr(pos+1);

        // Convert original to lower case
        std::for_each(original.begin(), original.end(), [] (char& c) {c = std::tolower(c);});

        // Create pair
        pairVector.push_back(std::make_pair(original, abb));
    }

    myFile.close();

    // Sort original array
    std::sort(pairVector.begin(), pairVector.end());

    // Extract sorted lipids and abbreviations
    for (std::pair<std::string, std::string> pairElem: pairVector)
    {
        originalNameArr.push_back(pairElem.first);
        abbArr.push_back(pairElem.second);
    }

}

void LipidMapsAbb::findCompounds(
    std::vector<std::string>& compoundNamesColumn, 
    std::vector<std::string> compoundNamesOriginal, 
    std::vector<int>& mappedIndex
)
{
    std::stringstream log;
    log << "\n** " <<  __DATE__ << " | " << __TIME__ << " | " << __FILE__ << "[" << __func__ << "]" << ":" << __LINE__ << " | ";
    log << "Searching compounds in LMSD table";
    std::cout << log.str();

    // Loop over each original compound
    for (int i=0; i<compoundNamesOriginal.size(); i++)
    {
        std::for_each(compoundNamesOriginal[i].begin(), compoundNamesOriginal[i].end(), [] (char& c) {c = std::tolower(c);});

        int index = binarySearch(0, originalNameArr.size()-1, compoundNamesOriginal[i]);

        if (index != -1) // If compound is in LMSD...
        {
            if (std::find(mappedIndex.begin(), mappedIndex.end(), i) == mappedIndex.end())
            { // If compound is not in mappedIndex, overwrite using LMSD abbreviation
                compoundNamesColumn[i] = "LMSD{ " + abbArr[index] + " }";
                mappedIndex.push_back(i);
            } 
            else
            { // If compound is in mappedIndex, append LMSD abbreviation
                compoundNamesColumn[i] += "; LMSD{ " + abbArr[index] + " }";
            }
        }
    }
}


// Adapted from: https://www.geeksforgeeks.org/binary-search/
int LipidMapsAbb::binarySearch(int l, int r, std::string x)
{
    if (r >= l) {
        int mid = l + (r - l) / 2;
  
        // If the element is present at the middle
        // itself
        if (originalNameArr[mid] == x)
            return mid;
  
        // If element is smaller than mid, then
        // it can only be present in left subarray
        if (originalNameArr[mid] > x)
            return binarySearch(l, mid - 1, x);
  
        // Else the element can only be present
        // in right subarray
        return binarySearch(mid + 1, r, x);
    }
  
    // We reach here when element is not
    // present in array
    return -1;
}