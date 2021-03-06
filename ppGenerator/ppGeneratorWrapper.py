#!/usr/bin/env python

# -*- coding: utf-8 -*-

'''
Name: Rafael Barrero Rodriguez
Date: 2021-04-22
Description: Script para generar base de datos que relaciona nombre inicial con nombre
final.
Execution: python ppGenerator\ppGeneratorWrapper.py
'''

#
# Import modules
#
import argparse
import os
import sys
import subprocess
import numpy as np
import pandas as pd
from zipfile import ZipFile

import time; start_time = time.time(); getTime = lambda : round(time.time() - start_time,3)


#
# Constants
#

# Script path used as path reference
scriptPath = os.path.dirname(__file__)

# Path to REname data folder where MapTable is located (file where output files will be saved)
REnameDataPath = os.path.join(scriptPath, '../TPProcesser/REname/data')

# Name of the file with the table with all assignations (input and output)
TPMapTable = "preProcessedNames.tsv"

# Name of the file with the index (output)
# TPMapTableIndex = "preProcessedNamesIndex.tsv"

# Name of the file containing original compounds (without any lower modification)
TPOriginalTable = "originalNames.tsv"

# Name of the file with mapping generated in the execution. User can check this table (output)
lastExecutionMap = "lastExecutionMap.tsv"

# Path to public folder where zip files with results will be copied
publicPath = os.path.join(scriptPath, '../../public/REnameMapTables')

# Characters replaced in processCompounds function
characterToReplace = {
    '±': '+/-',
    'α': 'alpha',
    'β': 'beta',
    'γ': 'gamma',
    'δ': 'delta',
    'ω': 'omega'
}


#
# Function definition
#

def readFiles (directory):
    """
    Description: Build a pandas dataframe with multiple files contained in a given directory
    """

    files = [i for i in os.listdir(directory) if os.path.isfile(os.path.join(directory, i))]
    
    if len(files) == 0:
        print(f"** {getTime()}s - Given directory has no file")
        sys.exit()
    
    # read and concatenate all files
    df = pd.concat([pd.read_csv(os.path.join(directory, i), sep="\t", header=None) for i in files])

    return df


def processCompounds (df, workingPath = scriptPath, apiMode=False):
    """
    Description: Process name of compounds using ppGenerator1, TPGoslin and ppGenerator2

    Input:
        - df: Pandas dataframe (with one column) containing compounds that must be processed
        - workingPath: String with path where files are going to be generated. (Crucial in API mode)
        - apiMode: Boolean indicating api mode execution
    
    Output:
        - Pandas dataframe (with two columns) containing mapped compounds
    """

    # Set column name
    df.columns = ["original"]

    # Preprocess name of the compounds (remove some characters and replace others)
    df['original'] = df['original'].str.replace(r"(;|\s/\s|\n)(\n|.)*$", "", regex=True)  # Remove multiple compounds in a single field
    
    for i in characterToReplace:
        df['original'] = df['original'].str.replace(i, characterToReplace[i])

    df['original'] = df['original'].str.strip() # strip names of the compounds

    # Save file with compounds after a brief preprocessing
    preProcessedFile = "compoundList.txt"
    df.to_csv(os.path.join(workingPath, preProcessedFile), header=None, index=None, sep="\t")  

    # Execute first part of name processing
    print(f"** {getTime()}s - Execute ppGenerator1")
    dbGenerator1Path = os.path.join(scriptPath, "ppGenerator1")
    subprocess.run([dbGenerator1Path, workingPath, preProcessedFile], check=True)

    # Execute TPGoslin
    print(f"** {getTime()}s - Execute TPGoslin")
    TPGoslinPath = os.path.join(scriptPath, "../TPProcesser/REname/lib/cppgoslin-1.1.2/TPGoslin")
    subprocess.run([TPGoslinPath, workingPath], check=True)

    # Execute second part of name processing
    print(f"** {getTime()}s - Execute ppGenerator2")
    dbGenerator2Path = os.path.join(scriptPath, "ppGenerator2")
    subprocess.run([dbGenerator2Path, workingPath], check=True)

    if apiMode:
        return None

    # Read .tsv with processed names
    print(f"** {getTime()}s - Reading preProcessed compounds")
    processedCompoundsFile = "pre_processed_compound.txt" # Name of the output file generated by ppGenerator2
    processedCompounds = pd.read_csv(os.path.join(workingPath, processedCompoundsFile), sep="\t", header=None)
    processedCompounds.rename(columns={0:'preProcessed'}, inplace=True)

    # Convert original compounds to lower
    df['original'] = df['original'].str.lower()

    

    # build map table with original and processed compounds
    print(f"** {getTime()}s - Build map table")

    df.reset_index(drop=True, inplace=True)
    processedCompounds.reset_index(drop=True, inplace=True)
    mapTable = pd.concat([df, processedCompounds], axis=1)
    mapTable.to_csv(os.path.join(REnameDataPath, lastExecutionMap), sep="\t", header=None, index=None)

    # remove files generated in the execution
    os.remove(os.path.join(workingPath, preProcessedFile)) # infile of ppGenerator1
    os.remove(os.path.join(workingPath, "compound_original.txt")) # outfile of ppGenerator1 for ppGenerator2 (name of compounds)
    os.remove(os.path.join(workingPath, "compound_index.txt")) # outfile of ppGenerator1 for ppGenerator2 (index of goslin compounds)
    os.remove(os.path.join(workingPath, "compound.txt")) # outfile of ppGenerator1 for TPGoslin
    os.remove(os.path.join(workingPath, "parsed_compound.txt")) # outfile of TPGoslin
    os.remove(os.path.join(workingPath, "TPGoslin.log")) # outfile of TPGoslin
    os.remove(os.path.join(workingPath, "mappedIndex.txt")) # outfile of TPGoslin
    os.remove(os.path.join(workingPath, processedCompoundsFile)) # outfile of ppGenerator2
    
    return mapTable


def preMappedTable (df):
    """
    Description: If ppGenerator receives table with some assignations,
    split it, and process compounds that are not assigned

    Input:
        - df: Pandas dataframe with two columns
    
    Output:
        - df: Pandas dataframe with two columns
    """

    df.columns = ['original', 'preProcessed']

    # Bool with True in unmapped compounds
    unMappedBool = pd.isna(df['preProcessed']).to_numpy()
    
    # Table containing compounds that were not mapped
    unMappedOnly = pd.DataFrame(df.iloc[unMappedBool, 0])

    # Table containing premapped compounds
    MappedOnly = pd.DataFrame(df.iloc[~unMappedBool, :])

    # Process compounds that were not mapped
    if np.any(unMappedBool):
        unMappedOnly_mapped = processCompounds(unMappedOnly)
        MappedOnly = pd.concat([MappedOnly, unMappedOnly_mapped])

    # return concat premapped and mapped
    return MappedOnly


def updateTPMapTable (mapTable):
    """
    Description: Update TPMapTable used by TurboPutative

    Input:
        - mapTable: Pandas dataframe with two columns (original and preProcessed)
    
    Output:
        - Write the TPMapTable
    """

    # Read destiny file
    print(f"** {getTime()}s - Read old map table")
    mapTableOld = pd.read_csv(os.path.join(REnameDataPath, TPMapTable), sep="\t", header=None)
    mapTableOld.rename(columns={0:'original', 1:'preProcessed'}, inplace=True)
    
    # Convert original compounds to lower in both cases
    mapTableOld['original'] = mapTableOld['original'].str.lower()
    mapTable['original'] = mapTable['original'].str.lower()

    # Remove duplicates from old table
    selectedRows = ~np.isin(mapTableOld['original'].to_numpy(), mapTable['original'].to_numpy())
    mapTableOld = mapTableOld.loc[selectedRows, :]

    # Combine both .tsv and remove repetitions
    print(f"** {getTime()}s - Update map table")
    mapTableUpdated = pd.concat([mapTable, mapTableOld])
    mapTableUpdated.drop_duplicates(subset='original', inplace=True)
    mapTableUpdated.sort_values(by=['original'], inplace=True)

    # Save map table
    print(f"** {getTime()}s - Writing map table")
    mapTableUpdated.to_csv(os.path.join(REnameDataPath, TPMapTable), sep="\t", header=None, index=None)

    # We do not need index, as REname1 performs binary search

    # Build index
    #print(f"** {getTime()}s - Building index")
    #originalArr = mapTableUpdated.iloc[:, 0].to_numpy()
    #lenIdx = int(np.sqrt(len(originalArr)))

    #index = [[i, n] for n,i in enumerate(originalArr) if n%lenIdx==0]
    #indexTable = pd.DataFrame(index)
    #indexTable.to_csv(os.path.join(REnameDataPath, TPMapTableIndex), header=None, index=None, sep="\t")


def copyToPublic():
    """
    Description: Create a zip containing TPMapTable, its index, and compounds processed.
    Zip file will be saved in TPMapTables public folder using date as name. 
    Doing this, different versions of TPMapTable and compounds added can be checked
    """

    with ZipFile(os.path.join(publicPath, time.strftime("%Y%m%d-%H%M%S.zip")), 'w') as zipObj:

        zipObj.write(os.path.join(REnameDataPath, TPMapTable), TPMapTable)
        # zipObj.write(os.path.join(REnameDataPath, TPMapTableIndex), TPMapTableIndex)
        zipObj.write(os.path.join(REnameDataPath, lastExecutionMap), lastExecutionMap)


#
# Main function
#

def main(args):
    """
    Main function
    """

    # Build pandas dataframe with compounds

    if args.dir != None:
        df = readFiles(args.dir)
    
    elif args.infile != None:
        if not os.path.isfile(args.infile):
            print(f"** {getTime()}s - Given file does not exist")
            sys.exit()

        df = pd.read_csv(args.infile, sep="\t", header=None)

    
    # Update file containing original compounds
    if not args.api:
        origTable = pd.read_csv(os.path.join(REnameDataPath, TPOriginalTable), sep="\t", header=None)
        pd.concat([origTable[0], df[0]])\
            .drop_duplicates()\
            .to_csv(os.path.join(REnameDataPath, TPOriginalTable), sep="\t", header=False, index=False)
    

    # Build mapTable that has to be added 
    if df.shape[1] == 1:
        workingPath = scriptPath if args.infile == None else os.path.dirname(os.path.abspath(args.infile))
        mapTable = processCompounds(df, workingPath, args.api)
    
    elif df.shape[1] == 2:
        mapTable = preMappedTable(df)

    # If execution is done in API mode, return
    if args.api:
        return


    # Update TPMapTable
    updateTPMapTable(mapTable)

    # Generate Zip file with results (TPMapTable, index and lastExecutionResult) in public folder to it can be accessed programatically
    if os.path.isdir(publicPath):
        copyToPublic()



#
# Execution
#

if __name__ == "__main__":

    print(f"** {getTime()}s - Start processing")

    parser = argparse.ArgumentParser()

    parser.add_argument('--infile', help="Path to file with list of compounds", type=str)
    parser.add_argument('--dir', help="Path to directory containing files with list of compounds", type=str)
    parser.add_argument('--api', help="Exexute script in REST API mode", action='store_true')

    args = parser.parse_args()

    if args.infile==None and args.dir==None:
        parser.print_help()
        sys.exit()

    main(args)
    print(f"** {getTime()}s - End processing")