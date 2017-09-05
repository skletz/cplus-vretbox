#!/bin/bash
# ¯\_(ツ)_/¯
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#	moshtoo.sh
# A script to copy mastershots with a specific criteria.
#
# Inputs:
# 1) Input CSV file containing mastershots which have to be copied.
# 2) Source directory containing the mastershots files
# 3) Destination directory of copy
#
# CSV file: Each line indicate a master shot entry in the following format:
# <filename>
#
# Enable globstar option (shopt -s globstar)
#
# @author skletz
# @version 1.0 01/09/17
# -----------------------------------------------------------------------------
# @TODO:
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

USAGE="A script to copy mastershots with a specific criteria.
Usage: `basename $0` [-infile] [-outfile] [-srv]
    -h    Show help
    -i    Input CSV file containing mastershots which have to be copied.
    -s    Source directory containing the mastershots files
    -d    Destination directory of copy

 Examples:
    bash `basename $0` -i ../testdata/filtered-mastershots.csv -s ../testdata/mastershots/all -d ../testdata/mastershots/filtered"

#INPUTS
INFILE=""
SRCDIR=""
DESTDIR=""

# parse command line
if [ $# -eq 0 ]; then #  must be at least one arg
    echo "$USAGE" >&2
    exit 1
fi

while getopts i:d:s:h OPT; do
    case $OPT in
    h)  echo "$USAGE"
        exit 0 ;;
    i)  INFILE=$OPTARG ;;
    s)  SRCDIR=$OPTARG ;;
    d)  DESTDIR=$OPTARG ;;
    \?) # getopts issues an error message
        echo "$USAGE" >&2
        exit 1 ;;
    esac
done
shift `expr $OPTIND - 1`

printf "%-30s %s\n" "input file :"  "$INFILE"
printf "%-30s %s\n" "source directory :"  "$DESTDIR"
printf "%-30s %s\n" "destination directory :"  "$SRCDIR"


# enable globstar options (shopt -s globstar)
file=$INFILE
filename=$(basename "$file")
dirname="$(basename "$(dirname "$file")")"

total=$(wc -l < "$file")
pstr="[=======================================================================]"
count=0
#iterate through csv file
IFS=","
[ ! -f $file ] && { echo "Error: $file file not found!"; exit 99; }
while read filename
do
  #Progress bar
  count=$(( $count + 1 ))
  pd=$(( $count * 73 / $total ))
  printf "\r%3d.%1d%% %.${pd}s" $(( $count * 100 / $total )) $(( ($count * 1000 / $total) % 10 )) $pstr
  #echo "cp $SRCDIR/$filename $DESTDIR/"
  cp $SRCDIR/$filename $DESTDIR/
  # mv $SRCDIR/$basename.bin $DESTDIR/
done < $file
