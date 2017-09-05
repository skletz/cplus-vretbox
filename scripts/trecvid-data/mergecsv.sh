#!/bin/bash
# ¯\_(ツ)_/¯
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#	mergecsv.sh
# A script to merge csv files
#
# Inputs: 1) List of CSV files to merge
# Outputs:  1) CVS file
#
# @author skletz
# @version 1.0 18/05/17
# -----------------------------------------------------------------------------
# @TODO:
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

INPUT=""
OUTPUT=""

USAGE="A tool for dividing a video into shots.
Usage: `basename $0` [-csv] [-shots]
    -h    Shows this help
    -i    Input files csv
    -o    Output file csv
 Examples:
    `bash basename $0` -i file1.csv -i file2.csv -o file-merged.csv"

# parse command line
if [ $# -eq 0 ]; then #  must be at least one arg
    echo "$USAGE" >&2
    exit 1
fi

while getopts i:o:h OPT; do
    case $OPT in
    h)  echo "$USAGE"
        exit 0 ;;
    i)  INPUT+=("$OPTARG") ;;
    o)  OUTPUT=$OPTARG ;;
    \?) # getopts issues an error message
        echo "$USAGE" >&2
        exit 1 ;;
    esac
done
shift `expr $OPTIND - 1`

echo "Input files: ${INPUT[@]}"
echo "Output file: $OUTPUT"

IFS=","
counter=0
for val in "${INPUT[@]}"; do
    #skip the first entry in ${INPUT[@]} (however it is empty)
    if (($(bc <<< "$counter == 0"))); then
      counter=$((counter+1))
      continue
    fi

    [ ! -f $val ] && { echo "$val file not found"; exit 99; }
    while read -r line
    do
      echo -e "DEGUB:\t$line"
      echo "$val,$line" >> $OUTPUT
      counter=$((counter+1))
    done < $val
done

echo "Sum of shots: $counter"
