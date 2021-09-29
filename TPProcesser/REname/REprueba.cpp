#include <string>
#include <vector>

#include "./lib/LipidMapsAbb.hpp"

#define LMSD_TABLE_PATH "./src/TurboPutative-2.0-built/TPProcesser/REname/data/LMSD_abbreviation.tsv"

int main()
{
    std::vector<std::string> compoundNamesOriginal = {
        "17:1(15Me)",
        "Acetylenic acids",
        "Geranylgeranoic acid",
        "PE 36:1",
        "PE(36:1)",
        "decyl hexanoate"
    };

    std::vector<std::string> compoundNamesColumn = {
        "17:1(15Me)",
        "Acetylenic acids",
        "Geranylgeranoic acid",
        "PE 36:1",
        "PE(36:1)",
        "decyl hexanoate"
    };

    std::vector<int> mappedIndex = {4};

    // Search in LipidMaps
    LipidMapsAbb lipidMapsAbb;
    lipidMapsAbb.readLMSDTable(LMSD_TABLE_PATH);
    lipidMapsAbb.findCompounds(compoundNamesColumn, compoundNamesOriginal, mappedIndex);

}