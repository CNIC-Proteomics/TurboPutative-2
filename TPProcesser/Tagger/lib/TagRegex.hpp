#ifndef TAG_REGEX
#define TAG_REGEX

#include <string>
#include <regex>
#include <vector>

class TagRegex
{
    protected:

    std::regex regexTag;
    std::string tag;

    public:
    // constructor
    TagRegex(std::string regexStr, std::string tag, bool icase=false);

    // method to add tag
    std::vector<std::string> addTag(std::vector<std::string>& compoundNamesColumn);
};

#endif