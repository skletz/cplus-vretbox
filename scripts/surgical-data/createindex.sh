#!/bin/bash
# ¯\_(ツ)_/¯
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#	createIndex.sh
# Create index for video-data (project structures matchingCriteria/videos)
# INPUT: /path/to/videodirectory outputfile
# @author skletz
# @version 1.0 11/07/07
# -----------------------------------------------------------------------------
# @TODO:
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

INDIR=$1
OUTFILE=$2

echo "Input directory: " $INDIR
echo "Output file: " $OUTFILE

find "${INDIR}/" -type f -name '*.mp4' -print0 | while IFS= read -r -d '' file; do
    group="$(basename "$(dirname "$file")")"
    filename=$(basename "$file")
    filename=${filename%.*}
    echo $file
    echo $group,$filename >> $OUTFILE
done
