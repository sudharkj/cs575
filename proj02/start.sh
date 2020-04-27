#!/bin/bash

# remove data folder
rm -rf data
# create data folder
mkdir -p data/input
mkdir -p data/output

# run the simulations
for THREADS in 1 2 4 8; do
  for NODES in 1000 2000 3000 4000 5000 6000 7000 8000 9000 10000; do
    make --always-make all NUMT="${THREADS}" NUMNODES="${NODES}"
    ./proj >data/input/results_"${THREADS}"_"${NODES}".csv
    make clean
  done
done

# plot and save the graphs
python analyze_results.py
