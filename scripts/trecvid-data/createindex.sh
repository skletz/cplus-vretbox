#!/bin/bash
# ¯\_(ツ)_/¯
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#	createIndex.sh
#
#
# @author skletz
# @version 1.0 11/07/07
# -----------------------------------------------------------------------------
# @TODO:
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

INDIR=$1
QUERYFILE=$2
OUTFILE=$3

echo "Input directory: " $INDIR
echo "Output file: " $OUTFILE

declare -A queriesmap

file=$QUERYFILE
#iterate throug query csv file and create an map
IFS=","
[ ! -f $file ] && { echo "$file file not found"; exit 99; }
while read qid vid sid
do
  echo $qid $vid $sid
  eval index=${vid}"_"${sid}
  queriesmap["$index"]=$qid
done < $file

# for i in "${!queriesmap[@]}"; do
#   echo "key  : $i"
#   echo "value: ${queriesmap[$i]}"
# done

# eval key="35471_31"
# echo "key  : $key"
# echo "value: "${queriesmap[$key]};
counter=-1
find "${INDIR}/" -type f -name '*.mp4' -print0 | while IFS= read -r -d '' file; do
    ((counter++))
    echo "Counter "$counter
    group="$(basename "$(dirname "$file")")"
    filename=$(basename "$file")
    filename=${filename%.*}
    echo $file
    IFS='_' read -r -a array <<< "$filename"
    eval key="${array[0]}_${array[1]}"
    eval value=${queriesmap["$key"]};

    if [ -z "$value" ];
    then
      echo 0","$filename >> $OUTFILE
      echo 0","$filename
    else
      echo $value","$filename >> $OUTFILE
      echo "Index in Querylist: "$value","$filename
    fi
done
