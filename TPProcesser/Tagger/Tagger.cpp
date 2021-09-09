#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <regex>

// Use Loguru (https://github.com/emilk/loguru) for logging
//#include "../lib/logging/loguru.hpp"
//#include "../lib/logging/loguru.cpp"

// IMPORT TurboPutative LIBRARIES
#include "../lib/ConfigReader.hpp"
#include "../lib/GenericTableHandler.hpp"
#include "./lib/TagList.hpp"
#include "./lib/TagRegex.hpp"

// DEFINES
#define MODULE "Tagger"

// #define LIST_FOLDER "./TPProcesser/Tagger/data"
#define LIST_FOLDER "./src/TurboPutative-2.0-built/TPProcesser/Tagger/data"
#define FOOD_LIST_FILENAME "food_list.tsv"
#define DRUG_LIST_FILENAME "drug_list.tsv"
#define MICROBIAL_LIST_FILENAME "microbial_list.tsv"
#define NATURAL_PRODUCT_LIST_FILENAME "natural_product_list.tsv"
#define PLANT_LIST_FILENAME "plant_list.tsv"

// 

// DECLARE FUNCTIONS AND CLASSES
std::vector<std::string> taggerFromList(std::string listFileName, std::string tag, std::vector<std::string>& compoundNamesColumn);
std::vector<std::string> taggerFromRegex(std::string regexStr, std::string tag, std::vector<std::string>& compoundNamesColumn, bool icase=false);

// MAIN FUNCTION
int main(int argc, char *argv[])
{

    // get path to working directory
    std::string workDirPath = argv[1];

    // configure logging using loguru --> https://github.com/emilk/loguru
    /*std::string logFilePath = workDirPath + "/Tagger.log";    // Add file and convert to string
    char* logFilePathPtr = &logFilePath[0]; // Get pointer to first character (received by loguru)

    loguru::init(argc, argv);
    loguru::add_file(logFilePathPtr, loguru::Append, loguru::Verbosity_MAX);*/
    
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

    // get the column with the name of the compounds
    std::vector<std::string> compoundNamesColumn = msTable.getColumnCopy(compoundColumnName);

    // check tag by tag
    if (config.getValue("food") == "true")
    {
        // tag from list
        std::vector<std::string> tagColumnList;
        tagColumnList = taggerFromList(FOOD_LIST_FILENAME, "Food", compoundNamesColumn);

        // tag from regex
        std::vector<std::string> tagColumnRegex;
        tagColumnRegex = taggerFromRegex("^ent-", "Food", compoundNamesColumn, true);

        // combine both tags
        std::vector<std::string> tagColumnCombined;
        tagColumnCombined.reserve(compoundColumnName.size());
        for (int i=0; i<compoundNamesColumn.size(); i++)
        {
            tagColumnRegex[i]=="Food" || tagColumnList[i]=="Food" ? tagColumnCombined.push_back("Food") : tagColumnCombined.push_back("");
        }

        msTable.addColumn(tagColumnCombined, compoundColumnNameIdx+1, "Food");
    }

    if (config.getValue("drug") == "true")
    {
        std::vector<std::string> tagColumn;
        tagColumn = taggerFromList(DRUG_LIST_FILENAME, "Drug", compoundNamesColumn);
        msTable.addColumn(tagColumn, compoundColumnNameIdx+1, "Drug");
    }

    if (config.getValue("microbial_compound") == "true")
    {
        std::vector<std::string> tagColumn;
        tagColumn = taggerFromList(MICROBIAL_LIST_FILENAME, "MC", compoundNamesColumn);
        msTable.addColumn(tagColumn, compoundColumnNameIdx+1, "Microbial");
    }

    if (config.getValue("natural_product") == "true")
    {
        std::vector<std::string> tagColumn;
        tagColumn = taggerFromList(NATURAL_PRODUCT_LIST_FILENAME, "NP", compoundNamesColumn);
        msTable.addColumn(tagColumn, compoundColumnNameIdx+1, "NaturalProduct");
    }

    if (config.getValue("plant") == "true")
    {
        std::vector<std::string> tagColumn;
        tagColumn = taggerFromList(PLANT_LIST_FILENAME, "Plant", compoundNamesColumn);
        msTable.addColumn(tagColumn, compoundColumnNameIdx+1, "Plant");
    }

    if (config.getValue("halogenated") == "true")
    {
        std::vector<std::string> tagColumn;
        
        if (config.getValue("column_chemical_formula") != "None")
        {
            std::vector<std::string> chemicalFormulaColumn = msTable.getColumn(config.getValue("column_chemical_formula"));
            tagColumn = taggerFromRegex("(F|Cl|Br|I)(?![a-z])", "x", chemicalFormulaColumn, false);
        } else
        {
            tagColumn = taggerFromRegex(config.getValue("halogenated_regex"), "x", compoundNamesColumn, true);
        }
        
        msTable.addColumn(tagColumn, compoundColumnNameIdx+1, "Halogenated");
    }

    if (config.getValue("peptide") == "true")
    {
        std::vector<std::string> tagColumn;
        tagColumn = taggerFromRegex(config.getValue("peptide_regex"), "Pep", compoundNamesColumn, true);
        msTable.addColumn(tagColumn, compoundColumnNameIdx+1, "Peptide");
    }

    // write table
    std::string outfilePath = workDirPath;
    outfilePath += "/" + config.getValue("outfile");
    msTable.writeTable(outfilePath);

    return 0;
}

// DEFINE FUNCTIONS AND CLASSES
std::vector<std::string> taggerFromList(std::string listFileName, std::string tag, std::vector<std::string>& compoundNamesColumn)
{
    // logging
    std::stringstream log;
    log << "\n** " <<  __DATE__ << " | " << __TIME__ << " | " << __FILE__ << "[" << __func__ << "]" << ":" << __LINE__ << " | ";
    log << "** Add " << tag << " tagging from list";
    std::cout << log.str();
    //LOG_F(INFO, &(log.str()[0]));

    // read file with list of compounds
    std::string fullListPath = LIST_FOLDER;
    fullListPath += "/" + listFileName;
    TagList tagList(fullListPath);

    // add tags
    std::vector<std::string> tagColumn = tagList.addTag(compoundNamesColumn, tag);

    return tagColumn;
}

std::vector<std::string> taggerFromRegex(std::string regexStr, std::string tag, std::vector<std::string>& compoundNamesColumn, bool icase)
{
    // logging
    std::stringstream log ;
    log << "\n** " <<  __DATE__ << " | " << __TIME__ << " | " << __FILE__ << "[" << __func__ << "]" << ":" << __LINE__ << " | ";
    log << "Add " << tag << " tagging from regular expression";
    std::string logo = log.str();
    std::cout << logo;
    //LOG_F(INFO, &(log.str()[0]));

    // instantiate TagRegex class
    TagRegex tagRegex(regexStr, tag, icase);

    // get column with tag
    std::vector<std::string> tagColumn = tagRegex.addTag(compoundNamesColumn);

    return tagColumn;
}