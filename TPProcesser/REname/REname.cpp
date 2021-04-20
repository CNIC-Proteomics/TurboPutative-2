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
#include "./lib/RegexObject.hpp"
#include "./lib/SortPeptide.hpp"

// DEFINES
#define MODULE "REname"

// 

// DECLARE FUNCTIONS AND CLASSES
void TPGoslinCreateProcess( std::string workDirPath ); // defined in "lib/TPGoslinCreateProcess.cpp"

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


    //              //
    // START GOSLIN // 
    //              //

    //
    // FIND GOSLIN LIPID
    //
    LipidList lipidListObject;
    std::vector<bool> isGoslinLipidVector = lipidListObject.findLipids(compoundNamesColumn);

    //
    // EXTRACT AND PRE-PROCESS TO BE ABLE TO APPLY GOSLIN
    //
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


    //
    // SAVE NAME OF COMPOUNDS TO APPLY TPGOSLIN
    //
    std::ofstream compoundFile(workDirPath / "compound.txt");
    for (std::string& compoundName : goslinLipids)
    {
        compoundFile << compoundName << std::endl;
    }    

    //
    // APPLY GOSLIN
    //
    // logging
    std::stringstream log;
    log << "Applying Goslin";
    LOG_F(INFO, &(log.str()[0]));

    TPGoslinCreateProcess( workDirPath.string() );

    log.str("");
    log << "Goslin was applied";
    LOG_F(INFO, &(log.str()[0]));


    //
    // READ PARSED COMPOUND NAMES AND REPLACE IN TABLE
    //
    std::vector<std::string> parsedCompounds;

    // READ
    std::ifstream parsedCompoundFile(workDirPath/"parsed_compound.txt");
    std::string line;
    while(std::getline(parsedCompoundFile, line))
    {
        parsedCompounds.push_back(line);
    }

    // REPLACE
    for (int i=0; i<goslinLipidsIdx.size(); i++)
    {
        int idx = goslinLipidsIdx[i];
        compoundNamesColumn[idx] = parsedCompounds[i];
    }

    //            //
    // END GOSLIN //
    //            //


    //
    // APPLY REGULAR EXPRESSIONS
    //
    RegexObject REObject;
    REObject.readRegexINI();
    REObject.applyRegex(compoundNamesColumn);

    //
    // SORT PEPTIDES
    //
    SortPeptide sortPeptideObject(config.getValue("aminoacid_separator"));
    sortPeptideObject.sortAA(compoundNamesColumn);

    //
    // APPLY SYNONYMS
    //
    synonymsReader.replace(compoundNamesColumn);

    //
    // FUSE ROWS WITH SAME NAME AND EXPERIMENTAL MASS PRESERVING THE REST
    //
    std::string experimentalMassCol = config.getValue("column_mass");
    std::vector<std::string> compareCols = {experimentalMassCol}, conserveCols;
    
    for (std::string& col : msTable.getColumnNames())
    {
        if (col != compoundColumnName && col != experimentalMassCol) conserveCols.push_back(col);
    }

    msTable.fuseRows(compareCols, conserveCols, compoundColumnName);


    //
    // FUSE ROWS WITH SAME INCHI-KEY (14CHARS) AND EXPERIMENTAL MASS PRESERVING THE REST BUT THE NAME
    //

    // if inChiKey column is present, appy this fusion
    std::string inChiKeyColName = config.getValue("column_inchi_key");

    if (inChiKeyColName != "None")
    {
        // --> Create new column with first 14 inchikey characters
        
        std::string inChiKey_14_ColName = "InChiKey_14";

        std::vector<std::string>& inChiKeyCol = msTable.getColumn(inChiKeyColName);
        std::vector<std::string> inChiKey_14;
        inChiKey_14.reserve(msTable.getNrows());

        for (std::string& inchi : inChiKeyCol)
        {
            inchi.length() > 14 ? inChiKey_14.push_back(inchi.substr(0, 14)) : inChiKey_14.push_back("N/A");
        }

        msTable.addColumn(inChiKey_14, msTable.getColumnNames().size(), inChiKey_14_ColName);

        
        compareCols = {experimentalMassCol, inChiKey_14_ColName};
        conserveCols = {};

        for (std::string& col : msTable.getColumnNames())
        {
            if (col != compoundColumnName && col != experimentalMassCol && col != inChiKey_14_ColName) conserveCols.push_back(col);
        }
        msTable.fuseRows(compareCols, conserveCols);

        msTable.removeColumn(inChiKey_14_ColName);
    }


    //
    // WRITE TABLE
    //
    std::filesystem::path outfilePath = workDirPath;
    outfilePath /= config.getValue("outfile");
    msTable.writeTable(outfilePath);
}