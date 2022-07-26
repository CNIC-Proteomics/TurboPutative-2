#!/bin/bash

# Description: Script to create a folder with required files for execution

BUILD_DIR_NAME="../../../TurboPutative-web/src/TurboPutative-2.0-built"

rm -r $BUILD_DIR_NAME
mkdir $BUILD_DIR_NAME

cp TPWrapper.py $BUILD_DIR_NAME/
cp errorCode.json $BUILD_DIR_NAME/

# Copy ppGenerator
mkdir -p $BUILD_DIR_NAME/ppGenerator/pendingFiles
cp ./ppGenerator/ppGeneratorWrapper.py $BUILD_DIR_NAME/ppGenerator/ppGeneratorWrapper.py
cp ./ppGenerator/preProcessedNames.py $BUILD_DIR_NAME/ppGenerator/preProcessedNames.py
cp ./ppGenerator/ppGenerator1 $BUILD_DIR_NAME/ppGenerator/ppGenerator1
cp ./ppGenerator/ppGenerator2 $BUILD_DIR_NAME/ppGenerator/ppGenerator2

# Copy TPPreProcesser
mkdir $BUILD_DIR_NAME/TPPreProcesser 
cp ./TPPreProcesser/PreProcesser.py $BUILD_DIR_NAME/TPPreProcesser/PreProcesser.py
cp -r ./TPPreProcesser/modules $BUILD_DIR_NAME/TPPreProcesser
rm -r $BUILD_DIR_NAME/TPPreProcesser/modules/__pycache__

# Copy TPPostProcesser
mkdir $BUILD_DIR_NAME/TPPostProcesser 
cp ./TPPostProcesser/PostProcesser.py $BUILD_DIR_NAME/TPPostProcesser/PostProcesser.py
cp -r ./TPPostProcesser/modules $BUILD_DIR_NAME/TPPostProcesser
rm -r $BUILD_DIR_NAME/TPPostProcesser/modules/__pycache__

# Copy TPProcesser
mkdir $BUILD_DIR_NAME/TPProcesser

# Tagger
mkdir $BUILD_DIR_NAME/TPProcesser/Tagger 
cp ./TPProcesser/Tagger/Tagger $BUILD_DIR_NAME/TPProcesser/Tagger/
cp -r ./TPProcesser/Tagger/data $BUILD_DIR_NAME/TPProcesser/Tagger/

# REname
mkdir $BUILD_DIR_NAME/TPProcesser/REname
cp ./TPProcesser/REname/REname1 $BUILD_DIR_NAME/TPProcesser/REname/
cp ./TPProcesser/REname/REname2 $BUILD_DIR_NAME/TPProcesser/REname/
cp -r ./TPProcesser/REname/data $BUILD_DIR_NAME/TPProcesser/REname/
mkdir -p $BUILD_DIR_NAME/TPProcesser/REname/lib/cppgoslin-1.1.2
cp ./TPProcesser/REname/lib/cppgoslin-1.1.2/TPGoslin $BUILD_DIR_NAME/TPProcesser/REname/lib/cppgoslin-1.1.2/
#cp -r ./TPProcesser/REname/lib/cppgoslin-1.1.2/lib $BUILD_DIR_NAME/TPProcesser/REname/lib/cppgoslin-1.1.2/ 

# RowMerger
mkdir $BUILD_DIR_NAME/TPProcesser/RowMerger
cp ./TPProcesser/RowMerger/RowMerger $BUILD_DIR_NAME/TPProcesser/RowMerger/

# TableMerger
mkdir $BUILD_DIR_NAME/TPProcesser/TableMerger
cp ./TPProcesser/TableMerger/TableMerger $BUILD_DIR_NAME/TPProcesser/TableMerger/
# cp ./TPProcesser/TableMerger/libm.so.6 $BUILD_DIR_NAME/TPProcesser/TableMerger/

# TPMetrics
mkdir -p $BUILD_DIR_NAME/TPProcesser/TPMetrics/modules
mkdir -p $BUILD_DIR_NAME/TPProcesser/TPMetrics/data

cp ./TPProcesser/TPMetrics/TPMetrics.py $BUILD_DIR_NAME/TPProcesser/TPMetrics/TPMetrics.py
cp ./TPProcesser/TPMetrics/modules/*.py $BUILD_DIR_NAME/TPProcesser/TPMetrics/modules/
cp ./TPProcesser/TPMetrics/data/* $BUILD_DIR_NAME/TPProcesser/TPMetrics/data/
