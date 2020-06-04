#!/bin/bash

# remove data folder
rm -rf data
# create data folder
mkdir -p data

# go to source
cd src || exit

# show opencl device and platform details
make --always-make printinfo
./printinfo
echo

# create empty file
touch ../data/simulations.csv
# add header
printf "mode,param,performance" >../data/simulations.csv
for SHIFT in {1..512}; do
  printf ",%d" "$SHIFT" >>../data/simulations.csv
done
printf "\n" >>../data/simulations.csv

# simulate non parallel
make --always-make proj MODE=0
./proj >>../data/simulations.csv

# simulate open-mp
for NUMT_POW in {0..10}; do
  NUMT=$((2 ** NUMT_POW))
  make --always-make proj MODE=1 NUMT="${NUMT}"
  ./proj >>../data/simulations.csv
done

# simulate cpu-simd
make --always-make proj MODE=2
./proj >>../data/simulations.csv

# simulate simd with open-mp
for NUMT_POW in {0..10}; do
  NUMT=$((2 ** NUMT_POW))
  make --always-make proj MODE=3 NUMT="${NUMT}"
  ./proj >>../data/simulations.csv
done

# simulate open-cl
for LOCAL_SIZE_POW in {0..10}; do
  LOCAL_SIZE=$((2 ** LOCAL_SIZE_POW))
  make --always-make proj MODE=4 LOCAL_SIZE="${LOCAL_SIZE}"
  ./proj >>../data/simulations.csv
done

# clean executables
make clean

# plot and save the graphs
python analyze_results.py

# move to report directory
cd ../latex || exit
rm -f *.pdf
# generate report
pdflatex -jobname=Autocorrelation\ Ravi report.tex
pdflatex -jobname=Autocorrelation\ Ravi report.tex
# remove generated files
rm -f *.aux *.log *.gz
