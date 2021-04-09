#ifndef REMOVE_ROW
#define REMOVE_ROW

#include <string>
#include <regex>
#include <vector>

class RemoveRow
{
    protected:

    std::regex removeRE;

    public:

    // constructor
    RemoveRow(std::string regexStr);

    // detect indexes to be removed
    std::vector<int> getIndexes(std::vector<std::string>& compoundNamesColumn);

};

#endif