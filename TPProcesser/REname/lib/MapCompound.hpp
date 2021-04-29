#ifndef MAP_COMPOUND
#define MAP_COMPOUND

#include <vector>
#include <string>
// #include <filesystem>

// Constants
// #define MAP_FILE_PATH "./TPProcesser/REname/data/preProcessedNames.tsv"
#define MAP_FILE_PATH "./src/TurboPutative-2.0-built/TPProcesser/REname/data/preProcessedNames.tsv"
// #define INDEX_FILE_PATH "./TPProcesser/REname/data/preProcessedNamesIndex.tsv"
#define INDEX_FILE_PATH "./src/TurboPutative-2.0-built/TPProcesser/REname/data/preProcessedNamesIndex.tsv"

class MapCompound
{
    protected:

    //
    // Attributes
    //

    std::vector <std::string> original, preProcessed; // Vectors with Original and PreProcessed
    std::vector <std::string> indexKey; // Vector with Compounds used as index
    std::vector <int> indexValue; // Vector with position associated to that compound

    // std::vector <std::string> compoundResult; // Vector with mapped compounds
    std::vector <int> positionResult; // Vector with position of compounds that could be mapped
    

    //
    // Methods
    //

    // Method that receives line read, and returns both tokens
    std::vector<std::string> parseBiTSV (std::string line);

    // Method that get position of the compound in saved indexes
    int getSortedPos (std::string compound);

    // Method to find the position of the compound in preProcessed list. If it is not found -1 is returned
    int getPosInMap (std::string compound, int startIndex, int endIndex);


    public:

    //
    // Methods
    //

    // Method to read list that maps original to preprocessed name
    void readMapFile (std::string path=MAP_FILE_PATH);

    // Method to file with index
    void readIndex (std::string path=INDEX_FILE_PATH);

    // Method to replace compounds of compound list with the preProcessed
    void mapCompounds (std::vector <std::string>& compoundList);

    // Method to write in a file the index of compounds that were mapped
    void writeIndexResult (std::string path);

    // Method to get position of preprocessed compounds
    std::vector <int>& getPositionResult ();


};

#endif