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
import os
import sys
import logging
import argparse
import zipfile

# Import TP modules
from modules.ResultWriter import ResultWriter
from modules.ExtensionMover import ExtensionMover
#import modules.constants as constants

# constants
numToMod = {
    '1': "Tagger",
    '2': "REname",
    '3': "RowMerger",
    '4': "TableMerger"
}

# Main function
def main(args, logging):
    """
    Main function
    """

    #
    # Write final tables
    #

    # --> Create ResultWriter object
    resultWriter = ResultWriter(args.workdir, args.infile, logging)

    # --> Add input tables tables
    resultWriter.addTable(args.infile, False) # add infile
    
    if "4" in args.workflow: resultWriter.addTable(args.tmfile, False) # add tmTable if module 4

    # --> Add and write output tables
    for i, mod in enumerate(args.workflow):
        fileName = f"{str(i+1)}_{numToMod[mod]}.tsv"
        resultWriter.addTable(fileName, True)


    # --> Save xlsx file with all tables
    logging.info("Generating combined table")
    #resultWriter.saveCombinedTable()

    logging.info("Final tables were generated")


    #
    # Zip results
    #
    resultWriter.zipResults()


if __name__ == "__main__":

    # parse arguments
    parser = argparse.ArgumentParser(
        description='PostProcesser',
        epilog=r"""
        Example:
            python TPPostProcesser\PostProcesser.py -wd C:\Users\Rafael\CNIC\Metabolomica\TurboPutative-2.0\test\test13 -wf 1234 -i MS_experiment.xls -tm FeatureInfo.xlsx
        """
    )

    parser.add_argument('-wd', '--workdir', help="Path to working directory", type=str, required=True)

    parser.add_argument('-wf', '--workflow', help="Modules executed: Tagger(1), REname(2), RowMerger(3), TableMerger(4)",
                         type=str, required=True)

    parser.add_argument('-i', '--infile', help="Name of input file", type=str, required=True)

    parser.add_argument('-tm', '--tmfile', help="Name of input file with additional information in TableMerger", type=str, required=False)

    args = parser.parse_args()

    # logging information
    log_file = os.path.join(args.workdir, os.path.splitext(args.infile)[0] + '_PostProcesser.log')
    logging.basicConfig(level=logging.INFO,
                        format='%(asctime)s - %(levelname)s - %(message)s',
                        datefmt='%m/%d/%Y %I:%M:%S %p',
                        handlers=[logging.FileHandler(log_file),
                                    logging.StreamHandler()])

    # start main function
    logging.info('start script: '+"{0}".format(" ".join([x for x in sys.argv])))
    main(args, logging)
    logging.info('end script')