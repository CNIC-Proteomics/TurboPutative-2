#ifndef LIPID_LIST
#define LIPID_LIST

#include <vector>
#include <string>
//#include <experimental/filesystem>
// #include <regex>
#include <map>

#include "boost/regex.hpp"

#define LIPID_LIST_PATH ".\\TPProcesser\\REname\\data\\goslinLipids.txt"

class LipidList
{
    protected:
    std::vector<std::string> lipidList;
    std::string lipidListPath = LIPID_LIST_PATH;

    public:
    // Constructor
    LipidList();

    // Method to identify lipids in the list
    std::vector<bool> findLipids(std::vector<std::string>& compoundNames, std::vector<int>& mappedLipids);
};


class PreProcessLipids
{
    protected:
    // Each vector has a pair: regex : replace
    std::vector<std::map<std::string, std::string>> REs = {
        {{"regex", "\\[\\w+\\]"}, {"replace", ""}},
        {{"regex", "(?<=[\\(/])[ia]-(?=\\d)"}, {"replace", ""}}
        // {{"regex", "\\n.*$"}, {"replace", ""}}
    };

    public:
    // Constructor
    // PreProcessLipid();

    // Preprocess
    void preProcessLipids(std::vector<std::string>& compoundNames);

};

#endif