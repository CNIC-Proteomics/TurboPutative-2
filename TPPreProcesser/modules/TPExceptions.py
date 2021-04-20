#!/usr/bin/env python

# -*- coding: utf-8 -*-

import sys

class TPFormatError(Exception):
    """
    Error raised user table has wrong format
    """

    def __init__(self, inFormat, accFormat, logging):

        logging.info(f"TPFormatError: {inFormat if len(inFormat) > 0 else 'No'} format was specified (accepted: {', '.join([i for i in accFormat])}).")
        self.code = 10000
        sys.exit(self.code)


class TPOpenError(Exception):
    """
    Error raised when table cannot be openned
    """

    def __init__(self):

        self.code = 10001
        sys.exit(self.code)


class TPHeaderError(Exception):
    """
    Error raised when header is not found in user table
    """

    def __init__(self, requiredColumns, maxRowsToFindHeader, inFileName, logging):

        logging.info(f"TPHeaderError: Header was not found in {inFileName}.")
        self.code = 10002
        sys.exit(self.code)


class TPColumnError(Exception):
    """
    Error raised when certain column is not present
    """

    def __init__(self, missingColumn, possibleNames, logging):

        logging.info(f"TPColumnError: {missingColumn} column was not found. Possible names: {', '.join(possibleNames)}.")
        self.code = 10003
        sys.exit(self.code)


class TPWriteError(Exception):
    """
    Error raised when exception during file writing 
    """

    def __init__(self, fullPath, logging):

        # logging.info(f"TPWriteError: An error occurred when writing {fullPath}")
        self.code = 10004
        sys.exit(self.code)


class TPAdditionalInfoTableError(Exception):
    """
    Error raised when Table Merger table with additional information has no header and number of rows different from 3
    """

    def __init__(self, nCols, logging):

        logging.info(f"""
        TPAdditionalInfoTableError: TableMerger table with additional information has no header and 
        it contains {nCols} columns (it should contain 3 columns if there is no header).
        """)
        self.code = 10005
        sys.exit(self.code)


class TPDataTypeError(Exception):
    """
    Error raised when converting second column of TableMerger column with additional information
    """

    def __init__(self):

        self.code = 10006
        sys.exit(self.code)

class TMTableColumnError(Exception):
    """
    Error raised when missing column containing experimental mass in TMTable
    """

    def __init__(self, possibleNames, logging):

        logging.info(f"""
        TMTableColumnError: Column containing experimental mass was not found. Possible names: {', '.join(possibleNames)}.
        """)
        self.code = 10007
        sys.exit(self.code)


class TPRowMergerComparedColumn(Exception):
    """
    Error raised when compared column is not in table
    """

    def __init__(self, comparedList, columnsList):

        logging.info(f"""
        TPRowMergerComparedColumn: Not all compared column ({', '.join(comparedList)}) are in input table.
        """)
        self.code = 10008
        sys.exit(self.code)


class TPRowMergerConservedColumn(Exception):
    """
    Error raised when compared column is not in table
    """

    def __init__(self, conservedList, columnsList, logging):

        logging.info(f"""
        TPRowMergerConservedColumn: Not all conserved column ({', '.join(conservedList)}) are in input table.
        """)
        self.code = 10008
        sys.exit(self.code)