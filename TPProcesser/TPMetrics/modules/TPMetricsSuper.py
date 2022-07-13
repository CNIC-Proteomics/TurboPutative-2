#!/usr/bin/env python

# -*- coding: utf-8 -*-

#
# IMPORT MODULES
#

import configparser
import json
import os
import re

#
# FUNTION AND CLASSES
#

class TPMetricsSuper():

    def __init__(self, workdir):

        self.workdir = workdir
        self.config = configparser.ConfigParser()
        self.config.read(os.path.join(self.workdir, 'configFile.ini'))

        
        # Get name of main columns
    
        self.m = self.config['TPMetrics']['column_mass']
        self.n = self.config['TPMetrics']['column_name']
        self.rt = self.config['TPMetrics']['column_rt']
        self.a = self.config['TPMetrics']['column_adduct']
        self.w = self.config['TPMetrics']['column_molweight']
        self.e = self.config['TPMetrics']['column_error']
        self.i = self.config['TPMetrics']['column_intensities'].split(' // ')

        
        # Define name of added columns

        self.tpc = 'TP_Class'
        self.tpcL = 'TP_Class_List'
        self.tpcL_s3s = 'TP_Class_Adduct_List'
        #self.tpcA = 'TP_Class_adduct'

        self.s1a = 'TP_all_corr1'
        self.s1aF = 'TP_corr1_Apex'
        self.s1m = 'TP_max_corr1'
        self.s1mF = 'TP_max_corr1_Apex'
        self.s1s = 'TP_sum_corr1'
        self.s1n = 'TP_n_corr1'
        self.s1mp = 'TP_max_corr1_pvalue'
        self.s1ms = 'TP_max_corr1_score'
        self.s1avg = 'TP_all_corr1_avg'
        self.s1sp = 'TP_sum_corr1_pvalue'
        self.s1ss = 'TP_sum_corr1_score'
        self.s1 = 'TP_corr1_score'

        self.s1argmax = 'TP_corr1_argmax'
        self.s1argmaxs = 'TP_corr1_score_argmax'
        self.s1argmaxp = 'MolWeight_argmax'


        self.s2a = 'TP_all_corr2'
        self.s2aF = 'TP_corr2_Apex'
        self.s2m = 'TP_max_corr2'
        self.s2mF = 'TP_max_corr2_Apex'
        self.s2s = 'TP_sum_corr2'
        self.s2n = 'TP_n_corr2'
        self.s2mp = 'TP_max_corr2_pvalue'
        self.s2ms = 'TP_max_corr2_score'
        self.s2avg = 'TP_all_corr2_avg'
        self.s2sp = 'TP_sum_corr2_pvalue'
        self.s2ss = 'TP_sum_corr2_score'
        self.s2 = 'TP_corr2_score'

        #self.s2adduct = 'TP_corr2_score_adduct'
        self.s2argmax = 'TP_corr2_argmax'
        self.s2argmaxs = 'TP_corr2_score_argmax'
        self.s2argmaxp = 'TP_Class_argmax'
        #self.s2argmaxA = 'TP_Class_Adduct_argmax'

        self.s3Ai = 'TP_Adduct_Index'
        self.s3An = 'TP_Adduct_Number'
        self.s3s = 'TP_Adduct_Score'

        self.s2s3 = 'TP_corr2_adduct_Score'

        self.s = 'TP_corr12_score_argmax'
        self.se = 'TP_error_penalty'
        self.sfinal = 'TPMetrics'

        self.rank = 'TPMetrics_rank'
        self.nFilt = 'Name_filtered'

        # Name of input and output files
        
        self.infile = self.config['TPMetrics']['infile']
        self.outfile = self.config['TPMetrics']['outfile']

        
        # Get parameters

        self.rt1 = self.config['TPMetrics'].getfloat('rt1')
        self.rt2 = self.config['TPMetrics'].getfloat('rt2')
        #self.ipatt = re.compile(self.config['TPMetrics'].get('i_pattern'))
        self.corrType = self.config['TPMetrics'].get('corr_type')

        self.L2A = json.loads(self.config['TPMetrics']['class_adducts']) # From lipid to list of possible adducts
        self.L = list(self.L2A.keys())
        self.L2An = {i:len(self.L2A[i]) for i in self.L2A} # From lipids to number of possible adducts
        
        # From lipid to dictionary mapping its possible adduct to its index
        self.L2A2i = {i:{j: n+1 for n,j in enumerate(self.L2A[i])} for i in self.L2A}


        # CMM columns moved to the end of the table
        self.cmmCol = ['Score 1','Score 2','Score 3','Final Score','CAS','Kegg','HMDB','LipidMaps','Metlin','PubChem','InChIKey','SMILES','Pathways']