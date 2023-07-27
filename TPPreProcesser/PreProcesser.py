#!/usr/bin/env python

# -*- coding: utf-8 -*-


# Module metadata variables
__author__ = "Rafael Barrero Rodriguez"
__credits__ = ["Rafael Barrero Rodriguez", "Jose Rodriguez", "Jesus Vazquez"]
__license__ = "Creative Commons Attribution-NonCommercial-NoDerivs 4.0 Unported License https://creativecommons.org/licenses/by-nc-nd/4.0/"
__version__ = "0.0.1"
__maintainer__ = "Jose Rodriguez"
__email__ = "rbarreror@cnic.es;jmrodriguezc@cnic.es"
__status__ = "Development"


# Import modules
import sys
import os
import argparse
import logging
import numpy as np
import pandas as pd
from configparser import ConfigParser

# Import project modules
import modules.TPExceptions as TPExc
from modules.TableHandler import MSTable, TMTable
from modules.INIHandler import ModuleInfo, InputINI
from modules.TableTester import MSTableTester
import modules.constants as constants


# Main function
def main(args):
    """
    Execute main function
    """

    # Define full path to infile from arguments
    infileFullPath = os.path.join(args.workdir, args.infile)

    # create table object
    msTable = MSTable(infileFullPath)

    # read table
    msTable.readTable()
    msTable.table[constants.TPIDX] = msTable.table.index

    # test input table
    msTableTester = MSTableTester(args)

    # test column names
    msTableTester.testColumns(msTable.tableColumnsLow)

    # start configuring ini file read by cpp scripts
    moduleInfo = ModuleInfo(args.workflow, args.workdir)
    moduleInfo.addModules()
    moduleInfo.addColumnNamesMSTable(msTable.table.columns)

    # apply regular expressions to debug and clean the table

    # Remove CMM annotation with certain identifier (yielding error)
    msTable.table = msTable.table[~np.isin(msTable.table['Identifier'], [188282])]

    # Remove fields with more than one compound
    colNameCompounds = moduleInfo.getColumnNameFromType(msTable.table.columns, "name")
    msTable.table[colNameCompounds] = msTable.table[colNameCompounds].str.replace(r"(;(\n|\s)|\s/\s\n?|\n).*$", "", regex=True)

        # Replace \n by \s
    msTable.removeLineFall()

        # Replace strange characters (±, α, β, γ, δ, ω)
    msTable.replaceChars()

        # strip compound names
    msTable.table[colNameCompounds] = msTable.table[colNameCompounds].str.strip()

    # write table
    msTable.writeTable(args.workdir, constants.OUTNAME)
    msTable.writeHTMLTable()

    # extract name of the compounds to be added to TPMapTable
    #msTable.extractCompoundNames(args.workdir)

    # process TableMerger table with additional information
    if ('4' in args.workflow) and (args.tmfile):
        tmTable = TMTable(os.path.join(args.workdir, args.tmfile))
        tmTable.readTable()

        # If column names of TMTable are repeated in MSTable, change them
        tmTable.colRename(msTable.table.columns)

        # Add name of columns in TMTable to INI
        moduleInfo.addColumnNamesTMTable(tmTable.table.columns)

        # Apply regular expressions to TMTable to clean it
        tmTable.removeLineFall()

        tmTable.writeTable(args.workdir, constants.OUTNAME_TMTABLE)
        tmTable.writeHTMLTable()

    # read user ini file and transfer parts of its content to c++ ini file
    userINI = InputINI(args)
    moduleInfo = userINI.transferToModuleInfo(moduleInfo, msTable.table.columns.to_list())

    # Add TPMetrics columns and check for possible errors
    if '5' in args.workflow:
        moduleInfo.addTPMetricsColumns(
            msTable.table.columns.to_list() if '4' not in args.workflow else tmTable.table.columns.to_list() + msTable.table.columns.to_list()
        )

    # Write info file
    moduleInfo.writeINI(args.workdir)

    # return input MS table: It will be used by TPFilter to get information of conserved properties (avoid re-reading the file)
    outcols = moduleInfo.iniDict['RowMerger']["conserved_columns"].split(',') if "RowMerger" in moduleInfo.iniDict.keys() else []
    return msTable.table.loc[:, set([constants.TPIDX, *outcols])]


if __name__ == "__main__":

    # parse arguments
    parser = argparse.ArgumentParser(
        description='PreProcesser',
        epilog=r"""
        Example:
            python TPPreProcesser\PreProcesser.py -wd C:\Users\Rafael\CNIC\Metabolomica\TurboPutative-2.0\test\test13 -wf 1234 -i MS_experiment.xls -tm FeatureInfo.xlsx
        """
    )

    parser.add_argument('-wd', '--workdir', help="Path to working directory", type=str, required=True)

    parser.add_argument('-wf', '--workflow', help="Modules executed: Tagger(1), REname(2), RowMerger(3), TableMerger(4)",
                         type=str, required=True)

    parser.add_argument('-i', '--infile', help="Name of input file", type=str, required=True)

    parser.add_argument('-tm', '--tmfile', help="Name of input file with additional information in TableMerger", type=str, required=False)

    args = parser.parse_args()

    # logging information
    log_file = os.path.join(args.workdir, os.path.splitext(args.infile)[0] + '_PreProcesser.log')
    logging.basicConfig(level=logging.INFO,
                        format='%(asctime)s - %(levelname)s - %(message)s',
                        datefmt='%m/%d/%Y %I:%M:%S %p',
                        handlers=[logging.FileHandler(log_file),
                                    logging.StreamHandler()])

    # start main function
    logging.info('start script: '+"{0}".format(" ".join([x for x in sys.argv])))
    main(args, logging)
    logging.info('end script')