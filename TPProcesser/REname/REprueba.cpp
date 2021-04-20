#include <string>
#include <vector>

#include "lib/RegexObject.hpp"
#include "../lib/logging/loguru.hpp"
#include "../lib/logging/loguru.cpp"

int main()
{
    std::vector<std::string> compoundNamesColumn = {
        "17:1(15Me)",
        "17:1(15Me)"
    };

    RegexObject REObject;
    REObject.readRegexINI();
    REObject.applyRegex(compoundNamesColumn);

}