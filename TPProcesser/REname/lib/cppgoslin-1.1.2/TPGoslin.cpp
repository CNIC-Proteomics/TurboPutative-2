#include <vector>
#include <string>
#include <fstream>
//#include <experimental/filesystem>
#include <iostream>

#include "cppgoslin/cppgoslin.h"

/* DECLARE FUNCTION */


/* MAIN FUNCTION */
int  main(int argc, char *argv[])
{
    std::string workDirPath = argv[1];

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

    
}