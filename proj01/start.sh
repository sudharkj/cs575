#!/bin/bash

# remove data folder
rm -rf data
# create data folder
mkdir -p data/input
mkdir -p data/output

# run the simulations
for TRIALS in 0 500000 1000000 1500000 2000000 2500000; do
  for THREADS in 1 2 3 4; do
    g++ -DNUMTRIALS="${TRIALS}" -DNUMT="${THREADS}" proj.cpp -o proj -lm -fopenmp
    ./proj >data/input/results_"${TRIALS}"_"${THREADS}".csv
    rm proj
  done
done

# plot and save the graphs
python analyze_results.py
