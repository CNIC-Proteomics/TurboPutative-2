#include "SortPeptide.hpp"
#include <vector>
#include <string>
#include <sstream>
#include "boost/regex.hpp"
#include <algorithm>

#include "../../lib/logging/loguru.hpp"


SortPeptide::SortPeptide(std::string sep)
{
    separator = sep;
}

void SortPeptide::sortAA(std::vector<std::string>& compoundNamesColumn)
{
    // logging
    std::stringstream log;
    log << "Sorting peptides";
    LOG_F(INFO, &(log.str()[0]));

    for (std::string& compound : compoundNamesColumn)   
    {
        std::string compound_tmp = compound;

        int firstPos = 0;
        
        // split by separator --> Extract all amino acids
        std::vector<std::string> tokens;
        int sepIdx;
        boost::smatch matchObject;
        bool isMatch = boost::regex_search(compound_tmp, matchObject, boost::regex(separator));

        while (isMatch)
        {
            std::string pref = matchObject.prefix();
            if (isMatch) sepIdx = pref.length();

            // extract amino acid
            tokens.push_back(pref);

            // prepare for next iteration
            firstPos = sepIdx+1;
            compound_tmp = compound_tmp.substr(firstPos);
            isMatch = boost::regex_search(compound_tmp, matchObject, boost::regex(separator));
        }
        // extract last amino acid
        tokens.push_back(compound_tmp);

        // check if all tokens are amino acids --> Check if compound is a peptide
        if (isPeptide(tokens))
        {
            // sort amino acids
            std::sort(tokens.begin(), tokens.end());

            // build peptide
            std::string sortedPeptide = "";
            for (std::string& token : tokens)
            {
                sortedPeptide += token + " ";
            }
            
            compound = sortedPeptide.substr(0, sortedPeptide.length()-1);
        }
    }

    // logging
    log.str("");
    log << "Finish sorting peptides";
    LOG_F(INFO, &(log.str()[0]));
}

bool SortPeptide::isPeptide(std::vector<std::string> tokens)
{
    for (std::string& token : tokens)
    {
        bool isAA;
        for (std::string& aa : aminoAcids)
        {
            isAA = boost::regex_match(token, boost::regex(aa, boost::regex::icase));
            if (isAA) break;
        }

        if (!isAA) return false;
    }

    return true;
}