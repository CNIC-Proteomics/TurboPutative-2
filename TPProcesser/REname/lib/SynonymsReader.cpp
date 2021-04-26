#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <regex>

#include "SynonymsReader.hpp"
#include "../../lib/Tokenizer.hpp"
#include "../../lib/logging/loguru.hpp"

SynonymsReader::SynonymsReader()
{
    // logging
    std::stringstream log;
    log << "Reading synonyms";
    LOG_F(INFO, &(log.str()[0]));

    Tokenizer tokenizer("\t");
    std::ifstream synonymFile(synonymPath);

    std::regex re("[-\\s()[\\]{}]");

    std::string line;
    std::getline(synonymFile, line); // remove first line

    while (std::getline(synonymFile, line))
    {
        std::vector<std::string> tokens = tokenizer.tokenize(line);
        std::string preKey = tokens[0];
        std::transform(preKey.begin(), preKey.end(), preKey.begin(), [](unsigned char c) { return std::tolower(c); });
        preKey = std::regex_replace(preKey, re, "");

        key.push_back(preKey);
        value.push_back(tokens[1]);
    }

    log.str("");
    log << "Synonyms read";
    LOG_F(INFO, &(log.str()[0]));
}

void SynonymsReader::replace(std::vector<std::string>& compoundNamesColumn)
{
    // logging
    std::stringstream log;
    log << "Applying synonyms substitution";
    LOG_F(INFO, &(log.str()[0]));

    std::regex re("[-\\s()[\\]{}]");

    for  (int i=0; i<compoundNamesColumn.size(); i++)
    {
        std::string compoundLower = compoundNamesColumn[i];
        std::transform(compoundLower.begin(), compoundLower.end(), compoundLower.begin(), 
                        [](unsigned char c) { return std::tolower(c); });
        
        compoundLower = std::regex_replace(compoundLower, re, "");
        
        std::vector<std::string>::iterator itr;
        itr = std::find(key.begin(), key.end(), compoundLower);

        if (itr != key.end())
        {
            compoundNamesColumn[i] = value[itr-key.begin()];
        }
    }

    log.str("");
    log << "Finished synonym substitution";
    LOG_F(INFO, &(log.str()[0]));
}