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



#ifndef KNOWN_GRAMMARS_H
#define KNOWN_GRAMMARS_H

#include <string>
using namespace std;
const string goslin_grammar = "/* \n\
 * MIT License \n\
 *  \n\
 * Copyright (c) 2017 Dominik Kopczynski   -   dominik.kopczynski {at} isas.de \n\
 *                    Bing Peng   -   bing.peng {at} isas.de \n\
 *                    Nils Hoffmann  -  nils.hoffmann {at} isas.de \n\
 * \n\
 * Permission is hereby granted, free of charge, to any person obtaining a copy \n\
 * of this software and associated documentation files (the 'Software'), to deal \n\
 * in the Software without restriction, including without limitation the rights \n\
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell \n\
 * copies of the Software, and to permit persons to whom the Software is \n\
 * furnished to do so, subject to the following conditions:; \n\
 *  \n\
 * The above copyright notice and this permission notice shall be included in all \n\
 * copies or substantial portions of the Software. \n\
 *  \n\
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR \n\
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, \n\
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE \n\
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER \n\
 * LIABILITY, WHether IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, \n\
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE \n\
 * SOFTWARE. \n\
*/ \n\
 \n\
/* This is a BNF / ANTLR4 grammar for lipid subspecies identifiers following \n\
 * J.K. Pauling et al. 2017, PLoS One, 12(11):e0188394. \n\
 */ \n\
 \n\
grammar Goslin; \n\
 \n\
 \n\
/* first rule is always start rule */ \n\
lipid : lipid_eof EOF; \n\
lipid_eof : lipid_pure | lipid_pure adduct_info; \n\
lipid_pure : gl | pl | sl | sterol | mediatorc | saccharolipid; \n\
 \n\
 \n\
/* adduct information */ \n\
adduct_info : '[M' adduct ']' charge charge_sign | adduct_separator '[M' adduct ']' charge charge_sign; \n\
adduct : '+H' | '+2H' | '+NH4' | '-H' | '-2H' | '+HCOO' | '+CH3COO' | charge_sign arbitrary_adduct; \n\
arbitrary_adduct : adduct4 | adduct4 adduct4; \n\
adduct4 : adduct2 | adduct2 adduct2; \n\
adduct2 : character | character character; \n\
 \n\
fa2 : fa2_unsorted | fa2_sorted; \n\
fa2_unsorted: fa DASH fa | fa UNDERSCORE fa; \n\
fa2_sorted: fa SLASH fa | fa BACKSLASH fa; \n\
fa3 : fa3_unsorted | fa3_sorted; \n\
fa3_unsorted: fa DASH fa DASH fa | fa UNDERSCORE fa UNDERSCORE fa; \n\
fa3_sorted: fa SLASH fa SLASH fa | fa BACKSLASH fa BACKSLASH fa; \n\
fa4 : fa4_unsorted | fa4_sorted; \n\
fa4_unsorted: fa DASH fa DASH fa DASH fa | fa UNDERSCORE fa UNDERSCORE fa UNDERSCORE fa; \n\
fa4_sorted: fa SLASH fa SLASH fa SLASH fa | fa BACKSLASH fa BACKSLASH fa BACKSLASH fa; \n\
 \n\
/* glycerolipid rules (7 classes) */ \n\
gl : sqml | mgl | dgl | sgl | tgl; \n\
sqml : hg_sqml_full headgroup_separator fa; \n\
mgl : hg_mgl_full headgroup_separator fa; \n\
dgl : hg_dgl_full headgroup_separator gl_species | hg_dgl_full headgroup_separator dgl_subspecies; \n\
sgl : hg_sgl_full headgroup_separator gl_species | hg_sgl_full headgroup_separator dgl_subspecies; \n\
tgl : hg_tgl_full headgroup_separator gl_species | hg_tgl_full headgroup_separator tgl_subspecies; \n\
gl_species : fa; \n\
dgl_subspecies : fa2; \n\
tgl_subspecies : fa3; \n\
 \n\
hg_sqml_full : hg_sqml | hg_mgl heavy_hg; \n\
hg_mgl_full : hg_mgl | hg_mgl heavy_hg; \n\
hg_dgl_full : hg_dgl | hg_dgl heavy_hg; \n\
hg_sgl_full : hg_sgl | hg_sgl heavy_hg; \n\
hg_tgl_full : hg_tgl | hg_tgl heavy_hg; \n\
 \n\
hg_sqml : 'SQMG'; \n\
hg_mgl : 'MAG'; \n\
hg_dgl : 'DAG'; \n\
hg_sgl : 'MGDG' | 'DGDG' | 'SQDG'; \n\
hg_tgl : 'TAG'; \n\
 \n\
 \n\
 \n\
/* phospholipid rules (56 classes) */ \n\
pl : lpl | dpl | tpl | pl_o | cl | mlcl | dlcl; \n\
pl_o : lpl_o | dpl_o; \n\
lpl : hg_lplc headgroup_separator fa; \n\
lpl_o : hg_lpl_oc plasmalogen_separator fa; \n\
dpl : hg_plc headgroup_separator pl_species | hg_plc headgroup_separator pl_subspecies; \n\
tpl : hg_tplc headgroup_separator pl_species | hg_tplc headgroup_separator tpl_subspecies; \n\
dpl_o : hg_pl_oc plasmalogen_separator pl_species | hg_pl_oc plasmalogen_separator pl_subspecies; \n\
dlcl : hg_dlclc headgroup_separator pl_species | hg_dlclc headgroup_separator dlcl_subspecies; \n\
mlcl : hg_mlclc headgroup_separator pl_species | hg_mlclc headgroup_separator mlcl_subspecies; \n\
cl : hg_clc headgroup_separator pl_species | hg_clc headgroup_separator cl_subspecies; \n\
 \n\
pl_species : fa; \n\
pl_subspecies : fa2; \n\
tpl_subspecies : fa3; \n\
dlcl_subspecies : fa2; \n\
mlcl_subspecies : fa3; \n\
cl_subspecies : fa4; \n\
 \n\
heavy_hg : heavy; \n\
 \n\
 \n\
hg_clc : hg_cl | hg_cl heavy_hg; \n\
hg_cl : 'CL'; \n\
hg_mlclc : hg_mlcl | hg_mlcl heavy_hg; \n\
hg_mlcl : 'MLCL'; \n\
hg_dlclc : hg_dlcl | hg_dlcl heavy_hg; \n\
hg_dlcl : 'DLCL'; \n\
hg_plc : hg_pl | hg_pl heavy_hg; \n\
hg_pl : 'BMP' | 'CDP-DAG' | 'DMPE' | 'MMPE' | 'PA' | 'PC' | 'PE' | 'PEt' | 'PG' | 'PI' | hg_pip | 'PS' | 'LBPA' | 'PGP' | 'PPA' | 'Glc-GP' | '6-Ac-Glc-GP' | hg_pim | 'PnC' | 'PnE' | 'PT' | 'PE-NMe2' | 'PE-NMe' | 'PIMIP' | 'CDPDAG'; \n\
hg_pim : 'PIM' hg_pim_number; \n\
hg_pim_number : number; \n\
hg_pip : hg_pip_pure | hg_pip_pure hg_pip_m | hg_pip_pure hg_pip_d | hg_pip_pure hg_pip_t; \n\
hg_pip_pure : 'PIP'; \n\
hg_pip_m : '[3\\']' | '[4\\']' | '[5\\']'; \n\
hg_pip_d : '2' | '2[3\\',4\\']' | '2[4\\',5\\']' | '2[3\\',5\\']'; \n\
hg_pip_t : '3' | '3[3\\',4\\',5\\']'; \n\
hg_tplc : hg_tpl | hg_tpl heavy_hg; \n\
hg_tpl : 'SLBPA' | 'NAPE'; \n\
hg_lplc : hg_lpl | hg_lpl heavy_hg; \n\
hg_lpl : 'LPA' | 'LPC' | 'LPE' | 'LPG' | 'LPI' | 'LPS' | hg_lpim | 'CPA' | 'LCDPDAG' | 'LDMPE' | 'LMMPE' | 'LPIMIP' | 'LPIN'; \n\
hg_lpim : 'LPIM' hg_lpim_number; \n\
hg_lpim_number : number; \n\
hg_lpl_oc : hg_lpl_o ' O' | hg_lpl_o heavy_hg ' O'; \n\
hg_lpl_o : 'LPC' | 'LPE'; \n\
hg_pl_oc : hg_pl_o ' O' | hg_pl_o heavy_hg ' O'; \n\
hg_pl_o : 'PA' | 'PC' | 'PE' | 'PG' | 'PI' | 'PS'; \n\
 \n\
 \n\
 \n\
/* sphingolipid rules (21) */ \n\
sl : lsl | dsl; \n\
lsl : hg_lslc headgroup_separator lcb; \n\
dsl : hg_dslc headgroup_separator sl_species | hg_dslc headgroup_separator sl_subspecies; \n\
 \n\
sl_species : lcb; \n\
sl_subspecies : lcb sorted_fa_separator fa; \n\
 \n\
hg_lslc : hg_lsl | hg_lsl heavy_hg; \n\
hg_lsl : 'LCB' | 'LCBP' | 'LHexCer' | 'LSM'; \n\
hg_dslc : hg_dsl | hg_dsl heavy_hg; \n\
hg_dsl : 'Cer' | 'CerP' | 'EPC' | 'GB4' | 'GD3' | 'GB3' | 'GM3' | 'GM4' | 'Hex3Cer' | 'Hex2Cer' | 'HexCer' | 'IPC' | 'M(IP)2C' | 'MIPC' | 'SHexCer' | 'SM' | 'FMC-5' | 'FMC-6' ; \n\
 \n\
 \n\
 \n\
 \n\
 \n\
/* sterol lipids (2 classes) */ \n\
sterol : stc | ste | stes; \n\
stc : st | st heavy_hg; \n\
st : 'Ch' | 'Cholesterol' | 'ST 27:1;1' | 'ST 27:2;1' | 'ST 28:3;1' | 'ST 30:2;1' | 'ST 29:2;1' | 'ST 28:2;1'; \n\
ste : hg_stc sorted_fa_separator fa; \n\
stes : hg_stcs headgroup_separator fa; \n\
hg_stc : hg_ste | hg_ste heavy_hg; \n\
hg_ste : 'SE 27:1' | 'SE 27:2' | 'SE 28:3' | 'SE 30:2' | 'SE 29:2' | 'SE 28:2'; \n\
hg_stcs : hg_stes | hg_stes heavy_hg; \n\
hg_stes : 'ChE' | 'CE'; \n\
 \n\
 \n\
/* mediator lipids (1 class) */ \n\
mediatorc : mediator | mediator heavy_hg; \n\
mediator : '10-HDoHE' | '11-HDoHE' | '11-HETE' | '11,12-DHET' | '11(12)-EET'| '12-HEPE' | '12-HETE' | '12-HHTrE' | '12-OxoETE' | '12(13)-EpOME' | '13-HODE' | '13-HOTrE' | '14,15-DHET' | '14(15)-EET' | '14(15)-EpETE' | '15-HEPE' | '15-HETE' | '15d-PGJ2' | '16-HDoHE' | '16-HETE' | '18-HEPE' | '5-HEPE' | '5-HETE' | '5-HpETE' | '5-OxoETE' | '5,12-DiHETE' | '5,6-DiHETE' | '5,6,15-LXA4' | '5(6)-EET' | '8-HDoHE' | '8-HETE' | '8,9-DHET' | '8(9)-EET' | '9-HEPE' | '9-HETE' | '9-HODE' | '9-HOTrE' | '9(10)-EpOME' | 'AA' | 'alpha-LA' | 'DHA' | 'EPA' | 'Linoleic acid' | 'LTB4' | 'LTC4' | 'LTD4' | 'Maresin 1' | 'Palmitic acid' | 'PGB2' | 'PGD2' | 'PGE2' | 'PGF2alpha' | 'PGI2' | 'Resolvin D1' | 'Resolvin D2' | 'Resolvin D3' | 'Resolvin D5' | 'tetranor-12-HETE' | 'TXB1' | 'TXB2' | 'TXB3'; \n\
 \n\
 \n\
 \n\
 \n\
/* saccharolipids rules (3 classes) */ \n\
saccharolipid : sac_di | sac_f; \n\
sac_di : hg_sac_di_c headgroup_separator sac_species | hg_sac_di_c headgroup_separator sac_di_subspecies; \n\
hg_sac_di_c : hg_sac_di | hg_sac_di heavy_hg; \n\
hg_sac_di : 'DAT' | 'AC2SGL'; \n\
sac_f : hg_sac_f_c headgroup_separator sac_species | hg_sac_f_c headgroup_separator sac_f_subspecies; \n\
hg_sac_f_c : hg_sac_f | hg_sac_f heavy_hg; \n\
hg_sac_f : 'PAT16' | 'PAT18'; \n\
 \n\
sac_species : fa; \n\
sac_di_subspecies : fa2; \n\
sac_f_subspecies : fa4; \n\
 \n\
 \n\
 \n\
/* generic rules */ \n\
fa : fa_pure | fa_pure heavy_fa | fa_pure ether | fa_pure ether heavy_fa; \n\
heavy_fa : heavy; \n\
fa_pure : carbon carbon_db_separator db | carbon carbon_db_separator db db_hydroxyl_separator hydroxyl; \n\
ether : 'a' | 'p'; \n\
lcb : lcb_pure | lcb_pure heavy_lcb; \n\
heavy_lcb : heavy; \n\
lcb_pure : carbon carbon_db_separator db db_hydroxyl_separator hydroxyl | old_hydroxyl carbon carbon_db_separator db; \n\
carbon : number; \n\
db : db_count | db_count db_positions; \n\
db_count : number; \n\
db_positions : ROB db_position RCB; \n\
db_position : db_single_position | db_position db_position_separator db_position; \n\
db_single_position : db_position_number | db_position_number cistrans; \n\
db_position_number : number; \n\
cistrans : 'E' | 'Z'; \n\
 \n\
hydroxyl : number; \n\
old_hydroxyl : 'd' | 't'; \n\
number :  digit; \n\
digit : '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9' | digit digit; \n\
 \n\
 \n\
 \n\
 \n\
 \n\
heavy : '(+' isotopes  ')'; \n\
isotopes : isotopes isotopes | isotope; \n\
isotope : '[' isotope_number ']' isotope_element isotope_count | '[' isotope_number ']' isotope_element; \n\
isotope_number : number; \n\
isotope_element : element; \n\
isotope_count : number; \n\
element : 'C' | 'H' | 'O' | 'N' | 'P' | 'S'; \n\
 \n\
/* separators */ \n\
SPACE : ' '; \n\
COLON : ':'; \n\
SEMICOLON : ';'; \n\
DASH : '-'; \n\
UNDERSCORE : '_'; \n\
SLASH : '/'; \n\
BACKSLASH : '\\\\'; \n\
COMMA: ','; \n\
ROB: '('; \n\
RCB: ')'; \n\
FRAGMENT_SEPARATOR : ' - '; \n\
 \n\
sorted_fa_separator : SLASH | BACKSLASH; \n\
adduct_separator : SPACE; \n\
unsorted_fa_separator : DASH | UNDERSCORE; \n\
plasmalogen_separator : headgroup_separator | DASH; \n\
headgroup_separator : SPACE; \n\
carbon_db_separator : COLON; \n\
db_hydroxyl_separator : SEMICOLON; \n\
db_position_separator : COMMA; \n\
round_open_bracket : ROB; \n\
round_close_bracket : RCB; \n\
 \n\
character : 'a' | 'b' | 'c' | 'd' | 'e' | 'f' | 'g' | 'h' | 'i' | 'j' | 'k' | 'l' | 'm' | 'n' | 'o' | 'p' | 'q' | 'r' | 's' | 't' | 'u' | 'v' | 'w' | 'x' | 'y' | 'z' | 'A' | 'B' | 'C' | 'D' | 'E' | 'F' | 'G' | 'H' | 'I' | 'J' | 'K' | 'L' | 'M' | 'N' | 'O' | 'P' | 'Q' | 'R' | 'S' | 'T' | 'U' | 'V' | 'W' | 'X' | 'Y' | 'Z' |'0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9'; \n\
charge : '1' | '2' | '3' | '4'; \n\
charge_sign : '-' | '+'; \n\
 \n\
";



const string goslin_fragment_grammar = "/* \n\
 * MIT License \n\
 *  \n\
 * Copyright (c) 2017 Dominik Kopczynski   -   dominik.kopczynski {at} isas.de \n\
 *                    Bing Peng   -   bing.peng {at} isas.de \n\
 *                    Nils Hoffmann  -  nils.hoffmann {at} isas.de \n\
 * \n\
 * Permission is hereby granted, free of charge, to any person obtaining a copy \n\
 * of this software and associated documentation files (the 'Software'), to deal \n\
 * in the Software without restriction, including without limitation the rights \n\
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell \n\
 * copies of the Software, and to permit persons to whom the Software is \n\
 * furnished to do so, subject to the following conditions:; \n\
 *  \n\
 * The above copyright notice and this permission notice shall be included in all \n\
 * copies or substantial portions of the Software. \n\
 *  \n\
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR \n\
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, \n\
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE \n\
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER \n\
 * LIABILITY, WHether IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, \n\
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE \n\
 * SOFTWARE. \n\
*/ \n\
 \n\
/* This is a BNF / ANTLR4 grammar for lipid subspecies identifiers following \n\
 * J.K. Pauling et al. 2017, PLoS One, 12(11):e0188394. \n\
 */ \n\
 \n\
grammar GoslinFragments; \n\
 \n\
/* first rule is always start rule */ \n\
lipid : lipid_eof EOF; \n\
lipid_eof : just_lipid | just_lipid FRAGMENT_SEPARATOR fragment_name; \n\
just_lipid : lipid_pure | lipid_pure adduct_info; \n\
lipid_pure : gl | pl | sl | sterol | mediatorc | saccharolipid; \n\
 \n\
/* rules for fragments */ \n\
fragment_name : frag_char; \n\
frag_char : frag_char frag_char | character | sign; \n\
sign : '-' | '+' | ' ' | ',' | '(' | ')' | '[' | ']' | ':' | '*' | ';'; \n\
 \n\
/* adduct information */ \n\
adduct_info : '[M' adduct ']' charge charge_sign | adduct_separator '[M' adduct ']' charge charge_sign; \n\
adduct : '+H' | '+2H' | '+NH4' | '-H' | '-2H' | '+HCOO' | '+CH3COO' | charge_sign arbitrary_adduct; \n\
arbitrary_adduct : adduct4 | adduct4 adduct4; \n\
adduct4 : adduct2 | adduct2 adduct2; \n\
adduct2 : character | character character; \n\
 \n\
fa2 : fa2_unsorted | fa2_sorted; \n\
fa2_unsorted: fa DASH fa | fa UNDERSCORE fa; \n\
fa2_sorted: fa SLASH fa | fa BACKSLASH fa; \n\
fa3 : fa3_unsorted | fa3_sorted; \n\
fa3_unsorted: fa DASH fa DASH fa | fa UNDERSCORE fa UNDERSCORE fa; \n\
fa3_sorted: fa SLASH fa SLASH fa | fa BACKSLASH fa BACKSLASH fa; \n\
fa4 : fa4_unsorted | fa4_sorted; \n\
fa4_unsorted: fa DASH fa DASH fa DASH fa | fa UNDERSCORE fa UNDERSCORE fa UNDERSCORE fa; \n\
fa4_sorted: fa SLASH fa SLASH fa SLASH fa | fa BACKSLASH fa BACKSLASH fa BACKSLASH fa; \n\
 \n\
/* glycerolipid rules */ \n\
gl : sqml | mgl | dgl | sgl | tgl; \n\
sqml : hg_sqml_full headgroup_separator fa; \n\
mgl : hg_mgl_full headgroup_separator fa; \n\
dgl : hg_dgl_full headgroup_separator gl_species | hg_dgl_full headgroup_separator dgl_subspecies; \n\
sgl : hg_sgl_full headgroup_separator gl_species | hg_sgl_full headgroup_separator dgl_subspecies; \n\
tgl : hg_tgl_full headgroup_separator gl_species | hg_tgl_full headgroup_separator tgl_subspecies; \n\
gl_species : fa; \n\
dgl_subspecies : fa2; \n\
tgl_subspecies : fa3; \n\
 \n\
hg_sqml_full : hg_sqml | hg_mgl heavy_hg; \n\
hg_mgl_full : hg_mgl | hg_mgl heavy_hg; \n\
hg_dgl_full : hg_dgl | hg_dgl heavy_hg; \n\
hg_sgl_full : hg_sgl | hg_sgl heavy_hg; \n\
hg_tgl_full : hg_tgl | hg_tgl heavy_hg; \n\
 \n\
hg_sqml : 'SQMG'; \n\
hg_mgl : 'MAG'; \n\
hg_dgl : 'DAG'; \n\
hg_sgl : 'MGDG' | 'DGDG' | 'SQDG'; \n\
hg_tgl : 'TAG'; \n\
 \n\
 \n\
 \n\
/* phospholipid rules */ \n\
pl : lpl | dpl | tpl | pl_o | cl | mlcl | dlcl; \n\
pl_o : lpl_o | dpl_o; \n\
lpl : hg_lplc headgroup_separator fa; \n\
lpl_o : hg_lpl_oc plasmalogen_separator fa; \n\
dpl : hg_plc headgroup_separator pl_species | hg_plc headgroup_separator pl_subspecies; \n\
tpl : hg_tplc headgroup_separator pl_species | hg_tplc headgroup_separator tpl_subspecies; \n\
dpl_o : hg_pl_oc plasmalogen_separator pl_species | hg_pl_oc plasmalogen_separator pl_subspecies; \n\
dlcl : hg_dlclc headgroup_separator pl_species | hg_dlclc headgroup_separator dlcl_subspecies; \n\
mlcl : hg_mlclc headgroup_separator pl_species | hg_mlclc headgroup_separator mlcl_subspecies; \n\
cl : hg_clc headgroup_separator pl_species | hg_clc headgroup_separator cl_subspecies; \n\
 \n\
pl_species : fa; \n\
pl_subspecies : fa2; \n\
tpl_subspecies : fa3; \n\
dlcl_subspecies : fa2; \n\
mlcl_subspecies : fa3; \n\
cl_subspecies : fa4; \n\
 \n\
heavy_hg : heavy; \n\
 \n\
 \n\
hg_clc : hg_cl | hg_cl heavy_hg; \n\
hg_cl : 'CL'; \n\
hg_mlclc : hg_mlcl | hg_mlcl heavy_hg; \n\
hg_mlcl : 'MLCL'; \n\
hg_dlclc : hg_dlcl | hg_dlcl heavy_hg; \n\
hg_dlcl : 'DLCL'; \n\
hg_plc : hg_pl | hg_pl heavy_hg; \n\
hg_pl : 'BMP' | 'CDP-DAG' | 'DMPE' | 'MMPE' | 'PA' | 'PC' | 'PE' | 'PEt' | 'PG' | 'PI' | hg_pip | 'PS' | 'LBPA' | 'PGP' | 'PPA' | 'Glc-GP' | '6-Ac-Glc-GP' | hg_pim | 'PnC' | 'PnE' | 'PT' | 'PE-NMe2' | 'PE-NMe' | 'PIMIP' | 'CDPDAG'; \n\
hg_pim : 'PIM' hg_pim_number; \n\
hg_pim_number : number; \n\
hg_pip : hg_pip_pure | hg_pip_pure hg_pip_m | hg_pip_pure hg_pip_d | hg_pip_pure hg_pip_t; \n\
hg_pip_pure : 'PIP'; \n\
hg_pip_m : '[3\\']' | '[4\\']' | '[5\\']'; \n\
hg_pip_d : '2' | '2[3\\',4\\']' | '2[4\\',5\\']' | '2[3\\',5\\']'; \n\
hg_pip_t : '3' | '3[3\\',4\\',5\\']'; \n\
hg_tplc : hg_tpl | hg_tpl heavy_hg; \n\
hg_tpl : 'SLBPA' | 'NAPE'; \n\
hg_lplc : hg_lpl | hg_lpl heavy_hg; \n\
hg_lpl : 'LPA' | 'LPC' | 'LPE' | 'LPG' | 'LPI' | 'LPS' | hg_lpim | 'CPA' | 'LCDPDAG' | 'LDMPE' | 'LMMPE' | 'LPIMIP' | 'LPIN'; \n\
hg_lpim : 'LPIM' hg_lpim_number; \n\
hg_lpim_number : number; \n\
hg_lpl_oc : hg_lpl_o ' O' | hg_lpl_o heavy_hg ' O'; \n\
hg_lpl_o : 'LPC' | 'LPE'; \n\
hg_pl_oc : hg_pl_o ' O' | hg_pl_o heavy_hg ' O'; \n\
hg_pl_o : 'PA' | 'PC' | 'PE' | 'PG' | 'PI' | 'PS'; \n\
 \n\
 \n\
 \n\
/* sphingolipid rules */ \n\
sl : lsl | dsl; \n\
lsl : hg_lslc headgroup_separator lcb; \n\
dsl : hg_dslc headgroup_separator sl_species | hg_dslc headgroup_separator sl_subspecies; \n\
 \n\
sl_species : lcb; \n\
sl_subspecies : lcb sorted_fa_separator fa; \n\
 \n\
hg_lslc : hg_lsl | hg_lsl heavy_hg; \n\
hg_lsl : 'LCB' | 'LCBP' | 'LHexCer' | 'LSM'; \n\
hg_dslc : hg_dsl | hg_dsl heavy_hg; \n\
hg_dsl : 'Cer' | 'CerP' | 'EPC' | 'GB4' | 'GD3' | 'GB3' | 'GM3' | 'GM4' | 'Hex3Cer' | 'Hex2Cer' | 'HexCer' | 'IPC' | 'M(IP)2C' | 'MIPC' | 'SHexCer' | 'SM' | 'FMC-5' | 'FMC-6' ; \n\
 \n\
 \n\
 \n\
/* sterol lipids */ \n\
sterol : stc | ste | stes; \n\
stc : st | st heavy_hg; \n\
st : 'Ch' | 'Cholesterol' | 'ST 27:1;1' | 'ST 27:2;1' | 'ST 28:3;1' | 'ST 30:2;1' | 'ST 29:2;1' | 'ST 28:2;1'; \n\
ste : hg_stc sorted_fa_separator fa; \n\
stes : hg_stcs headgroup_separator fa; \n\
hg_stc : hg_ste | hg_ste heavy_hg; \n\
hg_ste : 'SE 27:1' | 'SE 27:2' | 'SE 28:3' | 'SE 30:2' | 'SE 29:2' | 'SE 28:2'; \n\
hg_stcs : hg_stes | hg_ste heavy_hg; \n\
hg_stes : 'ChE' | 'CE'; \n\
 \n\
 \n\
/* mediator lipids */ \n\
mediatorc : mediator | mediator heavy_hg; \n\
mediator : '10-HDoHE' | '11-HDoHE' | '11-HETE' | '11,12-DHET' | '11(12)-EET'| '12-HEPE' | '12-HETE' | '12-HHTrE' | '12-OxoETE' | '12(13)-EpOME' | '13-HODE' | '13-HOTrE' | '14,15-DHET' | '14(15)-EET' | '14(15)-EpETE' | '15-HEPE' | '15-HETE' | '15d-PGJ2' | '16-HDoHE' | '16-HETE' | '18-HEPE' | '5-HEPE' | '5-HETE' | '5-HpETE' | '5-OxoETE' | '5,12-DiHETE' | '5,6-DiHETE' | '5,6,15-LXA4' | '5(6)-EET' | '8-HDoHE' | '8-HETE' | '8,9-DHET' | '8(9)-EET' | '9-HEPE' | '9-HETE' | '9-HODE' | '9-HOTrE' | '9(10)-EpOME' | 'AA' | 'alpha-LA' | 'DHA' | 'EPA' | 'Linoleic acid' | 'LTB4' | 'LTC4' | 'LTD4' | 'Maresin 1' | 'Palmitic acid' | 'PGB2' | 'PGD2' | 'PGE2' | 'PGF2alpha' | 'PGI2' | 'Resolvin D1' | 'Resolvin D2' | 'Resolvin D3' | 'Resolvin D5' | 'tetranor-12-HETE' | 'TXB1' | 'TXB2' | 'TXB3'; \n\
 \n\
 \n\
 \n\
 \n\
/* saccharolipids rules */ \n\
saccharolipid : sac_di | sac_f; \n\
sac_di : hg_sac_di_c headgroup_separator sac_species | hg_sac_di_c headgroup_separator sac_di_subspecies; \n\
hg_sac_di_c : hg_sac_di | hg_sac_di heavy_hg; \n\
hg_sac_di : 'DAT' | 'AC2SGL'; \n\
sac_f : hg_sac_f_c headgroup_separator sac_species | hg_sac_f_c headgroup_separator sac_f_subspecies; \n\
hg_sac_f_c : hg_sac_f | hg_sac_f heavy_hg; \n\
hg_sac_f : 'PAT16' | 'PAT18'; \n\
 \n\
sac_species : fa; \n\
sac_di_subspecies : fa2; \n\
sac_f_subspecies : fa4; \n\
 \n\
 \n\
 \n\
/* generic rules */ \n\
fa : fa_pure | fa_pure heavy_fa | fa_pure ether | fa_pure ether heavy_fa; \n\
heavy_fa : heavy; \n\
fa_pure : carbon carbon_db_separator db | carbon carbon_db_separator db db_hydroxyl_separator hydroxyl; \n\
ether : 'a' | 'p'; \n\
lcb : lcb_pure | lcb_pure heavy_lcb; \n\
heavy_lcb : heavy; \n\
lcb_pure : carbon carbon_db_separator db db_hydroxyl_separator hydroxyl | old_hydroxyl carbon carbon_db_separator db; \n\
carbon : number; \n\
db : db_count | db_count db_positions; \n\
db_count : number; \n\
db_positions : ROB db_position RCB; \n\
db_position : db_single_position | db_position db_position_separator db_position; \n\
db_single_position : db_position_number | db_position_number cistrans; \n\
db_position_number : number; \n\
cistrans : 'E' | 'Z'; \n\
hydroxyl : number; \n\
old_hydroxyl : 'd' | 't'; \n\
number :  digit; \n\
digit : '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9' | digit digit; \n\
 \n\
 \n\
 \n\
 \n\
 \n\
heavy : '(+' isotopes  ')'; \n\
isotopes : isotopes isotopes | isotope; \n\
isotope : '[' isotope_number ']' isotope_element isotope_count | '[' isotope_number ']' isotope_element; \n\
isotope_number : number; \n\
isotope_element : element; \n\
isotope_count : number; \n\
element : 'C' | 'H' | 'O' | 'N' | 'P' | 'S'; \n\
 \n\
/* separators */ \n\
SPACE : ' '; \n\
COLON : ':'; \n\
SEMICOLON : ';'; \n\
DASH : '-'; \n\
UNDERSCORE : '_'; \n\
SLASH : '/'; \n\
BACKSLASH : '\\\\'; \n\
COMMA: ','; \n\
ROB: '('; \n\
RCB: ')'; \n\
FRAGMENT_SEPARATOR : ' - '; \n\
 \n\
sorted_fa_separator : SLASH | BACKSLASH; \n\
adduct_separator : SPACE; \n\
unsorted_fa_separator : DASH | UNDERSCORE; \n\
plasmalogen_separator : headgroup_separator | DASH; \n\
headgroup_separator : SPACE; \n\
carbon_db_separator : COLON; \n\
db_hydroxyl_separator : SEMICOLON; \n\
db_position_separator : COMMA; \n\
round_open_bracket : ROB; \n\
round_close_bracket : RCB; \n\
 \n\
character : 'a' | 'b' | 'c' | 'd' | 'e' | 'f' | 'g' | 'h' | 'i' | 'j' | 'k' | 'l' | 'm' | 'n' | 'o' | 'p' | 'q' | 'r' | 's' | 't' | 'u' | 'v' | 'w' | 'x' | 'y' | 'z' | 'A' | 'B' | 'C' | 'D' | 'E' | 'F' | 'G' | 'H' | 'I' | 'J' | 'K' | 'L' | 'M' | 'N' | 'O' | 'P' | 'Q' | 'R' | 'S' | 'T' | 'U' | 'V' | 'W' | 'X' | 'Y' | 'Z' |'0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9'; \n\
charge : '1' | '2' | '3' | '4'; \n\
charge_sign : '-' | '+'; \n\
";



const string lipid_maps_grammar = "//////////////////////////////////////////////////////////////////////////////// \n\
// MIT License \n\
//  \n\
// Copyright (c) 2017 Dominik Kopczynski   -   dominik.kopczynski {at} isas.de \n\
//                    Bing Peng   -   bing.peng {at} isas.de \n\
//                    Nils Hoffmann  -  nils.hoffmann {at} isas.de \n\
// \n\
// Permission is hereby granted, free of charge, to any person obtaining a copy \n\
// of this software and associated documentation files (the 'Software'), to deal \n\
// in the Software without restriction, including without limitation the rights \n\
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell \n\
// copies of the Software, and to permit persons to whom the Software is \n\
// furnished to do so, subject to the following conditions: \n\
//  \n\
// The above copyright notice and this permission notice shall be included in all \n\
// copies or substantial portions of the Software. \n\
//  \n\
// THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR \n\
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, \n\
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE \n\
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER \n\
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, \n\
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE \n\
// SOFTWARE. \n\
//////////////////////////////////////////////////////////////////////////////// \n\
 \n\
//// This is a BNF grammer for lipid subspecies identifiers followed by \n\
//// J.K. Pauling et al. 2017, PLoS One, 12(11):e0188394.  \n\
 \n\
grammar LipidMaps; \n\
 \n\
/* first rule is always start rule */ \n\
lipid: lipid_rule EOF; \n\
lipid_rule: lipid_mono | lipid_mono isotope; \n\
lipid_mono: lipid_pure | lipid_pure isoform; \n\
lipid_pure: pure_fa | gl | pl | sl | pk | sterol | mediator; \n\
isoform: square_open_bracket isoform_inner square_close_bracket; \n\
isoform_inner : 'rac' | 'iso' | 'iso' number | 'R'; \n\
isotope: SPACE round_open_bracket element number round_close_bracket | DASH round_open_bracket element number round_close_bracket | DASH element number; \n\
element: 'd'; \n\
 \n\
 \n\
/* pure fatty acid */ \n\
pure_fa: hg_fa pure_fa_species | fa_no_hg; \n\
fa_no_hg: fa; \n\
pure_fa_species: round_open_bracket fa round_close_bracket | fa | round_open_bracket fa2 round_close_bracket; \n\
hg_fa: 'FA' | 'WE' | 'CoA' | 'CAR' | 'FAHFA' | 'CoA'; \n\
 \n\
 \n\
fa2 : fa2_unsorted | fa2_sorted; \n\
fa2_unsorted: fa DASH fa | fa UNDERSCORE fa; \n\
fa2_sorted: fa SLASH fa | fa BACKSLASH fa; \n\
fa3 : fa3_unsorted | fa3_sorted; \n\
fa3_unsorted: fa DASH fa DASH fa | fa UNDERSCORE fa UNDERSCORE fa; \n\
fa3_sorted: fa SLASH fa SLASH fa | fa BACKSLASH fa BACKSLASH fa; \n\
fa4 : fa4_unsorted | fa4_sorted; \n\
fa4_unsorted: fa DASH fa DASH fa DASH fa | fa UNDERSCORE fa UNDERSCORE fa UNDERSCORE fa; \n\
fa4_sorted: fa SLASH fa SLASH fa SLASH fa | fa BACKSLASH fa BACKSLASH fa BACKSLASH fa; \n\
lcb_fa_sorted: lcb SLASH fa | lcb BACKSLASH fa; \n\
species_fa : fa; \n\
 \n\
/* glycerolipid rules */ \n\
gl: sgl | tgl; \n\
sgl: hg_sglc sgl_species | hg_sglc sgl_subspecies; \n\
sgl_species: round_open_bracket fa round_close_bracket | fa; \n\
sgl_subspecies: round_open_bracket fa2 round_close_bracket | fa2; \n\
tgl: hg_glc tgl_species | hg_glc tgl_subspecies; \n\
tgl_species: round_open_bracket fa round_close_bracket | fa; \n\
tgl_subspecies: round_open_bracket fa3 round_close_bracket | fa3; \n\
 \n\
hg_sglc: hg_sgl | hg_sgl headgroup_separator; \n\
hg_sgl: 'MGDG' | 'DGDG' | 'SQDG' | 'SQMG' | 'DG' | 'DGCC' | 'PE-GlcDG'; \n\
hg_glc: hg_gl | hg_gl headgroup_separator; \n\
hg_gl: 'MG' | 'DG' | 'TG'; \n\
 \n\
 \n\
 \n\
 \n\
/* phospholipid rules */ \n\
pl: lpl | dpl | cl | fourpl | threepl; \n\
lpl: hg_lplc round_open_bracket fa_lpl round_close_bracket | hg_lplc fa_lpl; \n\
fa_lpl: fa | fa2; \n\
dpl: hg_ddpl dpl_species | hg_ddpl dpl_subspecies; \n\
dpl_species: round_open_bracket fa round_close_bracket | fa; \n\
dpl_subspecies: round_open_bracket fa2 round_close_bracket | fa2; \n\
cl: hg_clc cl_species | hg_clc cl_subspecies; \n\
cl_species: round_open_bracket fa round_close_bracket | fa; \n\
cl_subspecies: round_open_bracket '1\\'-' square_open_bracket fa2 square_close_bracket ',3\\'-' square_open_bracket fa2 square_close_bracket round_close_bracket | hg_clc '1\\'-' square_open_bracket fa2 square_close_bracket ',3\\'-' square_open_bracket fa2 square_close_bracket; \n\
fourpl: hg_fourplc round_open_bracket fa4 round_close_bracket | hg_fourplc fa4 | hg_fourplc round_open_bracket species_fa round_close_bracket | hg_fourplc species_fa; \n\
threepl: hg_threeplc round_open_bracket fa3 round_close_bracket | hg_threeplc fa3 | hg_threeplc round_open_bracket species_fa round_close_bracket | hg_threeplc species_fa; \n\
 \n\
hg_ddpl: hg_dplc pip_position | hg_dplc; \n\
 \n\
hg_clc: hg_cl | hg_cl headgroup_separator; \n\
hg_cl: 'CL'; \n\
hg_dplc: hg_dpl | hg_dpl headgroup_separator; \n\
hg_dpl: 'LBPA' | 'CDP-DG' | 'DMPE' | 'MMPE' | 'PA' | 'PC' | 'PE' | 'PEt' | 'PG' | 'PI' | 'PIP' | 'PIP2' | 'PIP3' | 'PS' | 'PIM1' | 'PIM2' | 'PIM3' | 'PIM4' | 'PIM5' | 'PIM6' | 'Glc-DG' | 'PGP' | 'PE-NMe2' | 'AC2SGL' | 'DAT' | 'PE-NMe' | 'PT' | 'Glc-GP' | 'PPA' | 'PnC' | 'PnE' | '6-Ac-Glc-GP'; \n\
hg_lplc: hg_lpl | hg_lpl headgroup_separator; \n\
hg_lpl: 'LysoPC' | 'LPC' | 'LysoPE' | 'LPE' | 'LPI' | 'LPG' | 'LPS' | 'LPIM1' | 'LPIM2' | 'LPIM3' | 'LPIM4' | 'LPIM5' | 'LPIM6' | 'CPA' | 'LPA'; \n\
hg_fourplc: hg_fourpl | hg_fourpl headgroup_separator; \n\
hg_fourpl: 'PAT16' | 'PAT18'; \n\
pip_position: square_open_bracket pip_pos square_close_bracket; \n\
pip_pos: pip_pos COMMA pip_pos | number '\\''; \n\
hg_threeplc: hg_threepl | hg_threepl headgroup_separator; \n\
hg_threepl: 'SLBPA' | 'PS-NAc' | 'NAPE'; \n\
 \n\
 \n\
 \n\
/* sphingolipid rules */ \n\
sl: lsl | dsl | sphingoxine; \n\
lsl: hg_lslc round_open_bracket lcb round_close_bracket | hg_lslc lcb; \n\
dsl: hg_dslc dsl_species | hg_dslc dsl_subspecies; \n\
dsl_species: round_open_bracket lcb round_close_bracket | lcb; \n\
dsl_subspecies: round_open_bracket lcb_fa_sorted round_close_bracket | lcb_fa_sorted; \n\
 \n\
sphingoxine: sphingoxine_pure | sphingoxine_var; \n\
sphingoxine_pure: sphingosine_name | sphinganine_name; \n\
sphingoxine_var: ctype headgroup_separator sphingosine_name | ctype headgroup_separator sphinganine_name; \n\
sphingosine_name: 'Sphingosine' | 'So' | 'Sphingosine-1-phosphate'; \n\
sphinganine_name: 'Sphinganine' | 'Sa' | 'Sphinganine-1-phosphate'; \n\
ctype: 'C' number; \n\
 \n\
hg_dslc: hg_dsl | hg_dsl headgroup_separator; \n\
hg_dsl: 'Cer' | 'CerP' | 'EPC' | 'GB3' | 'GB4' | 'GD3' | 'GM3' | 'GM4' | 'Hex3Cer' | 'Hex2Cer' | 'HexCer' | 'IPC' | 'M(IP)2C' | 'MIPC' | 'SHexCer' | 'SulfoHexCer' | 'SM' | 'PE-Cer' | 'PI-Cer' | 'GlcCer' | 'FMC-5' | 'FMC-6' | 'LacCer' | 'GalCer' | 'C1P' | special_cer; \n\
special_cer : special_cer_prefix '-Cer'; \n\
special_cer_prefix : '1-O-' special_cer_prefix_1_O | '(3\\'-sulfo)Galbeta'; \n\
special_cer_prefix_1_O : 'myristoyl' | 'palmitoyl' | 'stearoyl' | 'eicosanoyl' | 'behenoyl' | 'lignoceroyl' | 'cerotoyl' | 'carboceroyl' | 'tricosanoyl'; \n\
 \n\
 \n\
hg_lslc: hg_lsl | hg_lsl headgroup_separator; \n\
hg_lsl: 'SPH' | 'Sph' | 'S1P' | 'HexSph' | 'SPC' | 'SPH-P' | 'LysoSM' | 'SIP'; \n\
 \n\
 \n\
 \n\
/* polyketides */ \n\
pk : pk_hg pk_fa; \n\
pk_hg : 'RESORCINOL' | 'ANACARD' | 'PHENOL' | 'CATECHOL'; \n\
pk_fa : round_open_bracket fa round_close_bracket; \n\
 \n\
 \n\
 \n\
/* sterol lipids */ \n\
sterol: chc | chec; \n\
chc: ch | ch headgroup_separator; \n\
ch: 'Cholesterol'; \n\
chec: che | che headgroup_separator | che_fa; \n\
che: fa headgroup_separator hg_che; \n\
che_fa: hg_che round_open_bracket fa round_close_bracket; \n\
hg_che: 'Cholesteryl ester' | 'Cholesterol ester' | 'CE'; \n\
 \n\
 \n\
/* mediator lipids */ \n\
mediator: mediator_var | mediator_const; \n\
mediator_var: mediator_prefix mediator_name_separator mediator_var_names | '(+/-)-' mediator_prefix mediator_name_separator mediator_var_names; \n\
mediator_prefix: mediator_numbers | mediator_prefix mediator_prefix; \n\
mediator_numbers: mediator_numbers mediator_separator mediator_numbers | mediator_number_pure | mediator_number_pure mediator_pos; \n\
mediator_number_pure: number | round_open_bracket number round_close_bracket; \n\
mediator_pos: 'R' | 'S'; \n\
mediator_var_names: mediator_var_name | mediator_oxo '-' mediator_var_name; \n\
mediator_var_name: 'HHTrE' | 'EpOME' | 'HODE' | 'HOTrE' | 'DHET' | 'EET' | 'EpETE' | 'HEPE' | 'HETE' | 'PGJ2' | 'HDoHE' | 'HpETE' | 'ETE' | 'DiHETE' | 'LXA4'; \n\
mediator_const: 'Arachidonic acid' | 'Arachidonic Acid' | 'alpha-LA' | 'DHA' | 'EPA' | 'Linoleic acid' | 'LTB4' | 'LTC4' | 'LTD4' | 'Maresin 1' | 'Palmitic acid' | 'PGB2' | 'PGD2' | 'PGE2' | 'PGF2alpha' | 'PGI2' | 'Resolvin D1' | 'Resolvin D2' | 'Resolvin D3' | 'Resolvin D5' | 'tetranor-12-HETE' | 'TXB1' | 'TXB2' | 'TXB3'; \n\
mediator_oxo: 'Oxo' | 'oxo'; \n\
 \n\
 \n\
 \n\
/* generic rules */ \n\
fa: fa_unmod | fa_unmod fa_mod | fa_unmod fa_mod_separator fa_mod; \n\
fa_unmod: round_open_bracket ether fa_pure round_close_bracket | round_open_bracket fa_pure round_close_bracket | ether fa_pure | fa_pure; \n\
fa_mod: round_open_bracket modification round_close_bracket; \n\
modification: modification ',' modification | number mod_text | mod_text; \n\
mod_text: mod_text mod_text | square_open_bracket mod_text square_close_bracket | 'OH' | 'Ke' | 'S' | 'OOH' | 'My' | 'Me' | 'R' | 'Br' | 'CHO' | 'COOH' | 'Cp' | '(R)' | 'Ep' | 'OH2' | 'OH3' | 'OH4' | 'OH6' | 'cyclo' | 'cyclo2' | 'KE2' | 'Ke2' | 'NH2' | 'Me6' | 'Me2' | 'Me3'  | 'Y'; \n\
ether: 'P-' | 'O-'; \n\
fa_pure: carbon carbon_db_separator db | carbon carbon_db_separator db db_hydroxyl_separator hydroxyl; \n\
lcb_fa: lcb_fa_unmod | lcb_fa_unmod lcb_fa_mod; \n\
lcb_fa_unmod: carbon carbon_db_separator db; \n\
lcb_fa_mod: round_open_bracket modification round_close_bracket; \n\
lcb: hydroxyl_lcb lcb_fa | lcb_fa; \n\
carbon: number; \n\
db : db_count | db_count db_positions; \n\
db_count : number; \n\
db_positions : ROB db_position RCB; \n\
db_position : db_single_position | db_position db_position_separator db_position; \n\
db_single_position : db_position_number | db_position_number cistrans; \n\
db_position_number : number; \n\
cistrans : 'E' | 'Z'; \n\
hydroxyl: number; \n\
hydroxyl_lcb: 'm' | 'd' | 't'; \n\
number:  '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9' | number number; \n\
 \n\
/* separators */ \n\
SPACE : ' '; \n\
COLON : ':'; \n\
SEMICOLON : ';'; \n\
DASH : '-'; \n\
UNDERSCORE : '_'; \n\
SLASH : '/'; \n\
BACKSLASH : '\\\\'; \n\
COMMA: ','; \n\
ROB: '('; \n\
RCB: ')'; \n\
SOB: '['; \n\
SCB: ']'; \n\
 \n\
fa_separator: UNDERSCORE | SLASH | BACKSLASH | DASH; \n\
headgroup_separator: SPACE; \n\
fa_mod_separator: SPACE; \n\
carbon_db_separator: COLON; \n\
db_hydroxyl_separator: SEMICOLON; \n\
db_position_separator: COMMA; \n\
mediator_separator: COMMA; \n\
mediator_name_separator: DASH; \n\
round_open_bracket: ROB; \n\
round_close_bracket: RCB; \n\
square_open_bracket: SOB; \n\
square_close_bracket: SCB; \n\
 \n\
";



const string swiss_lipids_grammar = "/* \n\
 * MIT License \n\
 *  \n\
 * Copyright (c) 2020 Dominik Kopczynski   -   dominik.kopczynski {at} isas.de \n\
 *                    Bing Peng   -   bing.peng {at} isas.de \n\
 *                    Nils Hoffmann  -  nils.hoffmann {at} isas.de \n\
 * \n\
 * Permission is hereby granted, free of charge, to any person obtaining a copy \n\
 * of this software and associated documentation files (the 'Software'), to deal \n\
 * in the Software without restriction, including without limitation the rights \n\
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell \n\
 * copies of the Software, and to permit persons to whom the Software is \n\
 * furnished to do so, subject to the following conditions:; \n\
 *  \n\
 * The above copyright notice and this permission notice shall be included in all \n\
 * copies or substantial portions of the Software. \n\
 *  \n\
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR \n\
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, \n\
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE \n\
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER \n\
 * LIABILITY, WHether IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, \n\
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE \n\
 * SOFTWARE. \n\
*/ \n\
 \n\
 \n\
 \n\
grammar SwissLipids; \n\
 \n\
 \n\
/* first rule is always start rule */ \n\
lipid : lipid_pure EOF; \n\
lipid_pure : fatty_acid | gl | pl | sl | st; \n\
 \n\
 \n\
 \n\
 \n\
/* fatty acyl rules */ \n\
fa : fa_core | fa_lcb_prefix fa_core | fa_core fa_lcb_suffix | fa_lcb_prefix fa_core fa_lcb_suffix; \n\
fa_core : carbon carbon_db_separator db | ether carbon carbon_db_separator db; \n\
 \n\
lcb : lcb_core | fa_lcb_prefix lcb_core | lcb_core fa_lcb_suffix | fa_lcb_prefix lcb_core fa_lcb_suffix; \n\
lcb_core : hydroxyl carbon carbon_db_separator db; \n\
 \n\
carbon : number; \n\
db : db_count | db_count db_positions; \n\
db_count : number; \n\
db_positions : ROB db_position RCB; \n\
db_position : db_single_position | db_position db_position_separator db_position; \n\
db_single_position : db_position_number | db_position_number cistrans; \n\
db_position_number : number; \n\
cistrans : 'E' | 'Z'; \n\
ether : 'O-' | 'P-'; \n\
hydroxyl : 'm' | 'd' | 't'; \n\
fa_lcb_suffix : fa_lcb_suffix_core | fa_lcb_suffix_separator fa_lcb_suffix_core | ROB fa_lcb_suffix_core RCB; \n\
fa_lcb_suffix_core : fa_lcb_suffix_number fa_lcb_suffix_type | fa_lcb_suffix_number fa_lcb_suffix_separator fa_lcb_suffix_type; \n\
fa_lcb_suffix_type : 'OH' | 'me'; \n\
fa_lcb_suffix_number : number; \n\
fa_lcb_prefix : fa_lcb_prefix_type | fa_lcb_prefix_type fa_lcb_prefix_separator; \n\
fa_lcb_prefix_type : 'iso'; \n\
 \n\
/* different fatty acyl types */ \n\
fa_species : fa; \n\
 \n\
fa2 : fa2_sorted | fa2_unsorted; \n\
fa2_sorted : fa sorted_fa_separator fa; \n\
fa2_unsorted : fa unsorted_fa_separator fa; \n\
 \n\
fa3 : fa3_sorted | fa3_unsorted; \n\
fa3_sorted : fa sorted_fa_separator fa sorted_fa_separator fa; \n\
fa3_unsorted : fa unsorted_fa_separator fa unsorted_fa_separator fa; \n\
 \n\
fa4 : fa4_sorted | fa4_unsorted; \n\
fa4_sorted : fa sorted_fa_separator fa sorted_fa_separator fa sorted_fa_separator fa; \n\
fa4_unsorted : fa unsorted_fa_separator fa unsorted_fa_separator fa unsorted_fa_separator fa; \n\
 \n\
 \n\
 \n\
 \n\
 \n\
/* fatty acid rules */ \n\
fatty_acid : fa_hg fa_fa | fa_hg headgroup_separator fa_fa | mediator; \n\
fa_hg : 'FA' | 'fatty acid' | 'fatty alcohol' | 'NAE' | 'GP-NAE'; \n\
fa_fa : ROB fa RCB; \n\
 \n\
 \n\
 \n\
 \n\
 \n\
 \n\
 \n\
/* mediator rules */ \n\
mediator : mediator_single | mediator_single headgroup_separator med_positions; \n\
mediator_single : mediator_single mediator_single | mediator_single '-' mediator_single | db_positions | med_positions | 'KETE' | 'keto' | 'oxo' | 'Hp' | 'EPE' | 'ETE' | 'ODE' | 'EPT' | 'H' | 'LXA4' | 'hydroxy' | 'Di' | 'RvE1' | 'glyceryl' | 'EpETrE' | 'DHE' | 'ETrE' | 'DHA' | 'DoHA' | 'LTB4' | 'PGE2' | ' PGE2' | 'PGF2alpha' | 'trihydroxy' | 'TriH' | 'OTrE' | 'dihydroxy' | 'Ep' | 'LXB4' | 'Tri' | 'PAHSA' | 'MUFA' | 'GPGP' | 'GPIP' | 'PGE1' | 'PGG2' | 'SFA' | 'PUFA' | 'PGI2' | 'TXA2' | 'CoA' | 'FAOH' | 'EA' | 'beta' | 'PGH2' | 'LTD4' | 'kete' | 'DPE' | ' PGD2' | 'PGD2' | 'PGF2' | 'LTC4' | 'LTA4' | 'PGD1' | 'PGA1' | 'LTDE4' | 'epoxyoleate' | 'epoxystearate' | 'carboxy' | 'PGH1' | 'EtrE' | 'HXA3' | 'HxA3' | 'HXA3-C'; \n\
 \n\
 \n\
 \n\
med_positions : med_position | ROB med_position RCB; \n\
med_position : med_position med_position_separator med_position | number med_suffix | number; \n\
med_suffix : 'S' | 'R'; \n\
 \n\
 \n\
 \n\
 \n\
 \n\
 \n\
 \n\
/* glycerolipid rules */ \n\
gl : gl_regular | gl_mono | gl_molecular; \n\
 \n\
gl_regular : gl_hg gl_fa | gl_hg headgroup_separator gl_fa; \n\
gl_fa : ROB fa_species RCB | ROB fa3 RCB; \n\
gl_hg : 'MG' | 'DG' | 'TG' |  'MAG' | 'DAG' | 'TAG'; \n\
 \n\
gl_molecular : gl_molecular_hg gl_molecular_fa | gl_molecular_hg headgroup_separator gl_molecular_fa; \n\
gl_molecular_fa : ROB fa2 RCB; \n\
gl_molecular_hg : 'DG' | 'DAG'; \n\
 \n\
 \n\
gl_mono : gl_mono_hg gl_mono_fa | gl_mono_hg headgroup_separator gl_mono_fa; \n\
gl_mono_fa : ROB fa_species RCB | ROB fa2 RCB; \n\
gl_mono_hg : 'MHDG' | 'DHDG' | 'MGDG' | 'DGDG'; \n\
 \n\
 \n\
 \n\
 \n\
 \n\
 \n\
 \n\
/* phospholipid rules */ \n\
pl : pl_regular | pl_three | pl_four; \n\
 \n\
pl_regular : pl_hg pl_fa | pl_hg headgroup_separator pl_fa; \n\
pl_fa : ROB fa_species RCB | ROB fa2 RCB; \n\
pl_hg : 'LPA' | 'LPC' | 'LPE' | 'LPG' | 'LPI' | 'LPS' | 'PA' | 'PC' | 'PE' | 'PG' | 'PI' | 'PS' | 'PGP' | 'PIP' | 'PIP[3]' | 'PIP[4]' | 'PIP[5]' | 'PIP2' | 'PIP2[3,4]' | 'PIP2[3,5]' | 'PIP2[4,5]' | 'PIP3' | 'PIP3[3,4,5]' | 'CDP-DAG'; \n\
 \n\
pl_three : pl_three_hg pl_three_fa | pl_three_hg headgroup_separator pl_three_fa; \n\
pl_three_fa : ROB fa_species RCB | ROB fa3 RCB; \n\
pl_three_hg : 'NAPE'; \n\
 \n\
pl_four : pl_four_hg pl_four_fa | pl_four_hg headgroup_separator pl_four_fa; \n\
pl_four_fa : ROB fa_species RCB | ROB fa2 RCB | ROB fa4 RCB; \n\
pl_four_hg : 'BMP' | 'LBPA' | 'Lysobisphosphatidate' | 'CL' | 'MLCL' | 'DLCL'; \n\
 \n\
 \n\
 \n\
 \n\
/* sphingolipid rules */ \n\
sl : sl_hg sl_lcb | sl_hg headgroup_separator sl_lcb; \n\
sl_hg : sl_hg_names | sl_hg_prefix sl_hg_names | sl_hg_names sl_hg_suffix | sl_hg_prefix sl_hg_names sl_hg_suffix; \n\
sl_hg_names : 'HexCer' | 'Hex2Cer' | 'SM' | 'PE-Cer' | 'Cer' | 'CerP' | 'Gb3' | 'GA2' | 'GA1' | 'GM3' | 'GM2' | 'GM1' | 'GD3' | 'GT3' | 'GD1' | 'GT1' | 'GQ1' | 'GM4' | 'GD2' | 'GT2' | 'GP1' | 'GD1a' | 'GM1b' | 'GT1b' | 'GQ1b' | 'GT1a' | 'GQ1c' | 'GP1c' | 'GD1c' | 'GD1b' | 'GT1c' | 'IPC' | 'MIPC' | 'M(IP)2C' | 'Gb3Cer' | 'Gb4Cer' | 'Forssman'  | 'MSGG' | 'DSGG' | 'NOR1' | 'NORint' | 'NOR2' | 'Globo-H' | 'Globo-A' | 'SB1a' | 'SM1b' | 'SM1a' | 'Branched-Forssman' | 'Globo-B' | 'Para-Forssman' | 'Globo-Lex-9'; \n\
 \n\
sl_hg_prefix : sl_hg_prefix '-' | sl_hg_prefix sl_hg_prefix | ROB sl_hg_prefix RCB | 'Glc' | 'NAc' | 'Gal' | 'Fuc' | 'SO3' | 'NeuGc' | 'i' | 'NeuAc' | 'Lac' | 'Lex' | '(3\\'-sulfo)' | 'Ac-O-9' | '(alpha2-8)' | '(alpha2-6)' | 'NeuAc' | 'Sulfo'; \n\
sl_hg_suffix : sl_hg_suffix sl_hg_suffix | sl_hg_suffix '/' | ROB sl_hg_suffix RCB | 'NeuAc' | 'NeuGc' | ' alpha'; \n\
 \n\
sl_lcb : sl_lcb_species | sl_lcb_subspecies; \n\
sl_lcb_species : ROB lcb RCB; \n\
sl_lcb_subspecies : ROB lcb sorted_fa_separator fa RCB; \n\
 \n\
 \n\
 \n\
 \n\
/* sterol rules */ \n\
st : st_species | st_sub1 | st_sub2; \n\
 \n\
st_species : st_species_hg st_species_fa | st_species_hg headgroup_separator st_species_fa; \n\
st_species_hg : 'SE'; \n\
st_species_fa : ROB fa_species RCB; \n\
 \n\
st_sub1 : st_sub1_hg st_sub1_fa | st_sub1_hg headgroup_separator st_sub1_fa; \n\
st_sub1_hg : 'CE'; \n\
st_sub1_fa : ROB fa RCB; \n\
 \n\
st_sub2 : st_sub2_hg sorted_fa_separator fa RCB; \n\
st_sub2_hg : 'SE' ROB number COLON number; \n\
 \n\
 \n\
 \n\
 \n\
 \n\
 \n\
/* separators */ \n\
SPACE : ' '; \n\
COLON : ':'; \n\
SEMICOLON : ';'; \n\
DASH : '-'; \n\
UNDERSCORE : '_'; \n\
SLASH : '/'; \n\
BACKSLASH : '\\\\'; \n\
COMMA: ','; \n\
ROB: '('; \n\
RCB: ')'; \n\
 \n\
unsorted_fa_separator : UNDERSCORE; \n\
sorted_fa_separator : SLASH; \n\
headgroup_separator : SPACE; \n\
carbon_db_separator : COLON; \n\
db_position_separator : COMMA; \n\
med_position_separator : COMMA; \n\
fa_lcb_suffix_separator : DASH; \n\
fa_lcb_prefix_separator : DASH; \n\
 \n\
number :  digit; \n\
digit : '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9' | digit digit; \n\
";



const string hmdb_grammar = "/* \n\
 * MIT License \n\
 *  \n\
 * Copyright (c) 2020 Dominik Kopczynski   -   dominik.kopczynski {at} isas.de \n\
 *                    Bing Peng   -   bing.peng {at} isas.de \n\
 *                    Nils Hoffmann  -  nils.hoffmann {at} isas.de \n\
 * \n\
 * Permission is hereby granted, free of charge, to any person obtaining a copy \n\
 * of this software and associated documentation files (the 'Software'), to deal \n\
 * in the Software without restriction, including without limitation the rights \n\
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell \n\
 * copies of the Software, and to permit persons to whom the Software is \n\
 * furnished to do so, subject to the following conditions:; \n\
 *  \n\
 * The above copyright notice and this permission notice shall be included in all \n\
 * copies or substantial portions of the Software. \n\
 *  \n\
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR \n\
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, \n\
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE \n\
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER \n\
 * LIABILITY, WHether IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, \n\
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE \n\
 * SOFTWARE. \n\
*/ \n\
 \n\
 \n\
 \n\
grammar HMDB; \n\
 \n\
 \n\
/* first rule is always start rule */ \n\
lipid : lipid_pure EOF; \n\
lipid_pure : lipid_class | lipid_class lipid_suffix; \n\
lipid_class : fatty_acid | gl | pl | sl | st; \n\
 \n\
lipid_suffix : '[rac]'; \n\
 \n\
 \n\
 \n\
/* fatty acyl rules */ \n\
fa : fa_core | furan_fa | fa_lcb_prefix fa_core | fa_core fa_lcb_suffix | fa_lcb_prefix fa_core fa_lcb_suffix; \n\
fa_core : carbon carbon_db_separator db | ether carbon carbon_db_separator db; \n\
 \n\
furan_fa : furan_fa_mono | furan_fa_di; \n\
furan_fa_mono : number 'M' number | 'MonoMe(' number ',' number ')'; \n\
furan_fa_di : number 'D' number | 'DiMe(' number ',' number ')'; \n\
 \n\
lcb : lcb_core | fa_lcb_prefix lcb_core | lcb_core fa_lcb_suffix | fa_lcb_prefix lcb_core fa_lcb_suffix; \n\
lcb_core : hydroxyl carbon carbon_db_separator db; \n\
 \n\
carbon : number; \n\
db : db_count | db_count db_positions | db_count db_suffix | db_count db_positions db_suffix; \n\
db_count : number; \n\
db_suffix : 'e' | 'n' db_suffix_number; \n\
db_suffix_number : number; \n\
db_positions : ROB db_position RCB; \n\
db_position : db_single_position | db_position db_position_separator db_position; \n\
db_single_position : db_position_number | db_position_number cistrans; \n\
db_position_number : number; \n\
cistrans : 'E' | 'Z'; \n\
ether : ether_type | ether_link_pos ether_type; \n\
ether_link_pos : number '-'; \n\
ether_type : 'o-' | 'O-' | 'P-' | 'i-' | 'a-'; \n\
hydroxyl : 'm' | 'd' | 't'; \n\
fa_lcb_suffix : fa_lcb_suffix_core | fa_lcb_suffix_separator fa_lcb_suffix_core | ROB fa_lcb_suffix_core RCB; \n\
fa_lcb_suffix_core : fa_lcb_suffix_type | fa_lcb_suffix_number fa_lcb_suffix_type | fa_lcb_suffix_number fa_lcb_suffix_separator fa_lcb_suffix_type; \n\
fa_lcb_suffix_type : 'OH' | 'me'; \n\
fa_lcb_suffix_number : number; \n\
fa_lcb_prefix : fa_lcb_prefix_type | fa_lcb_prefix_type fa_lcb_prefix_separator; \n\
fa_lcb_prefix_type : 'iso'; \n\
 \n\
/* different fatty acyl types */ \n\
fa_species : fa; \n\
 \n\
fa2 : fa2_sorted | fa2_unsorted; \n\
fa2_sorted : fa sorted_fa_separator fa; \n\
fa2_unsorted : fa unsorted_fa_separator fa; \n\
 \n\
fa3 : fa3_sorted | fa3_unsorted; \n\
fa3_sorted : fa sorted_fa_separator fa sorted_fa_separator fa; \n\
fa3_unsorted : fa unsorted_fa_separator fa unsorted_fa_separator fa; \n\
 \n\
fa4 : fa4_sorted | fa4_unsorted; \n\
fa4_sorted : fa sorted_fa_separator fa sorted_fa_separator fa sorted_fa_separator fa; \n\
fa4_unsorted : fa unsorted_fa_separator fa unsorted_fa_separator fa unsorted_fa_separator fa; \n\
 \n\
 \n\
 \n\
 \n\
 \n\
/* fatty acid rules */ \n\
fatty_acid : fa_hg fa_fa | fa_hg interlink_fa | fa_hg headgroup_separator fa_fa | mediator; \n\
fa_hg : 'FA' | 'fatty acid' | 'fatty alcohol' | 'NAE' | 'GP-NAE' | 'FAHFA'; \n\
fa_fa : ROB fa RCB; \n\
interlink_fa : ROB fa sorted_fa_separator fa RCB; \n\
 \n\
 \n\
 \n\
 \n\
 \n\
/* mediator rules */ \n\
mediator : mediator_single | mediator_single headgroup_separator med_positions; \n\
mediator_single : mediator_single mediator_single | mediator_single '-' mediator_single | db_positions | med_positions | 'KETE' | 'keto' | 'oxo' | 'Hp' | 'EPE' | 'ETE' | 'ODE' | 'EPT' | 'H' | 'LXA4' | 'hydroxy' | 'Di' | 'RvE1' | 'glyceryl' | 'EpETrE' | 'DHE' | 'ETrE' | 'DHA' | 'DoHA' | 'LTB4' | 'PGE2' | ' PGE2' | 'PGF2alpha' | 'trihydroxy' | 'TriH' | 'OTrE' | 'dihydroxy' | 'Ep' | 'LXB4' | 'Tri' | 'PAHSA' | 'MUFA' | 'GPGP' | 'GPIP' | 'PGE1' | 'PGG2' | 'SFA' | 'PUFA' | 'PGI2' | 'TXA2' | 'CoA' | 'FAOH' | 'EA' | 'beta' | 'PGH2' | 'LTD4' | 'kete' | 'DPE' | ' PGD2' | 'PGD2' | 'PGF2' | 'LTC4' | 'LTA4' | 'PGD1' | 'PGA1' | 'LTDE4' | 'epoxyoleate' | 'epoxystearate' | 'carboxy' | 'PGH1' | 'EtrE' | 'HXA3' | 'HxA3' | 'HXA3-C'; \n\
 \n\
 \n\
 \n\
med_positions : med_position | ROB med_position RCB; \n\
med_position : med_position med_position_separator med_position | number med_suffix | number; \n\
med_suffix : 'S' | 'R'; \n\
 \n\
 \n\
 \n\
 \n\
 \n\
 \n\
 \n\
/* glycerolipid rules */ \n\
gl : gl_regular | gl_mono | gl_molecular; \n\
 \n\
gl_regular : gl_hg gl_fa | gl_hg headgroup_separator gl_fa; \n\
gl_fa : ROB fa_species RCB | ROB fa3 RCB; \n\
gl_hg : 'MG' | 'DG' | 'TG' |  'MAG' | 'DAG' | 'TAG'; \n\
 \n\
gl_molecular : gl_molecular_hg gl_molecular_fa | gl_molecular_hg headgroup_separator gl_molecular_fa; \n\
gl_molecular_fa : ROB fa2 RCB; \n\
gl_molecular_hg : 'DG' | 'DAG'; \n\
 \n\
 \n\
gl_mono : gl_mono_hg gl_mono_fa | gl_mono_hg headgroup_separator gl_mono_fa; \n\
gl_mono_fa : ROB fa_species RCB | ROB fa2 RCB; \n\
gl_mono_hg : 'MHDG' | 'DHDG' | 'MGDG' | 'DGDG'; \n\
 \n\
 \n\
 \n\
 \n\
 \n\
 \n\
 \n\
/* phospholipid rules */ \n\
pl : pl_regular | pl_three | pl_four; \n\
 \n\
pl_regular : pl_hg pl_fa | pl_hg headgroup_separator pl_fa; \n\
pl_fa : ROB fa_species RCB | ROB fa2 RCB; \n\
pl_hg : 'LPA' | 'LPC' | 'LPE' | 'LPG' | 'LPI' | 'LPS' | 'PA' | 'PC' | 'PE' | 'PG' | 'PI' | 'PS' | 'PGP' | 'PIP' | 'PIP[3]' | 'PIP[4]' | 'PIP[5]' | 'PIP2' | 'PIP2[3,4]' | 'PIP2[3,5]' | 'PIP2[4,5]' | 'PIP3' | 'PIP3[3,4,5]' | 'CDP-DAG' | 'LysoPA' | 'LysoPC' | 'LysoPE' | 'LysoPG' | 'LysoPI' | 'LysoPS' | 'PE-NMe' | 'PE-NMe2' | 'CDP-DG'; \n\
 \n\
pl_three : pl_three_hg pl_three_fa | pl_three_hg headgroup_separator pl_three_fa; \n\
pl_three_fa : ROB fa_species RCB | ROB fa3 RCB; \n\
pl_three_hg : 'NAPE'; \n\
 \n\
pl_four : pl_four_hg pl_four_fa | pl_four_hg headgroup_separator pl_four_fa; \n\
pl_four_fa : ROB fa_species RCB | ROB fa2 RCB | ROB fa4 RCB; \n\
pl_four_hg : 'BMP' | 'LBPA' | 'Lysobisphosphatidate' | 'CL' | 'MLCL' | 'DLCL'; \n\
 \n\
 \n\
 \n\
 \n\
/* sphingolipid rules */ \n\
sl : sl_hg sl_lcb | sl_hg headgroup_separator sl_lcb; \n\
sl_hg : sl_hg_names | sl_hg_prefix sl_hg_names | sl_hg_names sl_hg_suffix | sl_hg_prefix sl_hg_names sl_hg_suffix; \n\
sl_hg_names : 'HexCer' | 'Hex2Cer' | 'SM' | 'PE-Cer' | 'Cer' | 'CerP' | 'IPC' | 'MIPC' | 'M(IP)2C' | 'Gb3Cer' | 'Gb4Cer' | 'Forssman'  | 'MSGG' | 'DSGG' | 'NOR1' | 'NORint' | 'NOR2' | 'Globo-H' | 'Globo-A' | 'SB1a' | 'SM1b' | 'SM1a' | 'Branched-Forssman' | 'Globo-B' | 'Para-Forssman' | 'Globo-Lex-9' | 'LysoSM' | 'Glucosylceramide' | 'Ceramide' | 'Tetrahexosylceramide' | ganglioside; \n\
ganglioside : 'Ganglioside' headgroup_separator ganglioside_names | ganglioside_names; \n\
ganglioside_names : 'Gb3' | 'GA2' | 'GA1' | 'GM3' | 'GM2' | 'GM1' | 'GD3' | 'GT3' | 'GD1' | 'GT1' | 'GQ1' | 'GM4' | 'GD2' | 'GT2' | 'GP1' | 'GD1a' | 'GM1b' | 'GT1b' | 'GQ1b' | 'GT1a' | 'GQ1c' | 'GP1c' | 'GD1c' | 'GD1b' | 'GT1c'; \n\
 \n\
sl_hg_prefix : sl_hg_prefix '-' | sl_hg_prefix sl_hg_prefix | ROB sl_hg_prefix RCB | 'Glc' | 'NAc' | 'Gal' | 'Fuc' | 'SO3' | 'NeuGc' | 'i' | 'NeuAc' | 'Lac' | 'Lex' | '(3\\'-sulfo)' | 'Ac-O-9' | '(alpha2-8)' | '(alpha2-6)' | 'NeuAc' | 'Sulfo'; \n\
sl_hg_suffix : sl_hg_suffix sl_hg_suffix | sl_hg_suffix '/' | ROB sl_hg_suffix RCB | 'NeuAc' | 'NeuGc' | ' alpha'; \n\
 \n\
sl_lcb : sl_lcb_species | sl_lcb_subspecies; \n\
sl_lcb_species : ROB lcb RCB; \n\
sl_lcb_subspecies : ROB lcb sorted_fa_separator fa RCB; \n\
 \n\
 \n\
 \n\
 \n\
/* sterol rules */ \n\
st : st_species | st_sub1 | st_sub2; \n\
 \n\
st_species : st_species_hg st_species_fa | st_species_hg headgroup_separator st_species_fa; \n\
st_species_hg : 'SE'; \n\
st_species_fa : ROB fa_species RCB; \n\
 \n\
st_sub1 : st_sub1_hg st_sub1_fa | st_sub1_hg headgroup_separator st_sub1_fa; \n\
st_sub1_hg : 'CE'; \n\
st_sub1_fa : ROB fa RCB; \n\
 \n\
st_sub2 : st_sub2_hg st_sub2_fa | st_sub2_hg headgroup_separator st_sub2_fa; \n\
st_sub2_hg : 'SE'; \n\
st_sub2_fa : ROB fa2 RCB; \n\
 \n\
 \n\
 \n\
 \n\
 \n\
 \n\
/* separators */ \n\
SPACE : ' '; \n\
COLON : ':'; \n\
SEMICOLON : ';'; \n\
DASH : '-'; \n\
UNDERSCORE : '_'; \n\
SLASH : '/'; \n\
BACKSLASH : '\\\\'; \n\
COMMA: ','; \n\
ROB: '('; \n\
RCB: ')'; \n\
 \n\
unsorted_fa_separator : UNDERSCORE; \n\
sorted_fa_separator : SLASH; \n\
headgroup_separator : SPACE; \n\
carbon_db_separator : COLON; \n\
db_position_separator : COMMA; \n\
med_position_separator : COMMA; \n\
fa_lcb_suffix_separator : DASH; \n\
fa_lcb_prefix_separator : DASH; \n\
 \n\
number :  digit; \n\
digit : '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9' | digit digit; \n\
";



const string sum_formula_grammar = "//////////////////////////////////////////////////////////////////////////////// \n\
// MIT License \n\
//  \n\
// Copyright (c) 2017 Dominik Kopczynski   -   dominik.kopczynski {at} isas.de \n\
//                    Bing Peng   -   bing.peng {at} isas.de \n\
//                    Nils Hoffmann  -  nils.hoffmann {at} isas.de \n\
// \n\
// Permission is hereby granted, free of charge, to any person obtaining a copy \n\
// of this software and associated documentation files (the \"Software\"), to deal \n\
// in the Software without restriction, including without limitation the rights \n\
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell \n\
// copies of the Software, and to permit persons to whom the Software is \n\
// furnished to do so, subject to the following conditions: \n\
//  \n\
// The above copyright notice and this permission notice shall be included in all \n\
// copies or substantial portions of the Software. \n\
//  \n\
// THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR \n\
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, \n\
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE \n\
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER \n\
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, \n\
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE \n\
// SOFTWARE. \n\
//////////////////////////////////////////////////////////////////////////////// \n\
 \n\
//// This is a BNF grammer for lipid subspecies identifiers followed by \n\
//// J.K. Pauling et al. 2017, PLoS One, 12(11):e0188394.  \n\
 \n\
grammar SumFormula; \n\
 \n\
/* first rule is always start rule */ \n\
molecule: molecule_rule EOF; \n\
molecule_rule: molecule_group; \n\
molecule_group: element_group | single_element | molecule_group molecule_group; \n\
element_group: element count; \n\
single_element: element; \n\
element: 'C' | 'H' | 'N' | 'O' | 'P' | 'S'; \n\
count: digit; \n\
digit:  '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9' | digit digit; \n\
 \n\
";



#endif /* KNOWN_GRAMMARS_H */
