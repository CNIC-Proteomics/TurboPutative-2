/*
MIT License

Copyright (c) 2020 Dominik Kopczynski   -   dominik.kopczynski {at} isas.de
                   Nils Hoffmann  -  nils.hoffmann {at} isas.de

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


#ifndef LIPID_STRUCTURAL_SUBSPECIES_H
#define LIPID_STRUCTURAL_SUBSPECIES_H

#include <string>
#include "LipidExceptions.h"
#include "LipidSpeciesInfo.h"
#include "LipidEnums.h"
#include "LipidMolecularSubspecies.h"
#include <sstream>
#include <vector>
#include "FattyAcid.h"
#include <map>

using namespace std;
using namespace goslin;

class LipidStructuralSubspecies : public LipidMolecularSubspecies {
public:
    
    LipidStructuralSubspecies(string head_group);
    LipidStructuralSubspecies(string head_group, vector<FattyAcid*> *_fa);
    ~LipidStructuralSubspecies();
    string get_lipid_string(LipidLevel level = NO_LEVEL);
    LipidLevel get_lipid_level();
};

#endif /* LIPID_STRUCTURAL_SUBSPECIES_H */