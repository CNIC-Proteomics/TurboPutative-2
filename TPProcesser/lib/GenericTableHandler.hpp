#ifndef GENERIC_TABLE_HANDLER
#define GENERIC_TABLE_HANDLER

#include <string>
#include <vector>
//#include <filesystem>

// Class used to read and handle MS table
class GenericTableHandler
{
    protected:

    int nrows=0;
    std::vector<std::string> columns;
    std::vector<std::vector<std::string>> table;

    // Method to compare names
    bool compareNames(std::string& name1, std::string& name2);

    // Method to combine two fields hen fusing rows
    std::string conserveField(std::string& field1, std::string& field2);


    public:

    // Constructor
    // GenericTableHandler();

    // Table reader
    void readTable(std::string workDirPath, std::string fileName);

    // Method to get column
    std::vector<std::string>& getColumn(std::string colName);
    std::vector <std::string> getColumnCopy(std::string colName);

    // Method to get specific value
    std::string getValue(int& row, int& col);

    // Method to get column names
    std::vector<std::string>& getColumnNames();

    // Method to get number of rows
    int getNrows();

    // Method to get index of a column
    int getIndexOfColumn(std::string colName);

    // Method to add a column at certain position
    void addColumn(std::vector<std::string> columnArr, int position, std::string colName);

    // Method to remove column from column name
    void removeColumn(std::string colName);

    // Method to remove rows given a vector with indexes
    void removeIndexes(std::vector<int>& indexes);

    // Method to fuse rows of the table, given compared and conserved columns
    void fuseRows(std::vector<std::string> compareCol, std::vector<std::string> conserveCol, std::string name="");

    // Method to write output table
    void writeTable(std::string outfilePath);

};

#endif