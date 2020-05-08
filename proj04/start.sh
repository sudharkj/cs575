#!/bin/bash

# remove data folder
rm -rf data
# create data folder
mkdir -p data

# create empty file
touch data/results.csv
# add header
echo "simd,core,size,sum,performance" >data/results.csv

# go to source
cd src || exit
# run the simulations
for POW in {10..23}; do
  # calculate array size
  SIZE=$((2**POW))

  # simple loop
  make --always-make all ARRAYSIZE="${SIZE}"
  ./proj >>../data/results.csv
  make clean

  # simd alone
  make --always-make all USESIMD=1 ARRAYSIZE="${SIZE}"
  ./proj >>../data/results.csv
  make clean

  # cores with or without simd
  for SIMD in 0 1; do
    for CORES in 1 2 4 8; do
      make --always-make all USESIMD="${SIMD}" USECORE="${CORES}" ARRAYSIZE="${SIZE}"
      ./proj >>../data/results.csv
      make clean
    done
  done
done

# plot and save the graphs
python analyze_results.py

# move to report directory
cd ../latex || exit
rm -f *.pdf
# generate report
pdflatex -jobname=Vectorized\ Operations report.tex
pdflatex -jobname=Vectorized\ Operations report.tex
# remove generated files
rm -f *.aux *.log *.gz
