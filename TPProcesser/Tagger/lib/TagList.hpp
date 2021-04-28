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

        // vector containing compounds in index and their position in list
        std::vector<std::string> index;
        std::vector<int> indexMap;

        // mapping from compound in index to position in list
        //std::map<std::string, int> indexMap;

        // method to read list file and get list
        void readList(std::string relativePath);

        // method to get position when sorting alphabetically
        int getSortedPos(std::string compound);

        bool checkPresence(std::string compound, int init, int last);

    public:

        // constructor
        TagList(std::string strRelativePath);

        // method to get index and indexMap
        void buildIndex();

        // method to add tag --> Returns the tag column (vector of strings) that will be added to the table
        std::vector<std::string> addTag(std::vector<std::string>& compoundNamesColumn, std::string tag);

        // METHOD TO ADD TAG WITHOUT INDEX
        std::vector<std::string> addTagNoIndex(std::vector<std::string>& compoundNamesColumn, std::string tag);
};

#endif // TAG_LIST