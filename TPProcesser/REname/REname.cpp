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

// 

// DECLARE FUNCTIONS AND CLASSES

// MAIN FUNCTION
int main(int argc, char *argv[])
{
    // get path to working directory
    std::filesystem::path workDirPath = argv[1];

    // configure logging using loguru --> https://github.com/emilk/loguru
    std::string logFilePath = (workDirPath / "REname.log").string();    // Add file and convert to string
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
    // FIND IF THEY ARE GOSLIN LIPID
    //
    LipidList lipidListObject;
    std::vector<bool> isGoslinLipidVector = lipidListObject.findLipids(compoundNamesColumn);


    //
    // PRE-PROCESS TO BE ABLE TO APPLY GOSLIN
    //
    std::vector<std::string> goslinLipids;
    std::vector<int> goslinLipidsIdx;
    for (int i=0; i<compoundNamesColumn.size(); i++)
    {
        // take lipids that are going to be processed by Goslin and their index
        if (isGoslinLipidVector[i])
        {
            goslinLipids.push_back(compoundNamesColumn[i]);
            goslinLipidsIdx.push_back(i);
        }
    }

    // pre-process lipids that will be processed by Goslin
    PreProcessLipids preProcessLipids;
    preProcessLipids.preProcessLipids(goslinLipids);


    //
    // SAVE NAME OF COMPOUNDS TO APPLY TPGOSLIN
    //
    // logging
    std::stringstream log;
    log << "Applying Goslin";
    LOG_F(INFO, &(log.str()[0]));

    std::ofstream compoundFile(workDirPath / "compound.txt");
    for (std::string& compoundName : goslinLipids)
    {
        compoundFile << compoundName << std::endl;
    }

    log.str("");
    log << "Goslin was applied";
    LOG_F(INFO, &(log.str()[0]));

    //
    // WRITE TABLE
    //
    std::filesystem::path outfilePath = workDirPath;
    outfilePath /= config.getValue("outfile");
    msTable.writeTable(outfilePath);
}