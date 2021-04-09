#!/usr/bin/env python

# -*- coding: utf-8 -*-

# Import modules
import logging

import modules.TPExceptions as TPExc
import modules.constants as constants

class MSTableTester:

    def __init__(self, workflow):
        
        self.workflow = list(workflow)
        self.columns = constants.COLUMN_NAMES
    
    def testColumns(self, tableColumnsLower):
        """
        Check if required columns are present according with the module
        """

        # Name column was checked when reading table. It is the column used to find header.
        for module in self.workflow:

            if module == '1': # Tagger --> name

                testingColumn = "name"
                if not any([i.lower() in tableColumnsLower for i in self.columns[testingColumn]]):
                    raise TPExc.TPColumnError(testingColumn, self.columns[testingColumn], logging)
            
            elif module in ['2', '3', '4']: # REname, RowMerger --> name and mass // TableMerger --> mass (put together)

                testingColumn = "name"
                if not any([i.lower() in tableColumnsLower for i in self.columns[testingColumn]]):
                    raise TPExc.TPColumnError(testingColumn, self.columns[testingColumn], logging)

                testingColumn = "mass"
                if not any([i.lower() in tableColumnsLower for i in self.columns[testingColumn]]):
                    raise TPExc.TPColumnError(testingColumn, self.columns[testingColumn], logging)
        
        logging.info("MSTable columns checked successfully")