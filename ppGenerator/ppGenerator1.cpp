//
// Includes
//
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

// #include "../TPProcesser/REname/lib/SynonymsReader.hpp"
#include "../TPProcesser/REname/lib/LipidList_and_PreProcess.hpp"

// Use Loguru (https://github.com/emilk/loguru) for logging
#include "../TPProcesser/lib/logging/loguru.hpp"
#include "../TPProcesser/lib/logging/loguru.cpp"

//
// Constants
//
#define OUTFILE_NAME "compound.txt"
#define OUTFILE_INDEX "compound_index.txt"
#define OUTFILE_ORIGINAL "compound_original.txt"

//
// Main
//
int main(int argc, char *argv[])
{
    std::string workDirPath = argv[1];
    std::string inFilePath = workDirPath + "/" + argv[2];

    // Read infile and name of the compounds
    std::cout << "** Reading list with compound name" << std::endl;
    std::vector<std::string> compoundNames = {};

    std::ifstream myFile(inFilePath);

    std::string line;
    while (std::getline(myFile, line))
    {
        compoundNames.push_back(line);
    }

    //
    // GET COMPOUNDS THAT CAN BE PROCESSED BY GOSLIN
    //

    // Boolean vector with true in lipids processable by goslin
    std::cout << "** Finding compounds processable by Goslin" << std::endl;
    LipidList lipidListObject;

    std::vector <int> mappedCompounds = {}; // We need this variable for some functions.
    std::vector<bool> isGoslinLipidVector = lipidListObject.findLipids(compoundNames, mappedCompounds);

    // Get lipids with true (and their index)
    std::vector<std::string> goslinLipids;
    std::vector<int> goslinLipidsIdx;
    for (int i=0; i<compoundNames.size(); i++)
    {
        if (isGoslinLipidVector[i])
        {
            goslinLipids.push_back(compoundNames[i]);
            goslinLipidsIdx.push_back(i);
        }
    }

    // Preprocess lipids processable by Goslin
    std::cout << "** Parse compound to avoid Goslin error" << std::endl;
    PreProcessLipids preProcessLipids;
    preProcessLipids.preProcessLipids(goslinLipids);

    // Write compound.txt
    std::cout << "** Writing " << OUTFILE_NAME << std::endl;
    std::string outFilePath = workDirPath + "/" + OUTFILE_NAME;
    
    std::ofstream outFile(outFilePath);

    for (std::string& name : goslinLipids)
    {
        outFile << name << std::endl;
    }

    // Write compound_index.txt
    std::cout << "** Writing " << OUTFILE_INDEX << std::endl;
    std::string outFileIndexPath = workDirPath + "/" + OUTFILE_INDEX;
    std::ofstream outFileIndex(outFileIndexPath);

    for (int& idx : goslinLipidsIdx)
    {
        outFileIndex << idx << std::endl;
    }

    // Write all list of compounds
    std::cout << "** Writing " << OUTFILE_ORIGINAL << std::endl;
    std::string outFileOriginalPath = workDirPath + "/" + OUTFILE_ORIGINAL;
    std::ofstream outFileOriginal(outFileOriginalPath);

    for (std::string& name : compoundNames)
    {
        outFileOriginal << name << std::endl;
    }

    return 0;
}   