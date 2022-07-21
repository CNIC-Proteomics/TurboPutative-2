#include <vector>
#include <string>
// #include <filesystem>

// Use Loguru (https://github.com/emilk/loguru) for logging
//#include "../lib/logging/loguru.hpp"
//#include "../lib/logging/loguru.cpp"

// IMPORT TurboPutative LIBRARIES
#include "../lib/ConfigReader.hpp"
#include "../lib/GenericTableHandler.hpp"
#include "../lib/Tokenizer.hpp"

// DEFINES
#define MODULE "RowMerger"

#define SEPARATOR " // "


// MAIN FUNCTION
int main(int argc, char *argv[])
{
    // get path to working directory

    std::string workDirPath = argv[1];

    // configure logging using loguru --> https://github.com/emilk/loguru
    /*std::string logFilePath = workDirPath + "/RowMerger.log";    // Add file and convert to string
    char* logFilePathPtr = &logFilePath[0]; // Get pointer to first character (received by loguru)

    loguru::init(argc, argv);
    loguru::add_file(logFilePathPtr, loguru::Append, loguru::Verbosity_MAX);*/
    
    // read config file
    ConfigReader config(workDirPath);
    config.readConfigFile(MODULE);

    // read input table
    GenericTableHandler msTable;
    msTable.readTable(workDirPath, config.getValue("infile"));

    //
    // APPLY FUSE ROW
    //

    // --> get vector containing compared and conserved columns
    Tokenizer tokenizer(",");
    std::vector<std::string> comparedVec = tokenizer.tokenize(config.getValue("compared_columns"));
    std::vector<std::string> conservedVec = tokenizer.tokenize(config.getValue("conserved_columns"));

    // --> get name of the column with compound name --> if "": no compare
    std::string nameCompare = config.getValue("compared_name");

    // --> apply fusion
    msTable.fuseRows(comparedVec, conservedVec, nameCompare, SEPARATOR);

    //
    // WRITE TABLE
    //
    std::string outfilePath = workDirPath;
    outfilePath += "/" + config.getValue("outfile");
    msTable.writeTable(outfilePath);

    return 0;
}