#!/usr/bin/env python

# -*- coding: utf-8 -*-

# Import modules
import os
import logging
from configparser import ConfigParser

import modules.constants as constants


class ModuleInfo:
    """
    Write ini file containing information used by different C++ modules
    """
    def __init__(self):
        
        # Attributes
        self.config = ConfigParser()
        self.iniDict = {}
        self.workflow = None
    

    def addModules(self, workflow, workDir):
        """
        Add information of each module
        """

        self.workflow = workflow
        inFile = constants.OUTNAME
        cores = min(max(int(os.cpu_count()*constants.CORES_RATIO), 1), os.cpu_count()) # Get number of cores between 1 and maxCores

        for i, module in enumerate(self.workflow):

            if module == "1":
                outFile = f"{i+1}_Tagger.tsv"

                self.iniDict['Tagger'] = {
                    'infile': inFile,
                    'outfile': outFile,
                    'n': i+1,
                    'cores': cores
                }

            elif module == "2":
                outFile = f"{i+1}_REname.tsv"

                self.iniDict['REname'] = {
                    'infile': inFile,
                    'outfile': outFile,
                    'n': i+1,
                    'cores': cores
                }

            elif module == "3":
                outFile = f"{i+1}_RowMerger.tsv"
                self.iniDict['RowMerger'] = {
                    'infile': inFile,
                    'outfile': outFile,
                    'n': i+1
                }

            elif module == "4":
                outFile = f"{i+1}_TableMerger.tsv"

                self.iniDict['TableMerger'] = {
                    'infile': inFile,
                    'outfile': outFile,
                    'n': i+1,
                    "tmfile": constants.OUTNAME_TMTABLE
                }
            
            inFile = outFile
        

    def addColumnNamesMSTable(self, msColumns):
        """
        Add to config file de name of the MSTable columns that will be processed in each module
        """

        for module in self.workflow:

            if module == '1': # Tagger
                self.iniDict["Tagger"]["column_name"] = self.getColumnNameFromType(msColumns, "name")
                self.iniDict["Tagger"]["column_chemical_formula"] = self.getColumnNameFromType(msColumns, "chemical_formula")

            if module == '2': # REname
                self.iniDict["REname"]["column_name"] = self.getColumnNameFromType(msColumns, "name")
                self.iniDict["REname"]["column_mass"] = self.getColumnNameFromType(msColumns, "mass")

            if module == '3': # RowMerger
                self.iniDict["RowMerger"]["column_name"] = self.getColumnNameFromType(msColumns, "name")
                self.iniDict["RowMerger"]["column_mass"] = self.getColumnNameFromType(msColumns, "mass")

            if module == '4': # TableMerger
                self.iniDict["TableMerger"]["ms_column_mass"] = self.getColumnNameFromType(msColumns, "mass")
                self.iniDict["TableMerger"]["ms_column_rt"] = self.getColumnNameFromType(msColumns, "rt")


    def addColumnNamesTMTable(self, tmColumns):
        """
        Add to config file the name of the TMTable columns that will be processed
        """
        self.iniDict["TableMerger"]["tm_column_mass"] = self.getColumnNameFromType(tmColumns, "mass")
        self.iniDict["TableMerger"]["tm_column_rt"] = self.getColumnNameFromType(tmColumns, "rt")

    
    def getColumnNameFromType(self, columns, type):
        """
        From list with name of the columns, get the one corresponding to the type
        """
        
        possibleNames = [i.lower().strip() for i in constants.COLUMN_NAMES[type]]
        targetCol = [i for i in columns if i.lower().strip() in possibleNames]

        if len(targetCol) > 0:
            return targetCol[0]
        else:
            return "None"


    def writeINI(self, workDir):
        """
        Write information in INI file
        """

        self.config.read_dict(self.iniDict)

        fullPath = os.path.join(workDir, constants.INFO_FILENAME)
        with open(fullPath, 'w') as f:
            self.config.write(f)
        
        logging.info(f"Config file written successfully: {fullPath}")

# Define class
class InputINI:
    """
    Class to open and handle input ini file with user information
    """

    def __init__(self, workdir):
        
        self.fullPath = os.path.join(workdir, constants.INPUT_INI_FILENAME)
        self.config = self.readINI()
    
    def readINI(self):
        """
        Read ini file with input user information
        """
        config = ConfigParser()
        config.read(self.fullPath)
        return config
    
    def transferToModuleInfo(self, moduleInfo):
        """
        Transfer parameters from user ini to ini used by C++ modules (ModuleInfo)
        """

        # Loop over modules in C++ ini
        for module in moduleInfo.iniDict:

            if module == "Tagger":
                # transfer tags...
                for tag in constants.TAGS:
                    moduleInfo.iniDict[module][tag] = self.config[module][tag]

                # transfer other information of interest
                moduleInfo.iniDict[module]["halogenated_regex"] = self.config[module]["halogenated_regex"]
                moduleInfo.iniDict[module]["peptide_regex"] = self.config[module]["peptide_regex"]
            
            if module == "REname":
                moduleInfo.iniDict[module]["separator"] = self.config[module]["separator"]
                moduleInfo.iniDict[module]["aminoacid_separator"] = self.config[module]["aminoacid_separator"]
                moduleInfo.iniDict[module]["remove_row"] = self.config[module]["remove_row"]
            
            if module == "RowMerger":
                moduleInfo.iniDict[module]["compared_columns"] = self.config[module]["compared_columns"]
                moduleInfo.iniDict[module]["conserved_columns"] = self.config[module]["conserved_columns"]
            
            if module == "TableMerger":
                moduleInfo.iniDict[module]["n_digits"] = self.config[module]["n_digits"]
        
        return moduleInfo