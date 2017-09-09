#!/bin/bash
# ¯\_(ツ)_/¯
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#	xtract-all-videos.sh
# A script for extracting features from a set of videos.
# This script uses xtract-one-video.sh
# Dependencies:
#   export VRET_EVAL=/path/to/output
#   export VRET_DATA=/path/to/input
# @author skletz
# @version 1.0 09/06/17
# -----------------------------------------------------------------------------
# @TODO:
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

printf "%-20s %s\n" "input directory of all videos :"  "$VRET_DATA"
printf "%-20s %s\n" "output directory of all evaluation files :"  "$VRET_EVAL"

timestamp=$(date +"%Y-%m-%d_%H-%M-%S")
LOGFILE="$VRET_EVAL"/logs/hog-xtraction-$timestamp.out

echo "LOGFILE: " $LOGFILE

#CONFIGFILE="$VRET_EVAL"/configs/Sig_Xtract_Test.ini
CONFIGFILE=../testdata/config-files/HOG-Extraction.ini

#Settings
samplepoints=(8000)
initCentroids=(90)
framecnt=(2)
#framecnt=(10 15 20 25 30 50 125)
echo "Starting ..."
for samples in "${samplepoints[@]}"; do

  for centroids in "${initCentroids[@]}"; do

    for frames in "${framecnt[@]}"; do
      echo "---------------------------------------------------------" >> "${LOGFILE}"
      start_timestamp=$(date +"%Y-%m-%d_%H-%M-%S.%3N")
      echo "START: "$samples","$centroids","$frames","$start_timestamp >> "${LOGFILE}"
      echo "---------------------------------------------------------" >> "${LOGFILE}"
      time find "$VRET_DATA"/ -type f -name *.mp4 -print0 | xargs -0 -I {} -P 21 bash xtract-one-video.sh -i {} -o "$VRET_EVAL"/features -c $CONFIGFILE -q $samples -w $centroids -e $frames -r "$VRET_EVAL"/presentation 2>&1 | tee -a "${LOGFILE}"
      #read -p "Press enter to continue"
      echo "---------------------------------------------------------" >> "${LOGFILE}"
      end_timestamp=$(date +"%Y-%m-%d_%H-%M-%S.%3N")
      echo $end_timestamp
      echo "END: "$samples","$centroids","$frames","$end_timestamp >> "${LOGFILE}"
      echo "---------------------------------------------------------" >> "${LOGFILE}"
    done

  done

done

echo "Finished ..."
