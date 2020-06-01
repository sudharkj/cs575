#!/bin/bash

# remove data folder
rm -rf data
# create data folder
mkdir -p data

# go to source
cd src || exit

make --always-make printinfo
./printinfo
echo

for i in 0,0,"multiply","Multiply" 1,0,"add","Multiply-Add" 0,1,"reduce","Multiply-Reduce"
do IFS=","
  set -- $i
  ADD="$1"
  REDUCE="$2"
  OPERATION="$3"
  TITLE="$4"
  # echo "$ADD", "$REDUCE", "$OPERATION", "$TITLE"

  # create empty file
  touch ../data/"$OPERATION".csv
  # add header
  echo "global,local,performance" >../data/"$OPERATION".csv

  # run the simulations
  for ARRAY_SIZE_POW in {10..23}
  do
    # calculate array size
    ARRAY_SIZE=$((2**ARRAY_SIZE_POW))

    # cores with or without simd
    for WORKER_SIZE_POW in {5..8}
    do
      # calculate worker size
      WORKER_SIZE=$((2**WORKER_SIZE_POW))
      make --always-make proj ADD="${ADD}" REDUCE="${REDUCE}" ARRAY_SIZE="${ARRAY_SIZE}" WORKER_SIZE="${WORKER_SIZE}"
      ./proj >>../data/"$OPERATION".csv
    done
  done

  # plot and save the graphs
  python analyze_results.py "$OPERATION $TITLE"
done

# clean executables
make clean

# move to report directory
cd ../latex || exit
rm -f *.pdf
# generate report
pdflatex -jobname=OpenCL\ Multiply\ Ravi report.tex
pdflatex -jobname=OpenCL\ Multiply\ Ravi report.tex
# remove generated files
rm -f *.aux *.log *.gz
