#!/usr/bin/env python

# -*- coding: utf-8 -*-

# Import modules
import os
import logging

# Class and function definition
class ExtensionMover():
    """
    Class used to move of files of an extension to a particular folder
    """

    def __init__(self, extension, workDir, logging, exclude=[]):

        self.extension = extension
        self.workDir = workDir
        self.logging = logging

        self.excludeFiles = exclude
        self.files = []

        self.makeDir()
        self.moveFiles()
    
    def makeDir(self):
        """
        """
        self.logging.info(f"Creating extension folder: {self.extension}")
        os.mkdir(os.path.join(self.workDir, self.extension))

    def moveFiles(self):
        """
        """
        allFiles = os.listdir(self.workDir)
        
        for fileName in allFiles:
            
            if fileName in self.excludeFiles: continue

            extension_i = os.path.splitext(fileName)[1]
            
            if extension_i == f".{self.extension}":
                self.logging.info(f"Move file to {self.extension} folder: {fileName}")
                oldPath = os.path.join(self.workDir, fileName)
                newPath = os.path.join(self.workDir, self.extension, fileName)
                os.rename(oldPath, newPath)
                self.files.append(fileName)


