#!/bin/bash

# remove data folder
rm -rf data
# create data folder
mkdir -p data

cd src || exit

# run the simulation
make --always-make all
./proj >../data/results.csv
make clean

# plot and save the graphs
python analyze_results.py

# move to report directory
cd ../latex || exit
# generate report
pdflatex -jobname=Functional\ Decomposition report.tex
pdflatex -jobname=Functional\ Decomposition report.tex
# remove generated files
rm -f *.aux *.log *.gz
