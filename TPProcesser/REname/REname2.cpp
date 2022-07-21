#include <iostream>
#include <sstream>
// #include <filesystem>
#include <string>
#include <vector>
#include <regex>
#include <fstream>

// Use Loguru (https://github.com/emilk/loguru) for logging
//#include "../lib/logging/loguru.hpp"
//#include "../lib/logging/loguru.cpp"

// IMPORT TurboPutative LIBRARIES
#include "../lib/ConfigReader.hpp"
#include "../lib/GenericTableHandler.hpp"
#include "./lib/LipidMapsAbb.hpp"
#include "./lib/RegexObject.hpp"
#include "./lib/SortPeptide.hpp"

// DEFINES
#define MODULE "REname"
#define RENAME1_OUT_TABLE "Pre_REname_Table.tsv"

#define REGEX_FA_PN_INI_PATH "./src/TurboPutative-2.0-built/TPProcesser/REname/data/regexFAPN.ini"
#define REGEX_FA_INI_PATH "./src/TurboPutative-2.0-built/TPProcesser/REname/data/regexFA.ini"
#define REGEX_BA_INI_PATH "./src/TurboPutative-2.0-built/TPProcesser/REname/data/regexBA.ini"
#define REGEX_INI_PATH "./src/TurboPutative-2.0-built/TPProcesser/REname/data/regex.ini"

#define LMSD_TABLE_PATH "./src/TurboPutative-2.0-built/TPProcesser/REname/data/LMSD_abbreviation.tsv"

// separator between values of fused rows
#define SEPARATOR " ; "

// 

// DECLARE FUNCTIONS AND CLASSES
// void TPGoslinCreateProcess( std::string workDirPath ); // defined in "lib/TPGoslinCreateProcess.cpp"

// MAIN FUNCTION
int main(int argc, char *argv[])
{
    // get path to working directory
    std::string workDirPath = argv[1];

    // configure logging using loguru --> https://github.com/emilk/loguru
    /*std::string logFilePath = workDirPath + "/REname2.log";    // Add file and convert to string
    char* logFilePathPtr = &logFilePath[0]; // Get pointer to first character (received by loguru)

    loguru::init(argc, argv);
    loguru::add_file(logFilePathPtr, loguru::Append, loguru::Verbosity_MAX);*/
    
    // read config file
    ConfigReader config(workDirPath);
    config.readConfigFile(MODULE);

    // read input table
    GenericTableHandler msTable;
    msTable.readTable(workDirPath, RENAME1_OUT_TABLE);

    // get column names
    std::vector<std::string> columnNames = msTable.getColumnNames();

    // get the name of the column containing compound names and its index
    std::string compoundColumnName = config.getValue("column_name");
    std::vector<std::string>& compoundNamesColumn = msTable.getColumn(compoundColumnName);
    std::vector<std::string> compoundNamesOriginal = compoundNamesColumn;

    //
    // READ PARSED COMPOUND NAMES AND THEIR INDEX. THEN, REPLACE COMPOUNDS IN THE TABLE (USING THAT INDEX)
    //
    std::vector<std::string> parsedCompounds={};
    std::vector<int> goslinLipidsIdx={};

    // READ COMPOUND NAMES
    std::ifstream parsedCompoundFile(workDirPath+"/parsed_compound.txt");
    std::string line;
    while(std::getline(parsedCompoundFile, line))
    {
        parsedCompounds.push_back(line);
    }

    // READ INDEX
    std::ifstream compoundIdxFile(workDirPath+"/compound_index.txt");
    line = "";
    while(std::getline(compoundIdxFile, line))
    {
        int idx = std::stoi(line);
        goslinLipidsIdx.push_back(idx);
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
    // READ INDEX OF COMPOUNDS THAT WERE PREPROCESSED
    //
    std::vector<int> mappedIndex;
    std::ifstream mappedIndexFile (workDirPath + "/mappedIndex.txt");

    line = "";
    while(std::getline(mappedIndexFile, line))
    {
        mappedIndex.push_back(std::stoi(line));
    }

    //
    // APPLY REGULAR EXPRESSIONS
    //

    // Regular expressions used to translate fatty acids
    // appyRegex must receive a mappedIndex, but in this case we do not want to 
    // save processed compounds. They have to processed in the next regex-
    std::vector <int> mappedIndexUnsaved = {}; 
    RegexObject RE_FA_PN_Object;
    RE_FA_PN_Object.readRegexINI(REGEX_FA_PN_INI_PATH);
    RE_FA_PN_Object.applyRegex(compoundNamesColumn, mappedIndexUnsaved);

    // Fatty acid oriented set of regular expressions
    RegexObject RE_FA_Object;
    RE_FA_Object.readRegexINI(REGEX_FA_INI_PATH);
    RE_FA_Object.applyRegex(compoundNamesColumn, mappedIndex);

    // Fatty acid oriented set of regular expressions
    RegexObject RE_BA_Object;
    RE_BA_Object.readRegexINI(REGEX_BA_INI_PATH);
    RE_BA_Object.applyRegex(compoundNamesColumn, mappedIndex);

    // Search in LipidMaps
    LipidMapsAbb lipidMapsAbb;
    lipidMapsAbb.readLMSDTable(LMSD_TABLE_PATH);
    lipidMapsAbb.findCompounds(compoundNamesColumn, compoundNamesOriginal, mappedIndex);

    // 'Classical' set of regular expressions
    RegexObject REObject;
    REObject.readRegexINI(REGEX_INI_PATH);
    REObject.applyRegex(compoundNamesColumn, mappedIndex);

    //
    // SORT PEPTIDES
    //
    SortPeptide sortPeptideObject(config.getValue("aminoacid_separator"));
    sortPeptideObject.sortAA(compoundNamesColumn, mappedIndex);
    

    //
    // FUSE ROWS WITH SAME NAME AND EXPERIMENTAL MASS PRESERVING THE REST
    //
    std::string experimentalMassCol = config.getValue("column_mass");
    std::vector<std::string> compareCols = {experimentalMassCol}, conserveCols;
    
    for (std::string& col : msTable.getColumnNames())
    {
        if (col != compoundColumnName && col != experimentalMassCol) conserveCols.push_back(col);
    }

    msTable.fuseRows(compareCols, conserveCols, compoundColumnName, SEPARATOR);


    //
    // FUSE ROWS WITH SAME INCHI-KEY (14CHARS) AND EXPERIMENTAL MASS PRESERVING THE REST BUT THE NAME
    //

    /* DEACTIVATE INCHIKEY FUSION

    // if inChiKey column is present, apply this fusion
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

    */


    //
    // WRITE TABLE
    //
    std::string outfilePath = workDirPath;
    outfilePath += "/" + config.getValue("outfile");
    msTable.writeTable(outfilePath);
}