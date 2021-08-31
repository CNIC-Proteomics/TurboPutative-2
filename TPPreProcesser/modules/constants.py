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

# Maximum accepted number of rows and columns
MAX_ROWS = 50000
MAX_COLS = 100

# Possible names in different columns (all in lower case)
COLUMN_NAMES = {
    "name": ["name"], 
    "mass": ["experimental mass", "apex m/z", "mz", "m/z"],
    "rt": ["rt [min]", "rt[s]", "rt[sec]", "rt", "retention time"],
    "tags": ["food", "drug", "microbial", "nalogenated", "peptide", "plant", "naturalproduct"],
    "chemical_formula": ["chemical Formula", "formula"],
    "inchi_key": ["inchikey"],
    "adduct": ["adduct"],
    "mzError": ["mz error (ppm)", "mzerror"]
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

# From parameter to name of column in tags
TAGS_DICT = {
    "food": "Food",
    "drug": "Drug",
    "natural_product": "NaturalProduct",
    "microbial_compound": "Microbial",
    "halogenated": "Halogenated",
    "peptide": "Peptide",
    "plant": "Plant"
}

# Default regex used by Tagger (when configUser is empty for these parameters)
DEFAULT_HALOGENATED_REGEX = "([Ff]luor(?!ene)|[Cc]hlor(?!ophyl)|[Bb]rom|[Ii]od)"
DEFAULT_PEPTIDE_REGEX = "^(Ala|Arg|Asn|Asp|Cys|Gln|Glu|Gly|His|Ile|Leu|Lys|Met|Phe|Pro|Ser|Thr|Trp|Tyr|Val|[-\s,]){3,}$"

# Default REname (when configUser is empty)
DEFAULT_REMOVE_ROW_REGEX = "No compounds found for experimental mass"
DEFAULT_AA_SEPARATOR = "\s"

# Default TableMerger
DEFAULT_N_DIGITS = "4"