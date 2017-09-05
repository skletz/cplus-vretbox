#!/bin/bash
# ¯\_(ツ)_/¯
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#	xtract-one-video.sh
# This script uses the program vretbox
#
# @author skletz
# @version 1.0 11/07/07
# -----------------------------------------------------------------------------
# @TODO:
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

USAGE="A script for extracting content features from the surgical data dataset using vretbox.
Usage: `basename $0` [-infile] [-outfile] [-srv]
    -h    Shows help
    -i    Video-File <only tested with .MP4 files>
    -c    Config-File
    -o    The path to the output directory
    -q    Number of sample points used for clustering
    -w    Number of maximal cluster produced by k-means clustering
    -e    Number of maximal frames used for the extraction of dynamic features
    -r    Output directory of the visualization of the extracted features
 Examples:
    bash `basename $0` -i -c -o -q -w -e"

INFILE=""
CONFIGFILE=""
OUTFILE=""
SAMPLES=""
CENTROIDS=""
FRAMES=""
OUTDIR=""


# parse command line
if [ $# -eq 0 ]; then #  must be at least one arg
    echo "$USAGE" >&2
    exit 1
fi

while getopts i:c:o:q:w:e:r:h OPT; do
    case $OPT in
    h)  echo "$USAGE"
        exit 0 ;;
    i)  INFILE=$OPTARG ;;
    c)  CONFIGFILE=$OPTARG ;;
    o)  OUTFILE=$OPTARG ;;
    q)  SAMPLES=$OPTARG ;;
    w)  CENTROIDS=$OPTARG ;;
    e)  FRAMES=$OPTARG ;;
    r)  OUTDIR=$OPTARG ;;
    \?) # getopts issues an error message
        echo "$USAGE" >&2
        exit 1 ;;
    esac
done
shift `expr $OPTIND - 1`

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

IN_NAME=$(basename "$INFILE")
OUT_NAME=$(basename "$OUTFILE")
OUTFILE=$OUTFILE/${IN_NAME%.*}.txt

printf "%-20s %s\n" "input file :"  "$INFILE"
printf "%-20s %s\n" "config file :"   "$CONFIGFILE"
printf "%-20s %s\n" "output file :"   "$OUTFILE"
printf "%-20s %s\n" "init seeds :"   "$SAMPLES"
printf "%-20s %s\n" "initial centroids :"   "$CENTROIDS"
printf "%-20s %s\n" "max frames :"   "$FRAMES"
printf "%-20s %s\n" "outdir :"   "$OUTDIR"

echo "Execute:" $PROG --Cfg.fs.initSeeds $SAMPLES --Cfg.fs.initialCentroids $CENTROIDS --Cfg.dfs.maxFrames $FRAMES --infile $INFILE --outfile $OUTFILE --outdir $OUTDIR --config $CONFIGFILE

time $BIN/$PROG --Cfg.fs.initSeeds $SAMPLES --Cfg.fs.initialCentroids $CENTROIDS --Cfg.dfs.maxFrames $FRAMES --infile "$INFILE" --outfile "$OUTFILE" --outdir "$OUTDIR" --config "$CONFIGFILE"
