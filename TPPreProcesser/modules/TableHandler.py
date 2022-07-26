#!/usr/bin/env python

# -*- coding: utf-8 -*-

# Import modules
import os
import sys
import numpy as np
import pandas as pd
import logging
import re

# TP modules
import modules.constants as constants
import modules.TPExceptions as TPExc


# Class definition
class TPTable:
    """
    Generic class for table handling
    """

    def __init__(self, infilePath):

        # constant attributes
        self.acceptedFormat = constants.ACCEPTED_FORMAT

        # attributes that must be set
        self.table = None
        self.tableColumnsLow = None
        self.tableFormat = None
        self.tableName = None
        self.fullName = None
        self.tablePath = infilePath

        # get working directory (used by exceptions)
        self.workdir = os.path.dirname(infilePath)

        # parse name
        self.parseName()


    def parseName(self):
        """
        Get table name and format from full path
        """
        self.fullName = os.path.basename(self.tablePath)
        self.tableName = os.path.splitext(self.fullName)[0]
        self.tableFormat = os.path.splitext(self.fullName)[1]

        if self.tableFormat not in self.acceptedFormat:
            raise TPExc.TPFormatError(self.tableFormat, self.acceptedFormat, self.workdir)


    def openTable(self, headerRow):
        """
        Open table considering different formats
        """

        # if error when openning table, exit...
        try:
            if self.tableFormat == ".xls":
                self.table = pd.read_excel(self.tablePath, engine="xlrd", header=headerRow)
            
            elif self.tableFormat == ".xlsx":
                self.table = pd.read_excel(self.tablePath, engine="openpyxl", header=headerRow)
            
            elif self.tableFormat == ".tsv":
                self.table = pd.read_csv(self.tablePath, sep="\t", header=headerRow)
        
        except:
            logging.exception(f"TPOpenError: An error occurred when openning {self.fullName}. Traceback:")
            raise TPExc.TPOpenError(self.fullName, self.workdir)

        # Remove rows and columns containing NA only
        self.table.dropna(axis=0, how="all", inplace=True)
        self.table.dropna(axis=1, how="all", inplace=True)

        # check that table does not exceed maximum number of rows and columns
        if self.table.shape[0] > constants.MAX_ROWS or self.table.shape[1] > constants.MAX_COLS:
            raise TPExc.TPSizeTableError(self.table.shape, self.fullName, self.workdir)


    def removeLineFall(self):
        """
        Rpelace \n by \s in every string field (np.dtype('O'))
        """

        # lambda function to replace \n by \s
        func = lambda serie: serie.str.replace("\n", " ", regex=True) if serie.dtype==np.dtype('O') else serie

        # apply lambda function to each column
        self.table = self.table.apply(func, axis=0)


    def replaceChars(self):
        """
        Replace rare characters in name column: 
            - ± -> +/-
            - α -> alpha
            - β -> beta
            - γ -> gamma
            - δ -> delta
            - ω -> omega
        """

        characterToReplace = {
            '±': '+/-',
            'α': 'alpha',
            'β': 'beta',
            'ß': 'beta',
            'γ': 'gamma',
            'δ': 'delta',
            'ω': 'omega'
        }

        # if table has a column with name of the compounds, make there the replace
        nameColName = [i for i in self.table.columns if i.lower() in constants.COLUMN_NAMES["name"]]
        if len(nameColName) > 0:

            # make all replacements
            for i in characterToReplace:
                self.table[nameColName[0]] = self.table[nameColName[0]].str.replace(i, characterToReplace[i], regex=True)



    def writeTable(self, directory, outName):
        """
        Write output table
        """

        fullPath = os.path.join(directory, outName)

        try:
            self.table.to_csv(fullPath, sep="\t", index=False, na_rep="N/A", encoding='utf-8')
        
        except:
            logging.exception(f"TPWriteError: An error occurred when writing {fullPath}. Traceback:")
            raise TPExc.TPWriteError(outName, self.workdir)
        
        logging.info(f"{outName} was written successfully: {fullPath}")

    
    def writeHTMLTable(self):
        """
        Write table in HTML format
        """

        exportColumns = self.table.columns if len(self.table.columns)<6  \
            else [k for k in self.table.columns for i in constants.COLUMN_NAMES for j in constants.COLUMN_NAMES[i] if k.lower() == j and i != 'inchi_key']

        outName = self.tableName

        fullPath = os.path.join(self.workdir, outName)

        try:
            falseArr = np.zeros(self.table.shape[0], dtype='bool')
            self.table.loc[falseArr, :].to_html(fullPath+'.html', index=False, na_rep='-', columns=exportColumns)
            self.table.to_csv(fullPath+'.row', sep="\t", index=False, header=False, na_rep='-', columns=exportColumns)
        
        except:
            logging.exception(f"TPWriteHTMLError: An error occurred when writing {fullPath}. Traceback:")
            raise TPExc.TPWriteHTMLError(outName, self.workdir)
        
        logging.info(f"{outName} was written successfully: {fullPath}")



class MSTable(TPTable):
    """
    Class to handle table containing MS data (derived from TPTable)
    """

    def __init__(self, inFilePath):

        # attributes
        self.requiredColumns = constants.REQUIRED_COLUMN # [[name11, name12], [name21, name22]] Each element is a list with possibilities
        self.maxRowsToFindHeader = constants.MAX_ROW_HEADER # Including 0

        # initialize parent attributes
        super().__init__(inFilePath)          
    

    def readTable(self):
        """
        Read table from path given by the user
        """
        headerFound = False
        headerRow = 0

        # try to read considering different header positions
        while (not headerFound) and (headerRow <= self.maxRowsToFindHeader):

            # open table considering different formats
            self.openTable(headerRow)

            # Set column names to lower case
            self.tableColumnsLow = [str(i).lower().strip() for i in self.table.columns]
            
            # Check if required columns are present (considering different names)
            if all([any([j.lower().strip() in self.tableColumnsLow for j in i]) for i in self.requiredColumns]):
                headerFound = True
            else:
                headerRow += 1
            
        # if header was not found in different tries, raise error. Else, log it
        if not headerFound:
            raise TPExc.TPHeaderError(self.fullName, self.workdir)
        else:
            logging.info(f"MSTable was read successfully: {self.tablePath}")
    
    def extractCompoundNames(self, workdir):
        """
        Extract compound names and save in ppGenerator pendingFiles folder
        NOT USED
        """
        
        # The name of the will be the id of the project
        pendingFileName = os.path.basename(os.path.normpath(workdir)) + '.tsv'
        pendingFilesPath = os.path.join('./src/TurboPutative-2.0-built/ppGenerator/pendingFiles', pendingFileName)
        
        # Find name of the column containing compound names
        columnName = [i for i in self.table.columns if i.lower() in constants.COLUMN_NAMES["name"]][0]

        # Remove "No compounds found"
        filteredBool = [False if re.search('(?i)^no compounds found', i) else True \
            for i in self.table.loc[:, columnName].to_numpy()]

        # Save file
        self.table.loc[filteredBool, :].to_csv(pendingFilesPath, sep="\t", header=None, index=None, columns=[columnName])



class TMTable(TPTable):
    """
    Class to handle TableMerger table with additional information
    """

    def __init__(self, inFilePath):
        
        # attributes
        self.requiredColumns = constants.REQUIRED_COLUMN_TMTABLE
        self.maxRowsToFindHeader = 0 # Including 0
        self.massColName = None

        # initialize parent attributes
        super().__init__(inFilePath)         
    

    def readTable(self):

        # Open table considering headerRow = 0
        self.openTable(headerRow=0)

        # Get parsed column names
        self.tableColumnsLow = [str(i).lower().strip() for i in self.table.columns]

        # If header column was not found, re-open without headerRow
        if not all([any([j.lower().strip() in self.tableColumnsLow for j in i]) for i in self.requiredColumns]):
            logging.info("No header detected in TMTable. Set default columns (Feature, Experimental mass and RT [min]")

            # open without header
            self.openTable(headerRow=None)
        
            # if number of columns is not 3, raise error
            if self.table.shape[1] != 3:
                raise TPExc.TPAdditionalInfoTableError(self.fullName, self.workdir)

            # set column names
            self.table.columns = ['Feature', 'Experimental mass', 'RT [min]']
            self.tableColumnsLow = [str(i).lower().strip() for i in self.table.columns]


        # Check if there is a column containing experimental mass
        massPossibleNames = [i.lower().strip() for i in constants.COLUMN_NAMES['mass']]
        massColumnBool = [i.lower().strip() in massPossibleNames for i in self.table.columns]

        if not any(massColumnBool):
            raise TPExc.TMTableColumnError(constants.COLUMN_NAMES["mass"], self.fullName, self.workdir)

        self.massColName = self.table.columns[massColumnBool][:1] # Take only the first column

        # Try to convert column with experimental mass into float64 dtype
        try:
            self.table.loc[:, self.massColName] = self.table.loc[:, self.massColName].astype('float64')

        except:
            logging.exception(f"""
            TPDataTypeError: Experimental mass column in TMTable could not be converted to float64 data type. Traceback:
            """)
            raise TPExc.TPDataTypeError(self.fullName, self.workdir)
        
        logging.info(f"TMTable was read successfully: {self.tablePath}")

    def colRename(self, msColumns):
        """
        If columns of TMTable appear on msColumns, rename them (add TM file name as prefix)
        """
        
        # Possible names for mass columns (in lower)
        massColumnPossibleNames = [i.lower().strip() for i in constants.COLUMN_NAMES["mass"]]

        # Find TM column names that are in msColumns and are differente from mass columns
        colToRename = {}
        _ = [colToRename.update({i: f"{self.tableName}_{i}"}) for i in self.table.columns 
            if (i in msColumns) and (i.lower().strip() not in massColumnPossibleNames)]

        self.table.rename(columns=colToRename, inplace=True)