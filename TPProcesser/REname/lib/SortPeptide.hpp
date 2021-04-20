#ifndef SORT_PEPTIDE
#define SORT_PEPTIDE

#include <vector>
#include <string>

class SortPeptide
{
    protected:
    std::vector<std::string> aminoAcids = {
        "Ala", "Arg", "Asn", "Asp", "Cys",
        "Gln", "Glu", "Gly", "His", "Ile",
        "Leu", "Lys", "Met", "Phe", "Pro",
        "Ser", "Thr", "Trp", "Tyr", "Val"        
    };

    std::string separator;

    // Method to determine if all tokens are amino acids
    bool isPeptide(std::vector<std::string> aas);

    public:
    // Constructor
    SortPeptide(std::string sep);
    
    // Method to sort peptide amino acids
    void sortAA(std::vector<std::string>& compoundNamesColumn);

};


#endif