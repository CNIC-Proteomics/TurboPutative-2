#!/usr/bin/env python

# -*- coding: utf-8 -*-

# Import modules
import logging

import modules.TPExceptions as TPExc
import modules.constants as constants

class MSTableTester:

    def __init__(self, args):
        
        self.workflow = list(args.workflow)
        self.columns = constants.COLUMN_NAMES

        # Variables used if exception raised
        self.fullName = args.infile
        self.workdir = args.workdir

    
    def testColumns(self, tableColumnsLower):
        """
        Check if required columns are present according with the module
        """

        # Name column was checked when reading table. It is the column used to find header.
        for module in self.workflow:

            if module == '1': # Tagger --> name

                testingColumn = "name"
                if not any([i.lower() in tableColumnsLower for i in self.columns[testingColumn]]):
                    raise TPExc.TPColumnError(testingColumn, self.columns[testingColumn], self.fullName, self.workdir)
            
            elif module in ['2', '3', '4']: # REname, RowMerger --> name and mass // TableMerger --> mass (put together)

                testingColumn = "name"
                if not any([i.lower() in tableColumnsLower for i in self.columns[testingColumn]]):
                    raise TPExc.TPColumnError(testingColumn, self.columns[testingColumn], self.fullName, self.workdir)

                testingColumn = "mass"
                if not any([i.lower() in tableColumnsLower for i in self.columns[testingColumn]]):
                    raise TPExc.TPColumnError(testingColumn, self.columns[testingColumn], self.fullName, self.workdir)
        
        logging.info("MSTable columns checked successfully")