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
        self.tpcA = 'TP_Class_adduct'

        self.s1a = 'TP_all_corr1'
        self.s1m = 'TP_max_corr1'
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
        self.s2m = 'TP_max_corr2'
        self.s2s = 'TP_sum_corr2'
        self.s2n = 'TP_n_corr2'
        self.s2mp = 'TP_max_corr2_pvalue'
        self.s2ms = 'TP_max_corr2_score'
        self.s2avg = 'TP_all_corr2_avg'
        self.s2sp = 'TP_sum_corr2_pvalue'
        self.s2ss = 'TP_sum_corr2_score'
        self.s2 = 'TP_corr2_score'

        self.s2adduct = 'TP_corr2_score_adduct'
        self.s2argmax = 'TP_corr2_argmax'
        self.s2argmaxs = 'TP_corr2_score_argmax'
        self.s2argmaxp = 'TP_Class_argmax'
        self.s2argmaxA = 'TP_Class_Adduct_argmax'

        self.s = 'TP_corr12_score_argmax'
        self.se = 'TP_error_penalty'
        self.sfinal = 'TPMetrics'

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
        self.df = pd.read_csv(os.path.join(self.workdir, self.infile), sep='\t')
        self.LC = self.readLipidClasses()

        self.initCols = self.df.columns.to_list()
        self.finalCols = [self.tpc, self.s2argmaxp, self.s2argmaxA, self.sfinal, self.s1, self.s2] +self.initCols
        self.df = self.df.reset_index() # Generate column with index 


        # Other values

        # column names with intensities
        #self.i = [i for i in self.df.columns if self.ipatt.search(i)] 
        self.i = self.config['TPMetrics']['column_intensities'].split(' // ')
        
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

        with open(r'./src/TurboPutative-2.0-built/TPProcesser/TPMetrics/data/Lipid_Class.tsv', 'r') as f:
        #with open(r'./data/Lipid_Class.tsv', 'r') as f:
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
        
        self.getScoreMax(sim=self.s1m, simp=self.s1mp, sims=self.s1ms)
        self.getScoreMax(sim=self.s2m, simp=self.s2mp, sims=self.s2ms)
        
        self.getScoreSum(sia=self.s1a, sis=self.s1s, sin=self.s1n, siavg=self.s1avg, sisp=self.s1sp, siss=self.s1ss)
        self.getScoreSum(sia=self.s2a, sis=self.s2s, sin=self.s2n, siavg=self.s2avg, sisp=self.s2sp, siss=self.s2ss)
        
        self.combineScores()

        self.getMaximumScores()

        self.df[self.s] = self.df.loc[:, self.s1argmaxs].fillna(0)+self.df.loc[:, self.s2argmaxs].fillna(0)

        self.applyErrorPenalty()


    def getCorr(self, basedCol, dtypeCol, sa, sm, ss, sn):
        '''
        Correlations based on basedCol. Try to identify the same compound in different features. 
        Three columns with correlations will be added to self.df:
            - sa: [[],] List containing list of correlations (one per each elem in elem1 // elem2... of basedCol)
            - sm: [] List containing the maximum correlation in each list of sa
            - ss: [] List containing the sum of all correlations in each list of sa
        '''
        
        logging.info(f"Identify associated annotations based on {basedCol}")

        # Get a dataframe copy unfolding by molecular weight (preserving the same index after unfold)
        df = self.df.loc[:, ['index',self.m, self.rt, basedCol]].copy()
        df[basedCol] = self.df[basedCol].str.split(' // ')
        df = df.explode(
            basedCol, ignore_index=True
            ).dropna(
                axis=0, subset=[basedCol]
                ).astype(
                    {basedCol: dtypeCol}
                    )#.drop_duplicates(
                     #   subset=[self.m, self.rt, basedCol], keep='first'
                     #   )

        # This is the dataframe used to search associated features/annotations. One feature can have 
        # annotations with the same basedCol. We must remove the duplicates to avoid repeated intensities
        # and correlations
        df_wo_duplicates = df.drop_duplicates(subset=[self.m, self.rt, basedCol], keep='first')

        # Convert df to list of lists [(index, mz, rt, mw)]
        dfl = list(zip(*[j for i,j in df.to_dict('list').items()]))

        # Identify possible correlated elements [(index, [pair_index_1, pair_index_2])]
        idx2p = [
            ([index, w],
            df_wo_duplicates.loc[
                np.logical_and.reduce((
                    m != df_wo_duplicates.loc[:, self.m],
                    rt-self.rt1 <= df_wo_duplicates.loc[:, self.rt],
                    rt+self.rt1 >= df_wo_duplicates.loc[:, self.rt],
                    w == df_wo_duplicates.loc[:, basedCol]
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

        
        
        # --> Las correlaciones se capturan de la tabla original. Para el maximo y la suma se transforman en valores absolutos
        # --> A partir de ahí, la hipótesis nula la tenemos en valores absolutos.
        # 
        
        # pair must be a numpy array first; the convert to list when building the dataframe
        #
        ### !!!!! REMOVE NEGATIVE CORRELATIONS FOR MW CASE --> Solo tienes que indexar los positivos
        # if rmNeg: idx2p = [(index_w, pair[pair>0]) for index_w, pair in idx2p if sum(pair>0)>0]
        # 

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

        logging.info("Calculating correlations under the null hypothesis")

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



    def getScoreMax(self, sim, simp, sims):
        '''
        Get score based on maximum correlation
        '''

        logging.info(f"Calculating score based on maximum correlation: {sims}")

        df = self.df.loc[:, ['index', sim]].dropna().explode(sim).fillna(0)
        df[simp] = [(self.VcorrH0[0, :]>np.abs(i)).sum()/self.VcorrH0.shape[1] for i in df[sim].to_list()]
        df.loc[df[simp] == 0, simp] = 1/self.VcorrH0.shape[1]

        # FORMULA
        df[sims] = np.abs(df[sim]) * (-np.log10(df[simp]))

        self.df = pd.merge(
            self.df,
            df.groupby('index').agg(list).reset_index().loc[:, ['index', simp, sims]],
            on='index',
            how='left'
        )
    

    def getScoreSum(self, sia, sis, sin, siavg, sisp, siss):
        '''
        Get score based on all correlations
        '''

        logging.info("Calculating score based on sum of all correlations")

        df = self.df.loc[:, ['index', sia, sis, sin]].dropna().explode([sia, sis, sin]).fillna(0)
        df[siavg] = [np.mean(np.abs(i)) for i in df[sia]]

        df[sisp] = [
            1 if n==0 else (self.VcorrH0[int(n)-1,:]>s).sum()/self.VcorrH0.shape[1] 
            for s, n in zip(df[sis].to_list(), df[sin].to_list())
        ]

        df.loc[df[sisp] == 0, sisp] = 1/self.VcorrH0.shape[1]

        # FORMULA
        df[siss] = np.sqrt(df[sin]) * np.abs(df[siavg]) * (-np.log10(df[sisp]))

        self.df = pd.merge(
            self.df,
            df.groupby('index').agg(list).reset_index().loc[:, ['index', siavg, sisp, siss]],
            on='index',
            how='left'
        )


    def combineScores(self):
        '''
        Combine Max and Sum scores in molecular weight and lipid class
        '''

        logging.info("Combining scores based on maximum and addition of correlations")

        # Combine score based on molecular weight
        df = self.df.loc[:, ['index', self.s1ms, self.s1ss]].dropna().explode([self.s1ms, self.s1ss]).fillna(0)
        df[self.s1] = df[self.s1ms] + df[self.s1ss]

        self.df = pd.merge(
            self.df,
            df.groupby('index').agg(list).reset_index().loc[:, ['index', self.s1]],
            on='index',
            how='left'
        )

        # Combine scores from lipid class
        df = self.df.loc[:, ['index', self.a, self.tpc, self.s2ms, self.s2ss]].dropna()
        df[self.tpc] = df[self.tpc].str.split(' // ')
        df = df.explode([self.tpc,self.s2ms, self.s2ss])
        df[self.s2] = df[self.s2ms].fillna(0)+df[self.s2ss].fillna(0)

        df[self.tpcA] = [False if pd.isna(tpc) or tpc not in self.A.keys() else a in self.A[tpc] for a, tpc in list(zip(df[self.a].to_list(), df[self.tpc]))]
        f = np.ones_like(df[self.tpcA], dtype=float)

        # FORMULA
        f[df[self.tpcA]] = 1 + 0.25
        df[self.s2adduct] = df[self.s2]*f

        self.df = pd.merge(
            self.df,
            df.groupby('index').agg(list).reset_index().loc[:, ['index',self.s2, self.s2adduct, self.tpcA]],
            on='index',
            how='left'
        )

    def getMaximumScores(self):
        '''
        Get maximum score and their corresponding lipid class (and molecular weight)
        '''

        logging.info("Identify group of annotations providing the maximum score")

        # Get maximum score for molecular weight
        df = self.df.loc[:, ['index', self.w, self.s1]].dropna()

        dfl = list(zip(*[
            j 
            for i,j in df.to_dict('list').items()
        ]))

        dfl = [(index, w.split(' // '), s, np.argmax(s)) for index, w, s in dfl]

        dfl = pd.DataFrame([
            (index, w[argmax], s[argmax], argmax) 
            for index, w, s, argmax in dfl
        ], columns=['index', self.s1argmaxp, self.s1argmaxs, self.s1argmax])

        self.df = pd.merge(
            self.df,
            dfl,
            on='index',
            how='left'
        )


        # Get maximum for Lipid class
        df = self.df.loc[:, ['index', self.s2adduct, self.tpc, self.tpcA]].dropna()

        dfl = [
            (index, s, k.split(' // '), A, np.argmax(s)) 
            for index, s, k, A in list(zip(*[j for i,j in df.to_dict('list').items()]))
        ]

        dfl =[
            (index, s, k, A, argmax, s[argmax], k[argmax], A[argmax], np.array(s)==s[argmax]) 
            for index, s, k, A, argmax in dfl
        ]

        dfl = [ # if max score == 0 --> Find another with adduct == True
            (index, np.where(A)[0].tolist(), np.array(s)[A][0], ' // '.join(np.array(k)[A].tolist()), ' // '.join(np.array(A, dtype=str)[A].tolist()))
            if s_argmax==0 and A_argmax==False and np.sum(A)>0  else
            (index, np.where(s_argmax_bool)[0].tolist(), s_argmax, ' // '.join(np.array(k)[s_argmax_bool].tolist()), ' // '.join(np.array(A, dtype=str)[s_argmax_bool].tolist()))
            if s_argmax_bool.sum()>1  else
            (index, argmax, s_argmax, k_argmax, A_argmax) 
            for index, s, k, A, argmax, s_argmax, k_argmax, A_argmax, s_argmax_bool in dfl
        ]

        dfl = pd.DataFrame(
            dfl,
            columns = ['index', self.s2argmax, self.s2argmaxs, self.s2argmaxp, self.s2argmaxA]
        )

        self.df = pd.merge(
            self.df,
            dfl,
            on='index',
            how='left'
        )


    def applyErrorPenalty(self):
        '''
        Apply error penalty anf get final tpmetrics
        '''
        
        logging.info("Calculate final score and apply error penalty")

        df = self.df.loc[:, ['index', self.e, self.s]].dropna()

        B = 0.25 # Maximum percentage penalty
        Em = df[self.e].max() # Maximum error
        n = 2 # error penalty ~ error^n

        #FORMULA
        df[self.se] = (df[self.e]**2) * B / (Em**n)

        df[self.sfinal] = df[self.s] * (1 - df[self.se])

        self.df = pd.merge(
            self.df,
            df.loc[:, ['index', self.se, self.sfinal]],
            on='index',
            how='left'
        )
    
    def writeOutfile(self):
        '''
        '''
        outpath = os.path.join(self.workdir, self.outfile)
        logging.info(f"Writing output file: {outpath}")
        self.df.to_csv(
            outpath,
            sep='\t', index=False, columns=self.finalCols
        )