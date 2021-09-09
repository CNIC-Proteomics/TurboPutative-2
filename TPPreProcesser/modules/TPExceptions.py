#!/usr/bin/env python

# -*- coding: utf-8 -*-
import sys
import os
import json
import logging

# Import TP modules
import modules.constants as constants

class TPErrorClassBase:
    """
    Base class inherited by TurboPutative error classes
    """

    def __init__(self, workdir, msg, code):

        self.workdir = workdir
        #self.logging = logging
        
        self.errorInfo = {
            'msg': msg,
            'code': code
        }


    def writeErrorLog(self):
        """
        Write error.log file with error information used by the server
        """
        
        errorFile = os.path.join(self.workdir, "error.log")

        with open(errorFile, 'w') as f:
            json.dump(self.errorInfo, f)
        
        logging.info('error.log file was written')


    def exitProgram(self):
        """
        Print last logging and exit program
        """

        logging.info(self.errorInfo['msg'])
        logging.info(f"Leaving execution with code {self.errorInfo['code']}")
        sys.exit(self.errorInfo['code'])


class TPSizeTableError(Exception, TPErrorClassBase):
    """
    Error raised when input table has more than x rows
    and more than y columns
    """

    def __init__(self, shape, fullName, workdir):

        self.code = 10001
        self.msg = f"TPSizeTableError: Table dimensions are exceeded (maximum allowed: {constants.MAX_ROWS}x{constants.MAX_COLS}; {fullName}: {shape[0]}x{shape[1]})"

        # Initialize base class and go out
        TPErrorClassBase.__init__(self, workdir, self.msg, self.code)
        self.writeErrorLog()
        self.exitProgram()


class TPFormatError(Exception, TPErrorClassBase):
    """
    Error raised when user table has wrong format
    """

    def __init__(self, inFormat, accFormat, workdir):

        self.code = 10002
        self.msg = f"TPFormatError: {inFormat if len(inFormat) > 0 else 'No'} format was specified (accepted formats: {', '.join([i for i in accFormat])})"

        # Initialize base class and go out
        TPErrorClassBase.__init__(self, workdir, self.msg, self.code)
        self.writeErrorLog()
        self.exitProgram()


class TPOpenError(Exception, TPErrorClassBase):
    """
    Error raised when table cannot be openned
    """

    def __init__(self, fullName, workdir):

        self.code = 10003
        self.msg = f"TPOpenError: An error occurred when openning {fullName}"

        # Initialize base class and go out..
        TPErrorClassBase.__init__(self, workdir, self.msg, self.code)
        self.writeErrorLog()
        self.exitProgram()


class TPHeaderError(Exception, TPErrorClassBase):
    """
    Error raised when header is not found in user table
    """

    def __init__(self, fullName, workdir):

        self.code = 10004
        self.msg = f"TPHeaderError: Header was not found in {fullName}"

        # Initialize base class and go out..
        TPErrorClassBase.__init__(self, workdir, self.msg, self.code)
        self.writeErrorLog()
        self.exitProgram()


class TPColumnError(Exception, TPErrorClassBase):
    """
    Error raised when certain column is not present
    """

    def __init__(self, missingColumn, possibleNames, fullName, workdir):

        self.code = 10005
        self.msg = f"TPColumnError: {missingColumn} column was not found in {fullName} (possible names: {', '.join(possibleNames)})"

        # Initialize base class and go out..
        TPErrorClassBase.__init__(self, workdir, self.msg, self.code)
        self.writeErrorLog()
        self.exitProgram()


class TPWriteError(Exception, TPErrorClassBase):
    """
    Error raised when exception during file writing 
    """

    def __init__(self, outName, workdir):

        # logging.info(f"TPWriteError: An error occurred when writing {fullPath}")
        self.code = 10006
        self.msg = f"TPWriteError: An error occurred when writing {outName}"

        # Initialize base class and go out..
        TPErrorClassBase.__init__(self, workdir, self.msg, self.code)
        self.writeErrorLog()
        self.exitProgram()


class TPWriteHTMLError(Exception, TPErrorClassBase):
    """
    Error raised when exception during HTML file writing 
    """

    def __init__(self, outName, workdir):

        # logging.info(f"TPWriteError: An error occurred when writing {fullPath}")
        self.code = 100061
        self.msg = f"TPWriteHTMLError: An error occurred when writing {outName}"

        # Initialize base class and go out..
        TPErrorClassBase.__init__(self, workdir, self.msg, self.code)
        self.writeErrorLog()
        self.exitProgram()


class TPAdditionalInfoTableError(Exception, TPErrorClassBase):
    """
    Error raised when Table Merger table with additional information has 
    no header and number of rows different from 3
    """

    def __init__(self, fullName, workdir):

        self.code = 10007
        self.msg = f"TPAdditionalInfoTableError: No header detected in {fullName}. Hence, it must contain 3 columns ('Feature', 'Experimental mass', 'RT [min]')"

        # Initialize base class and go out..
        TPErrorClassBase.__init__(self, workdir, self.msg, self.code)
        self.writeErrorLog()
        self.exitProgram()


class TPDataTypeError(Exception, TPErrorClassBase):
    """
    Error raised when converting second column of TableMerger column with additional information
    """

    def __init__(self, fullName, workdir):

        self.code = 10008
        self.msg = f"TPDataTypeError: Experimental mass column in {fullName} could not be converted to float64 data type"

        # Initialize base class and go out..
        TPErrorClassBase.__init__(self, workdir, self.msg, self.code)
        self.writeErrorLog()
        self.exitProgram()


class TMTableColumnError(Exception, TPErrorClassBase):
    """
    Error raised when missing column containing experimental mass in TMTable
    """

    def __init__(self, possibleNames, fullName, workdir):

        self.code = 10009
        self.msg = f"TMTableColumnError: Column containing experimental mass was not found in {fullName}. Possible names: {', '.join(possibleNames)}"

        # Initialize base class and go out..
        TPErrorClassBase.__init__(self, workdir, self.msg, self.code)
        self.writeErrorLog()
        self.exitProgram()


class TPRowMergerComparedColumn(Exception, TPErrorClassBase):
    """
    Error raised when compared column is not in table
    """

    def __init__(self, missingCol, columnsList, fullName, workdir):

        self.code = 10010
        self.msg = f"TPRowMergerComparedColumn: Missing compared column ({', '.join(missingCol)}) in {fullName}"

        # Initialize base class and go out..
        TPErrorClassBase.__init__(self, workdir, self.msg, self.code)
        self.writeErrorLog()
        self.exitProgram()


class TPRowMergerConservedColumn(Exception, TPErrorClassBase):
    """
    Error raised when compared column is not in table
    """

    def __init__(self, missingCol, columnsList, fullName, workdir):

        self.code = 10011
        self.msg = f"TPRowMergerConservedColumn: Missing conserved column ({', '.join(missingCol)}) in {fullName}"

        # Initialize base class and go out..
        TPErrorClassBase.__init__(self, workdir, self.msg, self.code)
        self.writeErrorLog()
        self.exitProgram()


class TPTaggerError(Exception, TPErrorClassBase):
    """
    Error raised when there is an error in Tagger execution
    """

    def __init__(self, workdir):
        
        self.code = 20001
        self.msg = f"TPTaggerError: Error raised in Tagger execution"

        # Initialize base class and go out..
        TPErrorClassBase.__init__(self, workdir, self.msg, self.code)
        self.writeErrorLog()
        self.exitProgram()


class TPREnameError(Exception, TPErrorClassBase):
    """
    Error raised when there is an error in REname execution
    """

    def __init__(self, subModule, workdir):
        
        self.code = 30001
        self.msg = f"TPREnameError: Error raised in REname execution (sub-module: {subModule})"

        # Initialize base class and go out..
        TPErrorClassBase.__init__(self, workdir, self.msg, self.code)
        self.writeErrorLog()
        self.exitProgram()


class TPRowMergerError(Exception, TPErrorClassBase):
    """
    Error raised when there is an error in Tagger execution
    """

    def __init__(self, workdir):
        
        self.code = 40001
        self.msg = f"TPRowMergerError: Error raised in RowMerger execution"

        # Initialize base class and go out..
        TPErrorClassBase.__init__(self, workdir, self.msg, self.code)
        self.writeErrorLog()
        self.exitProgram()


class TPTableMergerError(Exception, TPErrorClassBase):
    """
    Error raised when there is an error in Tagger execution
    """

    def __init__(self, workdir):
        
        self.code = 50001
        self.msg = f"TPTableMergerError: Error raised in TableMerger execution"

        # Initialize base class and go out..
        TPErrorClassBase.__init__(self, workdir, self.msg, self.code)
        self.writeErrorLog()
        self.exitProgram()