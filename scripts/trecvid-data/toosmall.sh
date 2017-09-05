#!/bin/bash
# ¯\_(ツ)_/¯
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#	toosmall.sh
# A script to list master shot files with less than x seconds to update
# ground truths from the TRECVid dataset.
# This script process master shot files with the following filename format.
# <videoID>_<shotID>_<startframe>-<endframe>_<fps>_<width>x<height>.mp4
# Examples can be found in the "./testdata/shots/"
#
# Inputs: 1) Video-File
#         2) Maximal shot length in seconds
# Outputs:  1) CSV file with master shots less than x seconds
#           2) CSV file with master shots with more than x second
#           3) CSV file with all master shots containing a filter for the files with less than x second
# Each line indicate a master shot entry in the following format:
# <path,directory,filename,basename,videoID,shotID,startframe,endframe,fps,width,height,duration,too-small>
#
# Enable globstar option (shopt -s globstar)
#
# @author skletz
# @version 1.0 12/06/17
# -----------------------------------------------------------------------------
# @TODO:
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

USAGE="A script to list master shot files with less than x seconds.
Usage: `basename $0` [-infile] [-outfile] [-srv]
    -h    Shows help
    -i    Video-File <.MP4>
    -m    Maximal shot length in seconds
    -o    Output CSV file for master shots less than x seconds
    -u    Output CSV file for master shots with more than x seconds
    -a    Output CSV file for all founded master shots
 Examples:
    bash `basename $0` -i ../testdata/shots/39104_70_1930-1932_30.08_320x240.mp4 -m 1 -o ../testdata/waste-master-shots.csv -u ../testdata/new-master-shots.csv -a ../testdata/all-master-shots.csv
    for file in ../testdata/shots/*; do bash `basename $0` -i \$file -m 1 -o ../testdata/waste-master-shots.csv -u ../testdata/new-master-shots.csv -a ../testdata/all-master-shots.csv; done"

INFILE=""
OUTFILE_WASTE=""
OUTFILE_NEW=""
OUTFILE_ALL=""
# max seconds
MAX=""

# parse command line
if [ $# -eq 0 ]; then #  must be at least one arg
    echo "$USAGE" >&2
    exit 1
fi

while getopts i:m:o:u:a:h OPT; do
    case $OPT in
    h)  echo "$USAGE"
        exit 0 ;;
    i)  INFILE=$OPTARG ;;
    m)  MAX=$OPTARG ;;
    o)  OUTFILE_WASTE=$OPTARG ;;
    u)  OUTFILE_NEW=$OPTARG ;;
    a)  OUTFILE_ALL=$OPTARG ;;
    \?) # getopts issues an error message
        echo "$USAGE" >&2
        exit 1 ;;
    esac
done
shift `expr $OPTIND - 1`

printf "%-30s %s\n" "input file :"  "$INFILE"
printf "%-30s %s\n" "max seconds :"  "$MAX"
printf "%-30s %s\n" "output file (smaller than):"   "$OUTFILE_WASTE"
printf "%-30s %s\n" "output file (greater than):"   "$OUTFILE_NEW"
printf "%-30s %s\n" "output file (all) :"    "$OUTFILE_ALL"


function func_getVar {
  videoid=$(echo $1 | cut -d'_' -f1 )
  shotid=$(echo $1 | cut -d'_' -f2 )
  startframe=$(echo $1 | cut -d'_' -f3 | cut -d'-' -f1)
  endframe=$(echo $1 | cut -d'_' -f3 | cut -d'-' -f2)
  fps=$(echo $1 | cut -d'_' -f4 )
  width=$(echo $1 | cut -d'_' -f5 | cut -d'x' -f1 )
  height=$(echo $1 | cut -d'_' -f5 | cut -d'x' -f2 | cut -d'.' -f1)
}

videoid=""
shotid=""
startframe=""
endframe=""
fps=""
width=""
height=""

# enable globstar options (shopt -s globstar)
file=$INFILE
filename=$(basename "$file")
#echo "filename=$filename"
dirname="$(basename "$(dirname "$file")")"
#echo "dirname=$dirname"

echo -e "DEBUG:\tprocess file:\t$file"

if echo $filename | grep -Eq '[0-9]+_[0-9]+_[0-9]+-[0-9]+_[0-9]+.[0-9]+_[0-9]+x[0-9]+.mp4'
then
  func_getVar $filename

  length=$(echo $endframe - $startframe | bc -l);
  duration=$(echo $length / $fps | bc -l);

  #debugging
  #echo "videoid: $videoid, shotid: $shotid, startframe: $startframe, endframe: $endframe, fps: $fps, width: $width, height: $height, duration=$duration

  line="$dirname/$filename,$dirname,$filename,${filename%.*},$videoid,$shotid,$startframe,$endframe,$fps,$width,$height"
  filter=''
  if (($(bc <<< "$duration < $MAX")))
  then
    duration=0$(echo "$duration")
    #echo "===> Shot has less than $MAX second (duration)"
    echo $line,$duration >> $OUTFILE_WASTE
    filter='x'
  else
    echo $line,$duration >> $OUTFILE_NEW
    filter=''
  fi #if smaller than max seconds

  echo $line,$duration,$filter >> $OUTFILE_ALL

fi #if regex pattern matches
