#!/usr/bin/env python

# -*- coding: utf-8 -*-

import os

# Descritption: Set constant values used by PreProcesser

# Core ratio used in the execution
CORES_RATIO = 0.5

# Basename of output file
OUTNAME = "preProcessedTable.tsv"

# Basename of TableMerger table with additional information
OUTNAME_TMTABLE = "TM_table.tsv"

# Format accepted by TurboPutative
ACCEPTED_FORMAT = [".xls", ".xlsx", ".tsv"]

# Possible names in different columns (all in lower case)
COLUMN_NAMES = {
    "name": ["name"], 
    "mass": ["experimental mass", "apex m/z", "mz", "m/z"],
    "rt": ["rt [min]", "rt[s]", "rt[sec]", "rt", "retention time"],
    "tags": ["Food", "Drug", "Microbial", "Halogenated", "Peptide", "Plant", "NaturalProduct"],
    "chemical_formula": ["chemical Formula", "formula"],
    "inchi_key": ["Inchikey"]
    }

# Column required to input MS table (used to find header)
REQUIRED_COLUMN = [COLUMN_NAMES["name"]]

# Column required to input TM table (used to find header)
REQUIRED_COLUMN_TMTABLE = [COLUMN_NAMES["mass"]]

# Maximum row at which header is found (based-index: 0)
MAX_ROW_HEADER = 1

# Name of ini file with user information
INPUT_INI_FILENAME ="configUser.ini"

# Name of ini file with information for each C++ module
INFO_FILENAME = "configFile.ini"

# List with possible tags
TAGS = ["food", "drug", "natural_product", "microbial_compound", "halogenated", "peptide", "plant"]