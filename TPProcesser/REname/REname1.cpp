#include <iostream>
#include <sstream>
#include <filesystem>
#include <string>
#include <vector>
#include <regex>
#include <fstream>

// Use Loguru (https://github.com/emilk/loguru) for logging
#include "../lib/logging/loguru.hpp"
#include "../lib/logging/loguru.cpp"

// IMPORT TurboPutative LIBRARIES
#include "../lib/ConfigReader.hpp"
#include "../lib/GenericTableHandler.hpp"

#include "./lib/RemoveRow.hpp"
#include "./lib/SynonymsReader.hpp"
#include "./lib/LipidList_and_PreProcess.hpp"

// DEFINES
#define MODULE "REname"
#define RENAME1_OUT_TABLE "Pre_REname_Table.tsv"



// MAIN FUNCTION
int main(int argc, char *argv[])
{
    // get path to working directory
    std::filesystem::path workDirPath = argv[1];

    // configure logging using loguru --> https://github.com/emilk/loguru
    std::string logFilePath = (workDirPath / "REname1.log").string();    // Add file and convert to string
    char* logFilePathPtr = &logFilePath[0]; // Get pointer to first character (received by loguru)

    loguru::init(argc, argv);
    loguru::add_file(logFilePathPtr, loguru::Append, loguru::Verbosity_MAX);
    
    // read config file
    ConfigReader config(workDirPath);
    config.readConfigFile(MODULE);

    // read input table
    GenericTableHandler msTable;
    msTable.readTable(workDirPath, config.getValue("infile"));

    // get column names
    std::vector<std::string> columnNames = msTable.getColumnNames();

    // get the name of the column containing compound names and its index
    std::string compoundColumnName = config.getValue("column_name");
    int compoundColumnNameIdx = msTable.getIndexOfColumn(compoundColumnName);


    // 
    // REMOVE ROWS WITH REGULAR EXRESSION
    //
    RemoveRow removeRowObject(config.getValue("remove_row"));
    std::vector<int> removeIndex = removeRowObject.getIndexes(msTable.getColumn(config.getValue("column_name")));
    msTable.removeIndexes(removeIndex);


    //
    // SYNONYMS
    //
    // get the column with the name of the compounds
    std::vector<std::string>& compoundNamesColumn = msTable.getColumn(compoundColumnName);
    SynonymsReader synonymsReader;
    synonymsReader.replace(compoundNamesColumn);


    //
    // WRITE PRE-TABLE OF RENAME (GOSLIN WILL BE APPLIED)
    //
    std::filesystem::path outFilePath = workDirPath;
    outFilePath /= RENAME1_OUT_TABLE;
    msTable.writeTable(outFilePath);


    //              
    // PREPARE GOSLIN EXECUTION 
    //              

    // --> FIND GOSLIN LIPID
    LipidList lipidListObject;
    std::vector<bool> isGoslinLipidVector = lipidListObject.findLipids(compoundNamesColumn);

    
    // --> EXTRACT AND PRE-PROCESS TO BE ABLE TO APPLY GOSLIN
    std::vector<std::string> goslinLipids;
    std::vector<int> goslinLipidsIdx;
    for (int i=0; i<compoundNamesColumn.size(); i++)
    {
        // EXTRACT: take lipids that are going to be processed by Goslin and their index
        if (isGoslinLipidVector[i])
        {
            goslinLipids.push_back(compoundNamesColumn[i]);
            goslinLipidsIdx.push_back(i);
        }
    }

    // pre-process lipids that will be processed by Goslin
    PreProcessLipids preProcessLipids;
    preProcessLipids.preProcessLipids(goslinLipids);
    
    // --> SAVE NAME OF COMPOUNDS TO APPLY TPGOSLIN
    std::ofstream compoundFile(workDirPath / "compound.txt");
    for (std::string& compoundName : goslinLipids)
    {
        compoundFile << compoundName << std::endl;
    }

    // --> SAVE INDEX OF THESE COMPOUNDS
    std::ofstream indexFile(workDirPath / "compound_index.txt");
    for (int& idx : goslinLipidsIdx)
    {
        indexFile << idx << std::endl;
    }

    return 0;
}