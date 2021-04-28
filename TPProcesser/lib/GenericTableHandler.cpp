#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
// #include <filesystem>
#include <regex> // boost regex is no necessary; We should apply boost only to regexObject

#include "logging/loguru.hpp"

#include "GenericTableHandler.hpp"
#include "Tokenizer.hpp"

#define DELIM "\t"

/*
GenericTableHandler::GenericTableHandler()
{

}
*/

void GenericTableHandler::readTable(std::string workDirPath, std::string inFileName)
{
    std::string fullPath = workDirPath + "/" + inFileName;

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

std::string GenericTableHandler::getValue(int& row, int& col)
{
    return table[col][row];
}

void GenericTableHandler::addColumn(std::vector<std::string> columnArr, int position, std::string colName)
{
    table.insert(table.begin()+position, columnArr);
    columns.insert(columns.begin()+position, colName);
}

void GenericTableHandler::removeColumn(std::string colName)
{
    // remove element from column
    std::vector<std::string>::iterator it = std::find(columns.begin(), columns.end(), colName);
    columns.erase(it);

    // remove element from table
    int colPos = it - columns.begin();
    table.erase(table.begin()+colPos);
}

std::vector<std::string>& GenericTableHandler::getColumnNames()
{
    return columns;
}

int GenericTableHandler::getNrows()
{
    return nrows;
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

    nrows -= indexes.size();

    log.str("");
    log << "Indexes removed";
    LOG_F(INFO, &(log.str()[0]));
}

void GenericTableHandler::writeTable(std::string outfilePath)
{
    // logging
    std::stringstream log;
    log << "Start writing table";
    LOG_F(INFO, &(log.str()[0]));

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

    log.str("");
    log << "Table was written";
    LOG_F(INFO, &(log.str()[0]));
}

void GenericTableHandler::fuseRows(std::vector<std::string> compareCol, std::vector<std::string> conserveCol, std::string name)
{
    // logging
    std::stringstream log;
    log << "Start row fusion";
    LOG_F(INFO, &(log.str()[0]));

    // Extract index of comparing and conserving columns
    std::vector<int> compareIdx={}, conserveIdx={};

    for (std::string& col : compareCol)
    {
        std::vector<std::string>::iterator it;
        it = std::find(columns.begin(), columns.end(), col);
        compareIdx.push_back(it-columns.begin());        
    }

    for (std::string& col : conserveCol)
    {
        std::vector<std::string>::iterator it;
        it = std::find(columns.begin(), columns.end(), col);
        conserveIdx.push_back(it-columns.begin());
    }

    // Extract index of column with compound names
    int nameIdx = -1;
    if (name != "")
    {
        std::vector<std::string>::iterator it;
        it = std::find(columns.begin(), columns.end(), name);
        nameIdx = it-columns.begin();
    }

    // Sort rows by sorting first conserving column (only if there is a conserving column)
    std::vector<int> sortedRowIdx;
    sortedRowIdx.reserve(nrows);
    if (compareIdx.size() > 0)
    {
        // --> Create vector of pairs
        std::vector<std::pair<std::string, int>> pairColIdx;
        pairColIdx.reserve(nrows);
        for (int i=0; i<nrows; i++)
        {
            pairColIdx.push_back(std::make_pair(table[compareIdx[0]][i], i));
        }

        // --> Sort vector of pairs (by default it is done using first element)
        std::sort(pairColIdx.begin(), pairColIdx.end());

        // --> Extract sorted row indexes
        for (std::pair<std::string, int>& colIdxPair : pairColIdx)
        {
            sortedRowIdx.push_back(colIdxPair.second);
        }

    } else {
        for (int i=0; i<nrows; i++)
        {
            sortedRowIdx.push_back(i);
        }
    }

    // Create new table (fused table)
    std::vector<std::vector<std::string>> fusedTable;
    for (int i=0; i<columns.size(); i++)
    {
        std::vector<std::string> col;
        col.reserve(nrows);
        fusedTable.push_back(col);
    }

    // Loop row by row
    std::vector<int> processedRows;
    processedRows.reserve(nrows);
    for (int i=0; i<sortedRowIdx.size(); i++)
    {
        
        // if this row has been processed, continue, pleeeease...
        std::vector<int>::iterator it = std::find(processedRows.begin(), processedRows.end(), sortedRowIdx[i]);
        if (it != processedRows.end()) continue;

        // create new row = table[:][i]
        std::vector<std::string> newRow;
        for (int k=0; k<columns.size(); k++)
        {
            newRow.push_back(table[k][sortedRowIdx[i]]);
        }

        // compare with below rows
        for (int j=i+1; j<sortedRowIdx.size(); j++)
        {

            // if this row has been processed, continue, pleeeease...
            std::vector<int>::iterator it = std::find(processedRows.begin(), processedRows.end(), sortedRowIdx[j]);
            if (it != processedRows.end()) continue;

            // if first compare are different, break (there must be a first compare column...)
            if (compareIdx.size()>0) if (table[compareIdx[0]][sortedRowIdx[i]] != table[compareIdx[0]][sortedRowIdx[j]]) break;

            // compare rest of compare columns. If different, continue
            bool equal = true;
            for (int k=0; k<compareIdx.size(); k++)
            {
                if (table[compareIdx[k]][sortedRowIdx[i]] != table[compareIdx[k]][sortedRowIdx[j]] ||
                    table[compareIdx[k]][sortedRowIdx[i]] == "N/A" ||
                    table[compareIdx[k]][sortedRowIdx[j]] == "N/A")
                {
                    equal = false;
                    break;
                }
            }
            if (!equal) continue;

            // compare names, if it is indicated. If different, continue
            if (nameIdx != -1)
            {
                if (!compareNames(table[nameIdx][sortedRowIdx[i]], table[nameIdx][sortedRowIdx[j]])) continue;
            }

            // At this point, both rows must be fused
            // --> Loop conserved column
            for (int k=0; k<conserveIdx.size(); k++)
            {
                newRow[conserveIdx[k]] = conserveField(newRow[conserveIdx[k]], table[conserveIdx[k]][sortedRowIdx[j]]);
            }

            processedRows.push_back(sortedRowIdx[j]);

        }

        // Add newRow to table
        for (int k=0; k<newRow.size(); k++)
        {
            fusedTable[k].push_back(newRow[k]);
        }
    }

    table = fusedTable;
    nrows = fusedTable[0].size();

    // logging
    log.str("");
    log << "End row fusion";
    LOG_F(INFO, &(log.str()[0]));
}

bool GenericTableHandler::compareNames(std::string& name1, std::string& name2)
{
    std::regex re("[-\\s()[\\]{}]");

    std::string name1_rep, name2_rep;

    name1_rep = std::regex_replace(name1, re, "");
    std::transform(name1_rep.begin(), name1_rep.end(), name1_rep.begin(), [](unsigned char c) { return std::tolower(c); });

    name2_rep = std::regex_replace(name2, re, "");
    std::transform(name2_rep.begin(), name2_rep.end(), name2_rep.begin(), [](unsigned char c) { return std::tolower(c); });

    bool equal = (name1_rep == name2_rep);

    return equal;
}

std::string GenericTableHandler::conserveField(std::string& field1, std::string& field2)
{
    std::string fieldDelim = " // ";
    std::string outField = "";

    if (field2 == "N/A" || field2 == "")
    {
        outField = field1;
    } else if (field1 == "N/A" || field1 == "")
    {
        outField = field2;
    } else
    {
        Tokenizer tokenizer(fieldDelim);
        std::vector<std::string> tokens1 = tokenizer.tokenize(field1);
        std::vector<std::string> tokens2 = tokenizer.tokenize(field2);

        for (std::string token2_i : tokens2)
        {
            std::vector<std::string>::iterator it = std::find(tokens1.begin(), tokens1.end(), token2_i);
            if (it == tokens1.end())
            {
                tokens1.push_back(token2_i);
            }
        }

        for (std::string token1_i : tokens1)
        {
            outField += token1_i + fieldDelim;
        }

        outField = outField.substr(0, outField.length()-fieldDelim.length());
    }

    return outField;
    
}