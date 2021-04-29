#ifndef SYNONYMS_READER
#define SYNONYMS_READER

// #include <filesystem>
#include <vector>
#include <string>

#define SYNONYM_PATH ".\\TPProcesser\\REname\\data\\synonyms.tsv"

class SynonymsReader
{
    protected:
    std::vector<std::string> key, value;
    std::string synonymPath = SYNONYM_PATH;

    public:
    // constructor
    SynonymsReader();

    // apply substitution
    void replace(std::vector<std::string>& compoundNamesColumn);

};

#endif