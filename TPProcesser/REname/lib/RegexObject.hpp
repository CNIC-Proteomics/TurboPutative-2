#ifndef REGEX_OBJECT
#define REGEX_OBJECT

#include <vector>
#include <string>
#include <boost/regex.hpp>

class RegexObject
{
    protected:
    std::vector<boost::regex> REs;
    std::vector<std::string> replaces;

    public:
    
    // Method to read regex.ini
    void readRegexINI(std::string regex_ini_path);

    // Method to apply regex to compounds
    void applyRegex(std::vector<std::string>& compoundNamesColumn, std::vector<int>& mappedIndex);
};

#endif