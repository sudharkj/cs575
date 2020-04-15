#!/bin/bash

g++ -DNUMT=1 proj.cpp -o proj -lm -fopenmp
ONE_OUTPUT=$(./proj)
ONE_EXIT_CODE=$?
echo "${ONE_OUTPUT}"
rm proj

g++ -DNUMT=4 proj.cpp -o proj -lm -fopenmp
FOUR_OUTPUT=$(./proj)
FOUR_EXIT_CODE=$?
echo "${FOUR_OUTPUT}"
rm proj

if [ "${ONE_EXIT_CODE}" = "0" ] && [ "${FOUR_EXIT_CODE}" = "0" ]; then
  ONE_PERF=$(echo "$ONE_OUTPUT" | grep -Eo '[+-]?[0-9]+([.][0-9]+)?')
  FOUR_PERF=$(echo "$FOUR_OUTPUT" | grep -Eo '[+-]?[0-9]+([.][0-9]+)?')

  SPEEDUP=$(bc <<< "scale=2; $FOUR_PERF / $ONE_PERF")
  printf "4-Thread SpeedUp = %2.2lf\n" "${SPEEDUP}"

  PAR_FRAC=$(bc <<< "scale=2; 4 * (1 - (1/$SPEEDUP)) / 3")
  printf "Parallel Fraction = %2.2lf\n" "${PAR_FRAC}"
fi
