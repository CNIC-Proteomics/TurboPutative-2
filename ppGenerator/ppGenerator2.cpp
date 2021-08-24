//
// Include modules
//
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <regex>

#include "../TPProcesser/REname/lib/RegexObject.hpp"
#include "../TPProcesser/REname/lib/SortPeptide.hpp"
// #include "../TPProcesser/REname/lib/SynonymsReader.hpp"

// Use Loguru (https://github.com/emilk/loguru) for logging
#include "../TPProcesser/lib/logging/loguru.hpp"
#include "../TPProcesser/lib/logging/loguru.cpp"

// Constants
#define INFILE_ORIGINAL "compound_original.txt"
#define INFILE_NAME "parsed_compound.txt"
#define INFILE_IDX "compound_index.txt"
#define OUTFILE_NAME "pre_processed_compound.txt"
//
// Main
//
int main(int argc, char *argv[])
{
    std::string workDirPath = argv[1];

    // Read original compounds processed by dbGenerator1
    std::cout << "** Reading " << INFILE_ORIGINAL << std::endl;
    std::vector<std::string> compoundNames = {};
    std::ifstream originalFile(workDirPath + "/" + INFILE_ORIGINAL);

    std::string line;
    while (std::getline(originalFile, line))
    {
        compoundNames.push_back(line);
    }

    // Read parsedCompounds and their index
    std::vector<std::string> parsedCompounds={};
    std::vector<int> goslinLipidsIdx={}; 

    // READ COMPOUND NAMES
    std::cout << "** Reading " << INFILE_NAME << std::endl;
    std::ifstream parsedCompoundFile(workDirPath + "/" + INFILE_NAME);
    line = "";
    while(std::getline(parsedCompoundFile, line))
    {
        parsedCompounds.push_back(line);
    }

    // READ INDEX
    std::cout << "** Reading " << INFILE_IDX << std::endl;
    std::ifstream compoundIdxFile(workDirPath + "/" + INFILE_IDX);
    line = "";
    while(std::getline(compoundIdxFile, line))
    {
        int idx = std::stoi(line);
        goslinLipidsIdx.push_back(idx);
    }

    // REPLACE
    std::cout << "** Introduce lipids processed by Goslin" << std::endl;
    for (int i=0; i<goslinLipidsIdx.size(); i++)
    {
        int idx = goslinLipidsIdx[i];
        compoundNames[idx] = parsedCompounds[i];
    }

    //
    // READ INDEX OF COMPOUNDS THAT WERE PREPROCESSED
    //
    std::vector<int> mappedIndex;
    std::ifstream mappedIndexFile (workDirPath + "/mappedIndex.txt");

    line = "";
    while(std::getline(mappedIndexFile, line))
    {
        mappedIndex.push_back(std::stoi(line));
    }
    
    // APPLY REGULAR EXPRESSIONS
    std::cout << "** Applying regular expressions" << std::endl;
    RegexObject REObject;
    REObject.readRegexINI();
    REObject.applyRegex(compoundNames, mappedIndex);

    
    // SORT PEPTIDES
    std::cout << "** Sorting peptides" << std::endl;
    SortPeptide sortPeptideObject("\\s");
    sortPeptideObject.sortAA(compoundNames, mappedIndex);


    // Write parsed compounds
    std::cout << "** Writing " << OUTFILE_NAME << std::endl;
    std::ofstream outFile (workDirPath + "/" + OUTFILE_NAME);

    for (std::string& name : compoundNames)
    {
        outFile << name << std::endl;
    }

    return 0;
}