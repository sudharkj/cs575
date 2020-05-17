#!/bin/bash

# remove data folder
rm -rf data
# create data folder
mkdir -p data

# create empty file
touch data/simulations.csv
# add header
echo "trials,size,performance,probability" >data/simulations.csv

# go to source
cd src || exit
# run the simulations
for TRIALS_POW in {4..10}; do
  # calculate array size
  TRIALS=$((2**TRIALS_POW))
  TRIALS=$((1024*TRIALS))

  # cores with or without simd
  for SIZE_POW in {4..7}; do
    # calculate array size
    SIZE=$((2**SIZE_POW))
    make --always-make all NUMTRIALS="${TRIALS}" BLOCKSIZE="${SIZE}"
    ./proj >>../data/simulations.csv
    make clean
  done
done

# plot and save the graphs
python analyze_results.py

# move to report directory
cd ../latex || exit
rm -f *.pdf
# generate report
pdflatex -jobname=CUDA\ Monte\ Carlo\ Ravi report.tex
pdflatex -jobname=CUDA\ Monte\ Carlo\ Ravi report.tex
# remove generated files
rm -f *.aux *.log *.gz
