#!/usr/bin/env python

# -*- coding: utf-8 -*-

#
# IMPORT MODULES
#

import configparser
import json
import logging
import numpy as np
import os
import pandas as pd
import re

'''
2) Obtener conujunto de clases de cada elemento
3) Esta clase lee outfile  (4_tablemerger y eso)
4) Usar notebook para ir desarrollando la detección de clases de forma eficiente
'''


#
# CLASSES AND FUNCTIONS
#

class TPMetrics:

    def __init__(self, workdir):

        logging.info('Initializing TPMetrics object')
        
        self.workdir = workdir
        self.config = configparser.ConfigParser()
        self.config.read(os.path.join(self.workdir, 'configFile.ini'))

        # column names
        self.m = self.config['TPMetrics']['column_mass']
        self.n = self.config['TPMetrics']['column_name']
        self.rt = self.config['TPMetrics']['column_rt']
        self.a = self.config['TPMetrics']['column_adduct']
        self.w = self.config['TPMetrics']['column_molweight']
        self.e = self.config['TPMetrics']['column_error']

        self.tpc = 'TP_Class'
        self.s1a = 'TP_all_corr1'
        self.s1m = 'TP_max_corr1'
        self.s1s = 'TP_sum_corr1'
        self.s1n = 'TP_n_corr1'

        self.s2a = 'TP_all_corr2'
        self.s2m = 'TP_max_corr2'
        self.s2s = 'TP_sum_corr2'
        self.s2n = 'TP_n_corr2'

        # files
        self.infile = self.config['TPMetrics']['infile']
        self.outfile = self.config['TPMetrics']['outfile']

        # parameters
        self.rt1 = self.config['TPMetrics'].getfloat('rt1')
        self.rt2 = self.config['TPMetrics'].getfloat('rt2')
        self.A = json.loads(self.config['TPMetrics']['class_adducts'])
        self.ipatt = re.compile(self.config['TPMetrics'].get('i_pattern'))
        self.corrType = self.config['TPMetrics'].get('corr_type')

        # Working table
        self.df = pd.read_csv(os.path.join(self.workdir, self.infile), sep='\t').reset_index() # Generate column with index 
        self.LC = self.readLipidClasses()


        # Other values

        # column names with intensities
        self.i = [i for i in self.df.columns if self.ipatt.search(i)] 
        
        # correlation matrix between mz
        self.corr = self.df.loc[:, [self.m, *self.i]].drop_duplicates().set_index(self.m).T.corr(method=self.corrType) 
        
        # map index to mz
        self.idx2mz = dict(list(zip(
            self.df.index.to_list(),
            self.df.loc[:, self.m].to_list()
        )))

        # Array containing correlation values under the null hypothesis; getNullH()
        self.VcorrH0 = np.array([])

        logging.info('TPMetrics object initialized')


    def readLipidClasses(self):
        '''
        Read list containing classes of lipids
        '''

        #with open(r'./src/TurboPutative-2.0-built/TPProcesser/TPMetrics/data/Lipid_Class.tsv', 'r') as f:
        with open(r'./data/Lipid_Class.tsv', 'r') as f:
            return pd.DataFrame({'TP_Class': [i.strip() for i in f]})


    def getClasses(self):
        '''
        Add to input table lipid classes (found in LMSD)
        '''

        self.df = self.df.join(
            pd.merge(
                pd.DataFrame(
                    {
                        self.tpc: [
                            [re.sub(r'LMSD{ ([^}]+) }', r'\1', j).split(' ')[0] for j in i.split(' // ')]
                            for i in self.df[self.n].to_list()
                        ],
                        'index': range(self.df.shape[0])
                    }
                ).explode(self.tpc), # Dataframe with (potential) lipid class and index
                self.LC, # Dataframe with permited lipid classes
                on=self.tpc,
                how='inner'
            ).drop_duplicates().groupby('index').agg(list), # Dataframe with permitted lipid classes associated to each index
        )

        self.df[self.tpc] = self.df.loc[:, self.tpc].str.join(' // ')

    
    def getCorrelations(self):
        '''
        '''
        
        self.getCorr(basedCol=self.w, dtypeCol=np.float64, sa=self.s1a, sm=self.s1m, ss=self.s1s, sn=self.s1n)
        self.getCorr(basedCol=self.tpc, dtypeCol=str, sa=self.s2a, sm=self.s2m, ss=self.s2s, sn=self.s2n)
        self.getNullH()


    def getCorr(self, basedCol, dtypeCol, sa, sm, ss, sn):
        '''
        Correlations based on basedCol. Try to identify the same compound in different features. 
        Three columns with correlations will be added to self.df:
            - sa: [[],] List containing list of correlations (one per each elem in elem1 // elem2... of basedCol)
            - sm: [] List containing the maximum correlation in each list of sa
            - ss: [] List containing the sum of all correlations in each list of sa
        '''
        
        # Get a dataframe copy unfolding by molecular weight (preserving the same index after unfold)
        df = self.df.loc[:, ['index',self.m, self.rt, basedCol]].copy()
        df[basedCol] = self.df[basedCol].str.split(' // ')
        df = df.explode(
            basedCol, ignore_index=True
            ).dropna(
                axis=0, subset=[basedCol]
                ).astype(
                    {basedCol: dtypeCol}
                    ).drop_duplicates(
                        subset=[self.m, self.rt, basedCol], keep='first'
                        )

        # Convert df to list of lists [(index, mz, rt, mw)]
        dfl = list(zip(*[j for i,j in df.to_dict('list').items()]))

        # Identify possible correlated elements [(index, [pair_index_1, pair_index_2])]
        idx2p = [
            ([index, w],
            df.loc[
                np.logical_and.reduce((
                    m != df.loc[:, self.m],
                    rt-self.rt1 <= df.loc[:, self.rt],
                    rt+self.rt1 >= df.loc[:, self.rt],
                    w == df.loc[:, basedCol]
                )),
                'index'
            ].to_list())
            for index, m, rt, w in dfl
        ]

        # remove empty elements and replace indexes by correlation
        idx2p = [
            (
                index_w, 
                self.corr.loc[self.idx2mz[index_w[0]], [self.idx2mz[p] for p in pair]].to_list()
            ) 
            for index_w, pair in idx2p if len(pair)>0
        ]

        # add maximum and sum; build dataframe
        idx2p = pd.DataFrame(
            [
                [*index_w, pair, pair[np.argmax(np.abs(pair))], np.sum(np.abs(pair)), len(pair)]
                for index_w, pair in idx2p
            ], 
            columns=['index', basedCol, sa, sm, ss, sn]
        )

        # Add correlations to each unfolded element; and group by index (refold)
        df = pd.merge(
            df,
            idx2p,
            on=['index', basedCol],
            how='left'
        ).groupby('index').agg(list).reset_index()

        # Merge to original table based on index (could be a join)
        self.df = pd.merge(
            self.df,
            df.loc[:, ['index', sa, sm, ss, sn]],
            on='index',
            how='left'
        )


    def getNullH(self):
        '''
        Get correlations under the null hypothesis of no correlation
        This will be used to calculate empirical p-values
        '''

        # Get maximum number of correlations associated to an annotation
        maxn = self.df.loc[:, self.s1n].dropna().to_list()+self.df.loc[:, self.s2n].dropna().to_list()
        maxn = [[j for j in i if not pd.isna(j)] for i in maxn]
        maxn = int(max([max(i) for i in maxn if i!=[]]))

        # Get number of shuffling
        arr = self.df.loc[:, self.i].drop_duplicates().to_numpy()
        T = 100000 # Number of correlation values
        K = (2 * T)/(arr.shape[0]**2-arr.shape[0])
        K = int(np.ceil(K))

        # Convert matrix intensity into a vector; shuffle the vector; rebuild the matrix
        arrl = [arr.copy().reshape(arr.shape[0]*arr.shape[1]) for i in range(K)]
        _ = [np.random.shuffle(i) for i in arrl]
        arrl = [i.reshape(arr.shape) for i in arrl]

        # Calculate correlations
        Mcorr = [pd.DataFrame(i).T.corr(method=self.corrType).to_numpy() for i in arrl]

        # Get upper triangle of correlation matrix
        idx = np.triu_indices(arr.shape[0], 1)
        Vcorr = np.concatenate([M[idx[0], idx[1]] for M in Mcorr])

        # Get correlation distribution considering the addition
        VcorrH0 = [np.abs(Vcorr)]
        for n in range(2, maxn+1):
            Vcorrl = [Vcorr.copy() for i in range(n)]
            _ = [np.random.shuffle(i) for i in Vcorrl]
            VcorrH0.append(np.sum(np.abs(Vcorrl), axis=0))
        
        self.VcorrH0 = np.array(VcorrH0)