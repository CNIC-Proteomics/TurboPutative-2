#!/usr/bin/env python

# -*- coding: utf-8 -*-

# Import modules
import os
import pandas as pd
import logging
import zipfile

# Class and function definitions

class ResultWriter:
    """
    Class to write output tables
    """

    def __init__(self, workDirPath, infile, log):

        self.logging = log
        self.workDir = workDirPath
        self.infile = infile
        self.combinedOutFileName = os.path.splitext(infile)[0] + "_AllResults.xlsx"
        self.combinedOutFileFullPath = os.path.join(self.workDir, self.combinedOutFileName)
        # self.writer = pd.ExcelWriter(self.combinedOutFileFullPath, engine="openpyxl")

        self.tableFileNames = [] # list with the name of tables added (.tsv here)
        self.finalFileNames = [] # list with name of final files (.xlsx here)


    def addTable(self, fileName, writeApart=False):
        """
        Method used to add file to combined results and (if selected) write it apart
        """

        # open table
        try:
            df = self.openTable(fileName)
        
        except:
            self.logging.error(f"TPErr: Error when reading table: {fileName}")
            return None


        # write apart if selected
        self.writeTable(fileName, df) if writeApart else self.finalFileNames.append(fileName)
        
        # add to combined results
        # self.addSheet(fileName, df)

        self.logging.info(f"Table was added: {fileName}")


    def openTable(self, fileName):

        self.logging.info(f"Reading table: {fileName}")
        baseName, extension = os.path.splitext(fileName)

        if extension == ".xls":
            df = pd.read_excel(os.path.join(self.workDir, fileName), engine="xlrd")
        
        if extension == ".xlsx":
            df = pd.read_excel(os.path.join(self.workDir, fileName), engine="openpyxl")
        
        if extension == ".tsv":
            df = pd.read_csv(os.path.join(self.workDir, fileName), sep="\t")
        
        self.tableFileNames.append(fileName)

        self.logging.info(f"Table read")
        
        return df
    

    def writeTable(self, fileName, df):
        """
        Write table in a isolated file (xlsx)
        """
        self.logging.info(f"Writing table: {fileName}")
        
        outFileName = f"{os.path.splitext(fileName)[0]}_{os.path.splitext(self.infile)[0]}.xlsx"
        df.to_excel(os.path.join(self.workDir, outFileName), index=False, engine="openpyxl")
        self.finalFileNames.append(outFileName)
        
        self.logging.info(f"Table written")
    

    def addSheet(self, fileName, df):
        """
        Add sheet to combined excel file
        """
        sheetName = os.path.splitext(fileName)[0]
        df.to_excel(self.writer, index=False, sheet_name=sheetName)
    
    
    def saveCombinedTable(self):
        """
        Save xlsx file with all tables
        """
        self.writer.save()
        self.writer.close()
        self.finalFileNames.append(self.combinedOutFileName)
        self.logging.info(f"File was written: {self.combinedOutFileName}")


    def zipResults(self):
        """
        Make a zip with result tables
        """
        self.logging.info("Make a zip with all result files")

        zipName = f"TurboPutative_results.zip"
        fullPath = os.path.join(self.workDir, zipName)

        with zipfile.ZipFile(fullPath, 'w') as myzip:
            _ = [myzip.write(os.path.join(self.workDir, i), i) for i in self.finalFileNames]
            myzip.close()
        
        # remove files added to zip
        _ = [os.remove(os.path.join(self.workDir, i)) for i in self.finalFileNames]
        
        self.logging.info(f"Zip was created: {zipName}")