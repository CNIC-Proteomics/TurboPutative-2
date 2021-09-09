#ifndef TAG_LIST
#define TAG_LIST

#include <string>
#include <vector>
#include <map>
// #include <filesystem>

class TagList
{
    protected:

        // vector containing name of the compounds
        std::vector<std::string> list;
        std::vector<std::string> identifier;

        // vector containing columns (default: name and id)
        std::vector<std::string> columns;

        // method to read list file and get list
        void readList(std::string relativePath);

        // binary search
        bool binarySearch(int l, int r, std::string& x);


    public:

        // constructor
        TagList(std::string strRelativePath);

        // method to add tag --> Returns the tag column (vector of strings) that will be added to the table
        std::vector<std::string> addTag(std::vector<std::string>& compoundNamesColumn, std::string tag);
};

#endif // TAG_LIST