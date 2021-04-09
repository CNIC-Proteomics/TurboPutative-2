#include "TagRegex.hpp"
#include <string>
#include <vector>
#include <regex>

TagRegex::TagRegex(std::string regexStr, std::string tagStr, bool icase)
{
    if (icase)
    {
        regexTag.assign(regexStr, std::regex::ECMAScript | std::regex::icase);
    } else
    {
        regexTag.assign(regexStr, std::regex::ECMAScript);
    }
    tag = tagStr;
}

std::vector<std::string> TagRegex::addTag(std::vector<std::string>& compoundNamesColumn)
{
    // create vector where tags will be added
    std::vector<std::string> tagColumn;
    tagColumn.reserve(compoundNamesColumn.size());

    for (std::string& compound : compoundNamesColumn)
    {
        std::smatch matchObject;
        bool match = std::regex_search(compound, matchObject, regexTag);
        if (match)
        {
            ;
        }
        match ? tagColumn.push_back(tag) : tagColumn.push_back("");
    }

    return tagColumn;
}