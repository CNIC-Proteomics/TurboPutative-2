#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <math.h>
#include <cmath>

// Use Loguru (https://github.com/emilk/loguru) for logging
#include "../lib/logging/loguru.hpp"
#include "../lib/logging/loguru.cpp"

// IMPORT TurboPutative LIBRARIES
#include "../lib/ConfigReader.hpp"
#include "../lib/GenericTableHandler.hpp"
// #include "../lib/Tokenizer.hpp"

// DEFINES
#define MODULE "TableMerger"


// DECLARE FUNCTIONS
std::vector<int> findMatch(double& window, std::string msEMi, std::vector<std::string>& tmEM, bool rtBool, std::string msRTi, std::vector<std::string>& tmRT);
std::vector<std::vector<std::string>> combineRows(std::vector<int>& matches);

// MAIN FUNCTION
int main(int argc, char *argv[])
{
    // get path to working directory
    std::filesystem::path workDirPath = argv[1];

    // configure logging using loguru --> https://github.com/emilk/loguru
    std::string logFilePath = (workDirPath / "TableMerger.log").string();    // Add file and convert to string
    char* logFilePathPtr = &logFilePath[0]; // Get pointer to first character (received by loguru)

    loguru::init(argc, argv);
    loguru::add_file(logFilePathPtr, loguru::Append, loguru::Verbosity_MAX);
    
    // read config file
    ConfigReader config(workDirPath);
    config.readConfigFile(MODULE);

    // read MS input table
    GenericTableHandler msTable;
    msTable.readTable(workDirPath, config.getValue("infile"));

    // read TM input table (feature information)
    GenericTableHandler tmTable;
    tmTable.readTable(workDirPath, config.getValue("tmfile"));

    //
    // COMBINE BOTH TABLES
    //

    // --> Extract columns with experimental mass
    std::vector<std::string> msEM = msTable.getColumn(config.getValue("ms_column_mass"));
    std::vector<std::string> tmEM = tmTable.getColumn(config.getValue("tm_column_mass"));

    // --> Determine if retention time is going to be used
    bool rtBool = false;
    std::string msRTcolName = config.getValue("ms_column_rt");
    std::string tmRTcolName = config.getValue("tm_column_rt");

    std::vector<std::string> msRT = {};
    std::vector<std::string> tmRT = {};

    if (msRTcolName!="None" && tmRTcolName!="None")
    {
        rtBool = true;
        msRT = msTable.getColumn(msRTcolName);
        tmRT = tmTable.getColumn(tmRTcolName);
    }


    // --> Create vector with name of the columns present in output table
    std::vector<std::string> outColName = {};

    std::vector<std::string> tmTableColName = tmTable.getColumnNames();
    for (int i=0; i<tmTableColName.size(); i++)
    {
        outColName.push_back(tmTableColName[i]);
    }

    std::vector<std::string> msTableColName = msTable.getColumnNames();
    std::vector<int> msTableIdx = {};
    for (int i=0; i<msTableColName.size(); i++)
    {
        // skip column with experimental mass and retention time as they were added previously
        if (msTableColName[i]==config.getValue("ms_column_mass")) continue;
        if (msTableColName[i]==config.getValue("ms_column_rt") && rtBool) continue;

        outColName.push_back(msTableColName[i]);
        msTableIdx.push_back(i);
    }

    // --> Define output table (vector of row vectors)
    std::vector<std::vector<std::string>> outTable {};

    // --> Compare Experimental mass (and RT) between both tables
    int digits = std::stoi(config.getValue("n_digits"));
    double window_small = 1/std::pow(10, digits) * 1/2 - 1/std::pow(10, digits+2);
    double window_big = 1/std::pow(10, digits) * 1/2 + 1/std::pow(10, digits+2);
    std::vector<int> tmMatchedRows = {};

    for (int i=0; i<msEM.size(); i++)
    {
        std::vector<int> matches = {};

        // Get matches with small window (0.1 +- 0.049)
        if (rtBool)
        {
            matches = findMatch(window_small, msEM[i], tmEM, rtBool, msRT[i], tmRT);
        } else
        {
            matches = findMatch(window_small, msEM[i], tmEM, rtBool, "", tmRT);
        }

        // If there is no match with small window... use a big one (0.1 +- 0.051)
        if (matches.size()==0)
        {
            // Get matches with big window
            if (rtBool)
            {
                matches = findMatch(window_big, msEM[i], tmEM, rtBool, msRT[i], tmRT);
            } else
            {
                matches = findMatch(window_big, msEM[i], tmEM, rtBool, "", tmRT);
            }
        }

        // Get combined rows and add it to output table
        std::vector<std::string> msValues = {};
        for (int& k : msTableIdx)
        {
            msValues.push_back(msTable.getValue(i, k));
        }

        // If there is no match...
        if (matches.size()==0)
        {
            std::vector<std::string> newRow={};
            // Add N/A to positions corresponding to TMtable
            for (int h=0; h<tmTable.getColumnNames().size(); h++)
            {
                newRow.push_back("N/A");
            }
            
            // Add the values of msTable
            for (std::string h : msValues)
            {
                newRow.push_back(h);
            }
            
            outTable.push_back(newRow);
        }

        // If there is match...
        for (int match : matches)
        {
            tmMatchedRows.push_back(match);
            std::vector<std::string> newRow={};

            // Add values from tmTable
            for (int h=0; h<tmTable.getColumnNames().size(); h++)
            {
                newRow.push_back(tmTable.getValue(match, h));
            }

            // Add values from msTable
            for (std::string h : msValues)
            {
                newRow.push_back(h);
            }

            // Add row to output table
            outTable.push_back(newRow);
        }

    }

    // Process rows of TMTable that were not captured
    int nameIdx;
    for (int i=0; i<outColName.size(); i++)
    {
        if (outColName[i]==config.getValue("ms_column_name"))
        {
            nameIdx = i;
        }
    }

    for (int i=0; i<tmTable.getNrows(); i++)
    {
        std::vector<int>::iterator it = std::find(tmMatchedRows.begin(), tmMatchedRows.end(), i);
        if (it == tmMatchedRows.end())
        { // i was not captured
            std::vector<std::string> newRow={};

            // add values from tmTable
            for (int j=0; j<tmTable.getColumnNames().size(); j++)
            {
                newRow.push_back(tmTable.getValue(i, j));
            }

            // add N/A values corresponding to msTable
            for (int j=0; j<msTableIdx.size(); j++)
            {
                newRow.push_back("N/A");
            }

            newRow[nameIdx] = "UNK";
            outTable.push_back(newRow);
        }
    }

    //
    // WRITE TABLE
    //
    std::filesystem::path outfilePath = workDirPath;
    outfilePath /= config.getValue("outfile");

    std::ofstream myFile(outfilePath);

    // --> write column names
    std::string delim = "\t";
    std::string line = "";

    for (std::string i : outColName)
    {
        line += i + delim;
    }
    myFile << line.substr(0, line.length()-delim.length()) << std::endl;

    for (std::vector<std::string>& i : outTable)
    {
        line = "";
        for (std::string& j : i)
        {
            line += j + delim;
        }

        myFile << line.substr(0, line.length()-delim.length()) << std::endl;
    }

    return 0;
}


// DEFINE FUNCTIONS
std::vector<int> findMatch(double& window, std::string msEMi, std::vector<std::string>& tmEM, bool rtBool, std::string msRTi, std::vector<std::string>& tmRT)
{

    // logging
    std::stringstream log;

    double msEMiNum;
    try
    {
        msEMiNum = std::stod(msEMi);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        log << "Error when converting string to double: " << msEMi;
        LOG_F(INFO, &(log.str()[0]));
    }
    
    
    std::vector<int> matches = {};

    for (int i=0; i<tmEM.size(); i++)
    {
        double tmEMiNum, diff;
        try
        {
            tmEMiNum = std::stod(tmEM[i]);
            diff = std::abs(tmEMiNum-msEMiNum);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            log << "Error when converting string to double: " << tmEM[i];
            LOG_F(INFO, &(log.str()[0]));
        }

        if (diff < window) 
        { // if diff is below the threshold we have to check if we work with RT
            if (rtBool) 
            { // if we work with RT, check it and push if they are the same
                if (msRTi == tmRT[i])
                {
                    matches.push_back(i);
                }
            } else
            { // if we do not work with RT, push it directly
                matches.push_back(i);
            }

            
        }

    }

    return matches;
}