#!/bin/bash
# ¯\_(ツ)_/¯
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#	searchmissingfeatures.sh
# searches for missing extracted features
# INPUT: path/to/videodirectory path/to/extracted/featuers outputfile
# @author skletz
# @version 1.0 11/07/07
# -----------------------------------------------------------------------------
# @TODO:
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

INDIR=$1
FEATURES=$2
OUTFILE=$3

echo "Video Input directory: " $INDIR
echo "Features Input directory: " $FEATURES
echo "Output file: " $OUTFILE

find "${INDIR}/" -type f -name '*.mp4' -print0 | while IFS= read -r -d '' file; do

    group="$(basename "$(dirname "$file")")"
    filename=$(basename "$file")
    filename=${filename%.*}

    searchedfile=$FEATURES/$filename.txt
    echo "Search file " $searchedfile
    if [[ -f $searchedfile ]]
    then
        echo "File found!"
    else
      echo "File not found!"
      echo $group,$filename >> $OUTFILE
    fi
done
