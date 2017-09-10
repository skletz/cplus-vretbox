#!/bin/bash
# ¯\_(ツ)_/¯
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#	xtract-keyframe.sh
# A script for extracting the first, middle, and last frame of each video segment
# Dependencies: ffprobe, ffmpeg
#
# Inputs:   Input Directory
# Outputs:  Output Directory
#
# @author skletz
# @version 1.2 17/08/17
# @version 1.1 27/06/17
# @version 1.0 16/06/17
# -----------------------------------------------------------------------------
# @TODO:
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

INPUT=$1
OUTPUT=$2

echo Input Directory: $1
echo Output Directory: $2

  for dir in $1/*
  do
      for file in $dir/*
      do
          name=$(echo $(basename "$file") | sed -e 's/\.[^.]*$//')
		  echo  $OUTPUT/${dir##*/}
          mkdir -p $OUTPUT/${dir##*/}
          eval "$(ffprobe -v error -of flat=s=_ -show_entries format=duration $file)"

		  ffmpeg -y -i $file -ss "$(echo "0" | bc)" -q:v 2 -frames:v 1 $OUTPUT/${dir##*/}/$name'_1s'.jpg
		  ffmpeg -y -i $file -ss "$(echo "$format_duration/2" | bc)" -q:v 2 -frames:v 1 $OUTPUT/${dir##*/}/$name'_2c'.jpg
		  lf=`ffprobe -show_streams "$file" 2> /dev/null | grep nb_frames | head -1 | cut -d \= -f 2`
		  let "lf = $lf - 1"
		  ffmpeg -y -i $file -vf select=\'eq\(n,$lf\) -q:v 2 -vframes 1 $OUTPUT/${dir##*/}/$name'_3e'.jpg

      done

  done
