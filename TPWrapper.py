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

# time control
import time; start_time=time.time()
ti = lambda: f"{str(round(time.time()-start_time, 3))}s"


# Import modules
import os
import sys
import logging
import argparse
import subprocess

# TurboPutative modules
scriptPath = os.path.dirname(__file__)
sys.path.append(os.path.join(scriptPath, "TPPreProcesser"))
sys.path.append(os.path.join(scriptPath, "TPPostProcesser"))
from TPPreProcesser.PreProcesser import main as PreProcesser
import TPPreProcesser.modules.TPExceptions as TPExc
from TPPostProcesser.PostProcesser import main as PostProcesser
from TPPostProcesser.modules.ExtensionMover import ExtensionMover

# Constants
modulePath = {
    'Tagger': os.path.join(scriptPath, "TPProcesser", "Tagger", "Tagger"),
    'REname1': os.path.join(scriptPath, "TPProcesser", "REname", "REname1"),
    'TPGoslin': os.path.join(scriptPath, "TPProcesser", "REname", "lib", "cppgoslin-1.1.2", "TPGoslin"),
    'REname2': os.path.join(scriptPath, "TPProcesser", "REname", "REname2"),
    'RowMerger': os.path.join(scriptPath, "TPProcesser", "RowMerger", "RowMerger"),
    'TableMerger': os.path.join(scriptPath, "TPProcesser", "TableMerger", "TableMerger")
}

# Main
def main(args):
    """
    Main
    """

    # 
    # PreProcesser
    #
    logging.info(f"{ti()} - Start PreProcesser")
    PreProcesser(args, logging)
    logging.info(f"{ti()} - End PreProcesser")
    # sys.exit(0)

    #
    # TurboPutative Processing (C++)
    #
    for module in args.workflow:

        if module == '1': # Tagger
            logging.info(f"{ti()} - Start Tagger")
            
            try:
                subprocess.run([modulePath['Tagger'], args.workdir], check=True)#, shell=True)
            
            except Exception as e:
                logging.exception("Error raised when executing Tagger. Traceback:")
                raise TPExc.TPTaggerError(args.workdir)

            logging.info(f"{ti()} - End Tagger")


        if module == '2': # REname

            # REname1
            logging.info(f"{ti()} - Start REname1")

            try:
                subprocess.run([modulePath['REname1'], args.workdir], check=True)#, shell=True)
            
            except Exception as e:
                logging.exception("Error raised when executing REname1. Traceback:")
                raise TPExc.TPREnameError("REname1", args.workdir)
            
            logging.info(f"{ti()} - End REname1")


            # TPGoslin
            logging.info(f"{ti()} - Start TPGoslin")

            try:
                subprocess.run([modulePath['TPGoslin'], args.workdir], check=True)#, shell=True)
            
            except Exception as e:
                logging.exception("Error raised when executing TPGoslin. Traceback:")
                raise TPExc.TPREnameError("TPGoslin", args.workdir)

            logging.info(f"{ti()} - End TPGoslin")


            # REname2
            logging.info(f"{ti()} - Start REname2")

            try:
                subprocess.run([modulePath['REname2'], args.workdir], check=True)# , shell=True)
            
            except Exception as e:
                logging.exception("Error raised when executing REname2. Traceback:")
                raise TPExc.TPREnameError("REname2", args.workdir)

            logging.info(f"{ti()} - End REname2")


            # Remove files
            os.remove(os.path.join(args.workdir, "compound.txt"))
            os.remove(os.path.join(args.workdir, "parsed_compound.txt"))
            os.remove(os.path.join(args.workdir, "compound_index.txt"))
            os.remove(os.path.join(args.workdir, "mappedIndex.txt"))


        if module == '3': # RowMerger
            logging.info(f"{ti()} - Start RowMerger")

            try:
                subprocess.run([modulePath['RowMerger'], args.workdir], check=True)#, shell=True)
            
            except Exception as e:
                logging.exception("Error raised when executing RowMerger. Traceback:")
                raise TPExc.TPRowMergerError(args.workdir)

            logging.info(f"{ti()} - End RowMerger")


        if module == '4': # TableMerger
            logging.info(f"{ti()} - Start TableMerger")

            try:
                subprocess.run([modulePath['TableMerger'], args.workdir], check=True)#, shell=True)
            
            except Exception as e:
                logging.exception("Error raised when executing TableMerger. Traceback:")
                raise TPExc.TPTableMergerError(args.workdir)
            
            logging.info(f"{ti()} - End TableMerger")

    #
    # PostProcesser
    #
    logging.info(f"{ti()} - Start PostProcesser")
    PostProcesser(args, logging)
    logging.info(f"{ti()} - End PostProcesser")


    #
    # Sort .log, .ini and .tsv files
    #

    extensions = ['ini', 'tsv', 'html', 'row', 'json']
    
    # Create folder and move files of that extension there
    _ = [ExtensionMover(i, args.workdir, logging) for i in extensions]

    # In case of log the 'TPWrapper.log' must be excluded
    _ = ExtensionMover('log', args.workdir, logging, [log_file_base_name])
    



if __name__ == "__main__":

    # parse arguments
    parser = argparse.ArgumentParser(
        description='TPWrapper',
        epilog=r"""
        Example:
            python TPWrapper.py -wd D:\CNIC\Metabolomica\TurboPutative-2.0\test\test13 -wf 1234 -i MS_experiment.xls -tm FeatureInfo.xlsx
        """
    )

    parser.add_argument('-wd', '--workdir', help="Path to working directory", type=str, required=True)

    parser.add_argument('-wf', '--workflow', help="Modules executed: Tagger(1), REname(2), RowMerger(3), TableMerger(4)",
                         type=str, required=True)

    parser.add_argument('-i', '--infile', help="Name of input file", type=str, required=True)

    parser.add_argument('-tm', '--tmfile', help="Name of input file with additional information in TableMerger", type=str, required=False)

    args = parser.parse_args()

    # logging information
    log_file_base_name = os.path.splitext(args.infile)[0] + '_TPWrapper.log'
    log_file = os.path.join(args.workdir, log_file_base_name)
    logging.basicConfig(level=logging.INFO,
                        format='%(asctime)s - %(levelname)s - %(message)s',
                        datefmt='%m/%d/%Y %I:%M:%S %p',
                        handlers=[logging.FileHandler(log_file),
                                    logging.StreamHandler()])


    # start main function
    logging.info(f'{ti()} - start script: '+"{0}".format(" ".join([x for x in sys.argv])))
    main(args)
    logging.info(f'{ti()} - end script')
