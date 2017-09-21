#!/bin/bash
# ¯\_(ツ)_/¯
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#	.sh
# A script to create a
#
# Inputs:
# 1) Source directory containing the files
# 2) Input query file (Groundtruth) <matchingcritera, videoid, shotid>
#
# @author skletz
# @version 1.0 09/09/17
# -----------------------------------------------------------------------------
# @TODO:
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #


#Inputs
INDIR=$1
GT=$2
#Outpus
OUTDIR=$3

echo INDIR: $INDIR
echo GT: $GT
echo OUTDIR: $OUTDIR

declare -A queriesmap
pstr="[=======================================================================]"
file=$GT
#iterate throug query csv file and create a map
total=$(cat "$GT" | wc -l)
count=0
IFS=","
[ ! -f $file ] && { echo "$file file not found"; exit 99; }
while read qid video
do
  count=$(( $count + 1 ))
  pd=$(( $count * 73 / $total ))
  printf "\r%3d.%1d%% %.${pd}s" $(( $count * 100 / $total )) $(( ($count * 1000 / $total) % 10 )) $pstr

  #echo $qid $vid $sid
  vid=$( echo "$video" |cut -d_ -f1 )
  sid=$( echo "$video" |cut -d_ -f2 )
  #echo "VID: " $vid " SID:" $sid
  eval index=${vid}"_"${sid}
  queriesmap["$index"]=$qid
done < $file
echo "Number of entries in GT: " $count
# DEBUGGING
# for i in "${!queriesmap[@]}"; do
#   echo "key  : $i"
#   echo "value: ${queriesmap[$i]}"
#   read -p "Press any key to continue... " -n1 -s
 #done
 #eval key="35471_31"
 #echo "key  : $key"
# echo "value: "${queriesmap[$key]};


total=$(ls "$INDIR" | wc -l)
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
  #echo "Key:" $key " Value:" $value

  outmaster=""
  outkey=""
  #echo $filename
  #if the entrie is not found in the map
  if [ -z "$value" ];
  then
    #create directory with
    outkey=$OUTDIR/notingt/keyframes
    outmaster=$OUTDIR/notingt/mastershots
  else
    #create directory with query nr
    outkey=$OUTDIR/$value/keyframes
    outmaster=$OUTDIR/$value/mastershots

    mkdir -p $outkey
    mkdir -p $outmaster
    eval "$(ffprobe -v error -of flat=s=_ -show_entries format=duration $file)"
    #copy reduced size of mastershot
    < /dev/null ffmpeg -y -i $file -codec:v libx264 -profile:v high -preset slow -b:v 500k -maxrate 180k -bufsize 1000k -vf scale=-2:280 -threads 0 -codec:a libfdk_aac -b:a 128k $outmaster/$filename"_Xx280".mp4 #> /dev/null 2>&1
    #extract keyframe
    < /dev/null ffmpeg -y -i $file -ss "$(echo "$format_duration/2" | bc)" -q:v 2 -frames:v 1 $outkey/$filename"_mid".jpg #> /dev/null 2>&1
  fi


  unset file
done
