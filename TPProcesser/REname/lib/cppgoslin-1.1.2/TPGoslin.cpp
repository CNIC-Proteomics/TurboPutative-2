#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <regex>
#include <chrono>
#include <ctime>

#include "cppgoslin/cppgoslin.h"


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
    // READ COMPOUNDS
    //
    std::vector<std::string> compoundNames;
    compoundNames.reserve(20000);

    std::ifstream compoundFile(workDirPath+"\\compound.txt");
    std::string line;
    while(std::getline(compoundFile, line))
    {
        compoundNames.push_back(line);
    }

    logFile << "** compound.txt was read" << std::endl;

    //
    // APPLY GOSLIN
    //
    logFile << "** Applying Goslin" << std::endl;

    std::vector<std::string> parsedCompoundNames;

    LipidParser lipid_parser;
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

            std::string parsedCompound = "###" + header_group + "(" + bond_type + std::to_string(carbon_atoms) + 
                                         ":" + std::to_string(double_bonds) + methyl + hydroxyl + ")" + "###";
        
            parsedCompoundNames.push_back(parsedCompound);

        }
        catch(const std::exception& e)
        {
            logFile << "** Error with compound: " << compound << std::endl;
            logFile << "** " << e.what() << '\n';
            parsedCompoundNames.push_back(compound);
        }
    }

    //
    // WRITE PARSED COMPOUNDS
    //

    std::ofstream parsedFile (workDirPath+"\\parsed_compound.txt");

    for (std::string& parsedCompound : parsedCompoundNames)
    {
        parsedFile << parsedCompound << std::endl;
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