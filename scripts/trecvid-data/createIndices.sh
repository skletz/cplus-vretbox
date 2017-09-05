#!/bin/bash
# ¯\_(ツ)_/¯
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#	createIndices.sh
# A script to create a video and a query list.
#
# Inputs:
# 1) Source directory containing the files
# 2) Input query file (Groundtruth) <matchingcritera, videoid, shotid>
#
# @author skletz
# @version 1.1 01/09/07
# @version 1.0 11/07/07
# -----------------------------------------------------------------------------
# @TODO: /i/TRECVID/trecvid16_data/mastershots/filtered /i/TRECVID/trecvid16_data/groundtruth/relevant-extra.avs.qrels.tv16.csv
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

#Inputs
INDIR=$1
GT=$2

#Outputs
OUT_VIDEOLIST="trecvid16-video-list.csv"
OUT_QUERYLIST="trecvid16-query-list.csv"

echo "Input directory: " $INDIR
echo "Input groundtruth: " $GT

declare -A queriesmap

file=$GT
#iterate throug query csv file and create a map
count=0
IFS=","
[ ! -f $file ] && { echo "$file file not found"; exit 99; }
while read qid vid sid
do
  count=$(( $count + 1 ))
  #echo $qid $vid $sid
  eval index=${vid}"_"${sid}
  queriesmap["$index"]=$qid
done < $file
echo "Number of entries in GT: " $count
# DEBUGGING
# for i in "${!queriesmap[@]}"; do
#   echo "key  : $i"
#   echo "value: ${queriesmap[$i]}"
# done
# eval key="35471_31"
# echo "key  : $key"
# echo "value: "${queriesmap[$key]};

total=$(ls "$INDIR" | wc -l)
pstr="[=======================================================================]"
count=0

find "${INDIR}/" -type f -name '*.mp4' -print0 | while IFS= read -r -d '' file; do
  #Progress bar
  count=$(( $count + 1 ))
  pd=$(( $count * 73 / $total ))
  printf "\r%3d.%1d%% %.${pd}s" $(( $count * 100 / $total )) $(( ($count * 1000 / $total) % 10 )) $pstr

  #group="$(basename "$(dirname "$file")")"
  filename=$(basename "$file")
  filename=${filename%.*}
  #echo $file
  IFS='_' read -r -a array <<< "$filename"
  eval key="${array[0]}_${array[1]}"
  eval value=${queriesmap["$key"]};

  #if the entrie is not found in the map
  if [ -z "$value" ];
  then
    #add entry only in the video list
    echo 0","$filename >> $OUT_VIDEOLIST
    #echo 0","$filename
  else
    #add entry in the video list
    echo $value","$filename >> $OUT_VIDEOLIST
    #add entry in the query list
    echo $value","$filename >> $OUT_QUERYLIST
    #echo "Index in Querylist: "$value","$filename
  fi
done
