#ifndef LIPID_MAPS_ABB
#define LIPID_MAPS_ABB

#include <string>
#include <vector>

class LipidMapsAbb
{
private:
    /* data */
    
    // Array containing original compounds in lipid maps
    std::vector <std::string> originalNameArr;

    // Array containing associated abbreviations
    std::vector <std::string> abbArr;


    /* methods */
    
    // Method to determine if given compound is present in LMSD
    // Return -1 if it is not present. Otherwise, it returns the index
    int binarySearch(int l, int r, std::string x);

public:
    
    // Read .tsv table containing original compound and its associated abbreviation
    void readLMSDTable(std::string filePath);
    
    /* Check if compounds in compoundNamesOriginal are present in LMSD table.
    In that case, use it abbreviations together with name present in compoundNamesColumn
    (only if it is present in mappedIndex). If it is not present in mappedIndex, use only
    LMSD abbreviation
    */
    void findCompounds(
        std::vector<std::string>& compoundNamesColumn, 
        std::vector<std::string> compoundNamesOriginal, 
        std::vector<int>& mappedIndex
    );

};

#endif