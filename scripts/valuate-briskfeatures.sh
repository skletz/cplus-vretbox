#!/bin/bash
# ¯\_(ツ)_/¯
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#	valuate-features.sh
# A script for evaluating set of features.
# Dependencies:
#   export VRET_EVAL=/path/to/output
#   export VRET_DATA=/path/to/input
# @author skletz
# @version 1.0 29/08/17
# -----------------------------------------------------------------------------
# @TODO:
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

printf "%-20s %s\n" "input directory of all features :"  "$VRET_DATA"
printf "%-20s %s\n" "output directory of all evaluation files :"  "$VRET_EVAL"

timestamp=$(date +"%Y-%m-%d_%H-%M-%S")
LOGFILE="$VRET_EVAL"/logs/brisk-evaluation-$timestamp.out

echo "LOGFILE: " $LOGFILE

#CONFIGFILE="$VRET_EVAL"/configs/Sig_Xtract_Test.ini
CONFIGFILE=../testdata/config-files/BRISK-Evaluation.ini

#Default
BIN="builds/linux/bin"
PROG="progvretbox.1.0"

if [ "$(uname)" == "Darwin" ]; then
  echo "Target: Darwin"
  BIN="../builds/mac/bin"
  PROG="progvretbox.1.0"
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
  echo "Target: Linux"
  BIN="../builds/linux/bin"
  PROG="progvretbox.1.0"
else
  echo "Target: Windows?"
  BIN="../builds/win/bin"
  PROG="vretbox.exe"
fi

MAXTHREADS=21

#Settings
#atKs=(0 1000 100 10 9 8 7 6 5 4 3 2 1)#
atKs=(0)
echo "Starting ..."

for atK in "${atKs[@]}"; do
  echo "---------------------------------------------------------" >> "${LOGFILE}"
  start_timestamp=$(date +"%Y-%m-%d_%H-%M-%S.%3N")
  echo "START: "$atK","$start_timestamp >> "${LOGFILE}"
  echo "---------------------------------------------------------" >> "${LOGFILE}"

  echo --maxThreads $MAXTHREADS --General.atK $atK --config "$CONFIGFILE"
  time $BIN/$PROG --maxThreads $MAXTHREADS --General.atK $atK --config "$CONFIGFILE" 2>&1 | tee -a "${LOGFILE}"

  echo "---------------------------------------------------------" >> "${LOGFILE}"
  end_timestamp=$(date +"%Y-%m-%d_%H-%M-%S.%3N")
  echo $end_timestamp
  echo "END: "$atK","$end_timestamp >> "${LOGFILE}"
  echo "---------------------------------------------------------" >> "${LOGFILE}"
done

echo "Finished ..."
