#!/usr/bin/env python

# -*- coding: utf-8 -*-


# Module metadata variables
__author__ = "Rafael Barrero Rodriguez"
__credits__ = ["Rafael Barrero Rodriguez", "Jose Rodriguez", "Jesus Vazquez"]
__license__ = "Creative Commons Attribution-NonCommercial-NoDerivs 4.0 Unported License https://creativecommons.org/licenses/by-nc-nd/4.0/"
__email__ = "rbarreror@cnic.es"
__status__ = "Development"

#
# IMPORT MODULES
#

import argparse
import configparser
from curses import use_default_colors
import logging
import os
import pandas as pd
import shutil
import sys

sys.path.append(os.path.join(os.path.dirname(__file__), "../TPPreProcesser"))
from modules.constants import TPIDX

#
# LOCAL FUNCTIONS
#


#
# MAIN
#
def main(args):
    
    df = pd.DataFrame([], columns=['pre', 'post'])

    for path, dirnames, filenames in os.walk(args.dir):
        
        # If root directory, go to next
        if path == args.dir:
            continue
            
        # Get job id
        jobid = os.path.basename(path)
        logging.info(f'Processing job: {jobid}')
        
        # Get names of each file in the folder
        pre = [os.path.join(path,i) for i in filenames if i==jobid+'.tsv'][0] # MS_experiment table
        post = [os.path.join(path,i) for i in filenames if i==jobid+'_REname.tsv'][0] # REname processed table
        ini = [os.path.join(path,i) for i in filenames if i==jobid+'.ini'][0] # ini file

        # Read configFile and get name of column name
        configFile = configparser.ConfigParser()
        configFile.read(ini)
        ncol = configFile['REname']['column_name']

        # Read MS_experiment
        predf = pd.read_csv(pre, sep='\t', usecols=[ncol, TPIDX]).rename(columns={ncol:'pre'})

        # Read REname
        postdf = pd.read_csv(post, sep='\t', usecols=[ncol, TPIDX]).rename(columns={ncol:'post'})

        # Merge MS_experiment and REname
        postdf[TPIDX] = postdf[TPIDX].str.split(' ; ')
        postdf = postdf.explode(TPIDX).astype({TPIDX:int})

        pre2post = pd.merge(
            predf,
            postdf,
            how='right',
            on=TPIDX
        ).drop(TPIDX, axis=1)

        logging.info(f'Added compounds from {jobid}: {pre2post.shape[0]}')

        df = pd.concat([df, pre2post])

        shutil.rmtree(path)
    
    df.drop_duplicates('pre', keep='first', inplace=True)
    logging.info(f'Total unique compounds: {df.shape[0]}')

    # Read preProcessedNames and combine
    pPNamesPath = os.path.join(os.path.dirname(__file__), "../TPProcesser/REname/data/preProcessedNames.tsv")
    
    logging.info(f'Reading preProcessedNames: {pPNamesPath}')
    
    df0 = pd.read_csv(
        pPNamesPath,
        sep='\t', names=['pre', 'post']
    )

    logging.info(f'Number of compounds in old preProcessedNames: {df0.shape[0]}')

    
    # Change pre to minus, sort and write
    df['pre'] = df['pre'].str.lower()

    df = pd.concat([df, df0]).drop_duplicates('pre', keep='first')
    logging.info(f'Number of compounds in new preProcessedNames: {df.shape[0]}')

    df.sort_values('pre', inplace=True)
    
    logging.info('Writing preProcessedNames')
    df.to_csv(pPNamesPath, sep='\t', header=False, index=False)
    
    return 0


if __name__ == '__main__':
    
    parser = argparse.ArgumentParser()
    parser.add_argument('--dir', type=str, help='Path to pending files')
    args = parser.parse_args()

    log_file = os.path.join(args.dir, 'preProcessedNames.log')
    logging.basicConfig(level=logging.INFO,
                        format=os.path.basename(__file__)+' - %(asctime)s - %(levelname)s - %(message)s',
                        datefmt='%m/%d/%Y %I:%M:%S %p',
                        handlers=[logging.FileHandler(log_file),
                                    logging.StreamHandler()])

    logging.info(f'start script: '+"{0}".format(" ".join([x for x in sys.argv])))
    main(args)
    logging.info(f'end script')