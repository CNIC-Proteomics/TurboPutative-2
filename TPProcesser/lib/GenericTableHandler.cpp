#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <filesystem>

#include "logging/loguru.hpp"

#include "GenericTableHandler.hpp"
#include "Tokenizer.hpp"

#define DELIM "\t"

/*
GenericTableHandler::GenericTableHandler()
{

}
*/

void GenericTableHandler::readTable(std::filesystem::path workDirPath, std::string inFileName)
{
    std::filesystem::path fullPath = workDirPath / inFileName;

    // logging
    std::stringstream log;
    log << "Reading " << inFileName << " table: " << fullPath;
    LOG_F(INFO, &(log.str()[0]));

    // open file
    std::ifstream tableFile(fullPath);

    std::string line;

    // get columns
    std::getline(tableFile, line);
    Tokenizer tokenizer(DELIM);
    columns = tokenizer.tokenize(line);

    // add each column (vector) to table
    for (int i=0; i<columns.size(); i++)
    {
        std::vector<std::string> vec;
        vec.reserve(50000); // reserve 50,000 positions for each vector
        table.push_back(vec);
    }

    // read each line, extract values and add to the corresponding column
    while(std::getline(tableFile, line))
    {
        std::vector<std::string> values = tokenizer.tokenize(line);

        for (int i=0; i<values.size(); i++)
        {
            table[i].push_back(values[i]);
        }

        nrows++;
    }

    log.str("");
    log << "Table read successfully";
    LOG_F(INFO, &(log.str()[0]));

}

std::vector<std::string>& GenericTableHandler::getColumn(std::string colName)
{
    // Get iterator to position of that colname
    std::vector<std::string>::iterator it = std::find(columns.begin(), columns.end(), colName);

    // Get index of that name
    int colNameIndex = it - columns.begin();

    // Get and return column 
    std::vector<std::string>& column = table[colNameIndex];

    return column;
}

void GenericTableHandler::addColumn(std::vector<std::string> columnArr, int position, std::string colName)
{
    table.insert(table.begin()+position, columnArr);
    columns.insert(columns.begin()+position, colName);
}

std::vector<std::string>& GenericTableHandler::getColumnNames()
{
    return columns;
}

int GenericTableHandler::getIndexOfColumn(std::string colName)
{
    std::vector<std::string>::iterator it;
    it = std::find(columns.begin(), columns.end(), colName);
    int idx = std::distance(columns.begin(), it);

    return idx;
}

void GenericTableHandler::removeIndexes(std::vector<int>& indexes)
{
    // logging
    std::stringstream log;
    log << "Removing indexes";
    LOG_F(INFO, &(log.str()[0]));

    std::string rmTag = "###RM###";
    
    for (int j=0; j<table.size(); j++)
    //for (std::vector<std::string>& col : table)
    {
        for (int i=indexes.size()-1; i>=0; i--)
        {
            // table[j].erase(table[j].begin()+indexes[i]);
            table[j][indexes[i]] = rmTag;
        }
        
    }

    for (int j=0; j<table.size(); j++)
    {
        std::vector<std::string>::iterator it = std::remove(table[j].begin(), table[j].end(), rmTag);
        table[j].erase(it, table[j].end());
    }

    log.str("");
    log << "Indexes removed";
    LOG_F(INFO, &(log.str()[0]));
}

void GenericTableHandler::writeTable(std::filesystem::path outfilePath)
{
    std::ofstream myFile(outfilePath);

    // write columns
    std::string colStr = "";
    for (std::string& i : columns)
    {
        colStr += i + "\t";
    }

    colStr.pop_back();
    myFile << colStr << std::endl;

    // write data
    for (int i=0; i<nrows; i++)
    {
        std::string rowStr="";

        for (int j=0; j<columns.size(); j++)
        {
            rowStr += table[j][i] + "\t";
        }

        rowStr.pop_back();
        myFile << rowStr << std::endl;
    }
}