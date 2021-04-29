#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <regex>
#include <chrono>
#include <ctime>
#include <regex>

#include "cppgoslin/cppgoslin.h"

// Constants
// FAHFA(15:0-(18-O-22:0))
#define FAHFA_REGEX "^(FAHFA)\\((\\d+):(\\d+)-\\(\\d+-O-(\\d+):(\\d+)\\)\\)"

/* DECLARE FUNCTION */
std::string getHydroxyl(std::string& compound);
std::string getMethyl(std::string& compound);


/* MAIN FUNCTION */
int  main(int argc, char *argv[])
{
    std::string workDirPath = argv[1];

    //
    // LOG FOR GOSLIN
    //
    std::ofstream logFile (workDirPath+"/TPGoslin.log");
    auto start = std::chrono::system_clock::now();
    std::time_t start_time = std::chrono::system_clock::to_time_t(start);
    logFile << "** Start script: " << std::ctime(&start_time) << std::endl;

    //
    // DEFINE REGEX TO DETECT FAHFA
    //
    std::regex reFAHFA(FAHFA_REGEX);

    //
    // READ COMPOUNDS
    //
    std::vector<std::string> compoundNames;
    compoundNames.reserve(20000);

    std::ifstream compoundFile(workDirPath+"/compound.txt");
    std::string line;
    while(std::getline(compoundFile, line))
    {
        compoundNames.push_back(line);
    }

    //
    // READ INDEX OF THESE COMPOUNDS
    //
    std::vector<int> compoundIndexes;
    compoundIndexes.reserve(compoundNames.size());

    std::ifstream idxFile(workDirPath+"/compound_index.txt");
    line = "";
    while(std::getline(idxFile, line))
    {
        compoundIndexes.push_back(std::stoi(line));
    }

    logFile << "** compound.txt and compound_index.txt were read" << std::endl;

    //
    // APPLY GOSLIN
    //
    logFile << "** Applying Goslin" << std::endl;

    std::vector<std::string> parsedCompoundNames;
    std::vector<int> parsedCompoundIndexes;

    LipidParser lipid_parser;
    int count = 0;
    for (std::string& compound : compoundNames)
    {
        try
        {
            LipidAdduct* lipid = lipid_parser.parse(compound);

            std::vector<FattyAcid*> fa_list = lipid->lipid->get_fa_list();

            // If lipid does not have fatty acid, do not parse
            if (fa_list.size() == 0)
            {
                parsedCompoundNames.push_back(compound);
                continue;
            }

            // Take header group
            std::string header_group = lipid->get_lipid_string(goslin::CLASS);
            std::regex lyso("^lyso", std::regex_constants::ECMAScript | std::regex_constants::icase);
            header_group = std::regex_replace(header_group, lyso, "L");

            // Take information from fatty acids
            int carbon_atoms = 0;
            int double_bonds = 0;
            std::string bond_type = "";

            for (FattyAcid* fa : fa_list)
            {
                carbon_atoms += (*fa).num_carbon;
                double_bonds += (*fa).num_double_bonds;
                // hydroxyl += (*fa).num_hydroxyl;
                
                std::string fa_bond_type = FattyAcid::suffix((*fa).lipid_FA_bond_type);
                if (fa_bond_type == "a")
                {
                    bond_type = "O-";
                } else if (fa_bond_type == "p")
                {
                    bond_type = "P-";
                    double_bonds--;
                }
            }

            std::string hydroxyl = getHydroxyl(compound);
            std::string methyl = getMethyl(compound);

            std::string parsedCompound = header_group + "(" + bond_type + std::to_string(carbon_atoms) + 
                                         ":" + std::to_string(double_bonds) + methyl + hydroxyl + ")";
        
            parsedCompoundNames.push_back(parsedCompound);
            parsedCompoundIndexes.push_back(compoundIndexes[count]);

        }
        catch(const std::exception& e)
        {
            logFile << "** Error with compound: " << compound << std::endl;
            logFile << "** " << e.what() << '\n';

            std::smatch matchObject;
            if (std::regex_search(compound, matchObject, reFAHFA))
            {
                int Catoms = std::stoi(matchObject.str(2)) + std::stoi(matchObject.str(4));
                int dBonds = std::stoi(matchObject.str(3)) + std::stoi(matchObject.str(5));
                
                std::string parsedCompound = matchObject.str(1) + "(" + std::to_string(Catoms) + ":" + std::to_string(dBonds) + ")";
                parsedCompoundNames.push_back(parsedCompound);
                parsedCompoundIndexes.push_back(compoundIndexes[count]);
                logFile << "** " << compound << "was manually parsed to " << parsedCompound << std::endl;
            } else
            {
                parsedCompoundNames.push_back(compound);
            }
        }
        count++;
    }

    //
    // WRITE PARSED COMPOUNDS
    //

    std::ofstream parsedFile (workDirPath+"/parsed_compound.txt");

    for (std::string& parsedCompound : parsedCompoundNames)
    {
        parsedFile << parsedCompound << std::endl;
    }

    //
    // APPEND TO mappedIndex.txt THE INDEX OF COMPOUNDS PARSED BY TPGOSLIN
    //
    std::ofstream mappedIndexFile (workDirPath+"/mappedIndex.txt", std::ios_base::app);

    for (int& i : parsedCompoundIndexes)
    {
        mappedIndexFile << i << std::endl;
    }


    //
    // END SCRIPT
    //
    auto end = std::chrono::system_clock::now();
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
    logFile << "** End script: " << std::ctime(&end_time) << std::endl;

    return 0;
}

/* FUNCTION DEFINITION */

std::string getHydroxyl(std::string& compound)
{
    int hydroxyl = 0;
    std::string compound_tmp = compound;

    std::regex re("[(,]\\d*OH(\\[[\\w,]+\\])?(?=[),])");
    std::smatch matchObject;

    while (std::regex_search(compound_tmp, matchObject, re))
    {
        hydroxyl++;
        compound_tmp = matchObject.suffix().str();
    }

    if (hydroxyl == 0)
    {
        return "";
    } else if (hydroxyl == 1)
    {
        return "(OH)";
    } else
    {
        return "(" + std::to_string(hydroxyl) + "OH)";
    }
}

std::string getMethyl(std::string& compound)
{
    int methyl = 0;
    std::string compound_tmp = compound;

    std::regex re("[,(]\\d+(\\([RS]\\))?Me(?=[,)])");
    std::smatch matchObject;

    while (std::regex_search(compound_tmp, matchObject, re))
    {
        methyl++;
        compound_tmp = matchObject.suffix().str();
    }

    if (methyl == 0)
    {
        return "";
    } else if (methyl == 1)
    {
        return "(Me)";
    } else
    {
        return "(" + std::to_string(methyl) + "Me)";
    }
}