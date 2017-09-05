#!/bin/bash
# ¯\_(ツ)_/¯
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#	divdeo.sh
# A script for dividing the videos of the TREVid dataset into the proposed master-shots
# Dependencies: avprobe, avconv
#
# Inputs: 1) Video-File
#         2) Directory of master shot boundaries
#         3) ID (Server-ID in order to distinguish between different outputs)
# Outputs:  1) Master shot files
#
# @author skletz
# @version 1.2 19/06/07 shots with only one frame are now extracted as image
# @version 1.1 23/05/07
# @making 15/05/07
# -----------------------------------------------------------------------------
# @TODO:
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #


USAGE="A script for dividing the videos of the TREVid dataset into the proposed master-shots with avprobe and avconv
The output contains two further files: videos-msb-l1s.csv (shots that are less than 1 second), videos-msb.csv (all extracted shots)
Usage: `basename $0` [-video] [-MSB]
    -h    Shows help
    -i    Video-File <.MP4>
    -m    Directory that contains master shot boundaries <Video-File-Name.CSV>: <startframe>,<endframe>
    -o    The path to the output directory
    -s    Server-ID <s1,s2 ...> for the identification of the output files on different servers
 Examples:
    bash `basename $0` -i ../testdata/videos/39238.mp4 -m ../testdata/msbs -o ../testdata/shots -s s1"

VDO_FILE=""
MSB_DIR=""
OUTDIR=""
P_ID=""
SEP="# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #"
TOOL=avconv
INFO=avprobe

# parse command line
if [ $# -eq 0 ]; then #  must be at least one arg
    echo "$USAGE" >&2
    exit 1
fi

while getopts i:m:o:s:h OPT; do
    case $OPT in
    h)  echo "$USAGE"
        exit 0 ;;
    i)  VDO_FILE=$OPTARG ;;
    m)  MSB_DIR=$OPTARG ;;
    o)  OUTDIR=$OPTARG ;;
    s)  P_ID=$OPTARG ;;
    \?) # getopts issues an error message
        echo "$USAGE" >&2
        exit 1 ;;
    esac
done
shift `expr $OPTIND - 1`

#show input
echo $SEP
printf "%-20s %s\n" "input file :"  "$VDO_FILE"
printf "%-20s %s\n" "input directory :"   "$MSB_DIR"
printf "%-20s %s\n" "output directory :"   "$OUTDIR"
printf "%-20s %s\n" "server id :"    "$P_ID"
echo $SEP

#prepare input
VDO_FILENAME=$(basename "$VDO_FILE")
VDO_DIR=$(dirname "$VDO_FILE")
MSB_FILE=$MSB_DIR/${VDO_FILENAME%.*}.csv

OUTPUT=$OUTDIR

# get video information
echo "Search for fps, widht, height using avprobe ..."
FPS=$(${INFO} -v error -show_format -show_streams ${VDO_FILE} | grep avg_frame_rate | head -n1 | cut -d '=' -f 2)
FPS=$(echo "scale=2; ${FPS}" | bc -l)
WIDTH=$(${INFO} -v error -show_format -show_streams ${VDO_FILE} | grep ^width= | cut -d '=' -f 2)
HEIGHT=$(${INFO} -v error -show_format -show_streams ${VDO_FILE} | grep ^height= | cut -d '=' -f 2)
echo "Found video attributes in: $VDO_FILE with FPS: $FPS; WIDTH: $WIDTH; HEIGHT: $HEIGHT"
#BITRATE=$(avprobe -v error -show_format -show_streams ${VDO_FILE} | grep -m2 bit_rate | tail -n1 | cut -d '=' -f 2)

#line number is the shot id in msb files
shotid=0

#iterate throug csv file (msb file)
IFS=","
[ ! -f $INPUT ] && { echo "$INPUT file not found"; exit 99; }
while read start end
do
  echo $SEP
  echo "Process master shot boundary using $MSB_FILE ..."

  #shot id starts with line 1
  shotid=$((shotid+1))
  SECOND_START=$(echo "$start / $FPS" | bc -l);
  SECOND_END=$(echo $end / $FPS | bc -l);
  TIMESTAMP=$SECOND_START
  #TIMESTAMP=$(date -d@$SECOND_START -u +%H:%M:%S.%s)

  #DURATION=$(echo $SECOND_END-$SECOND_START | bc -l);
  #DURATION=$(echo $DURATION | bc -l);
  #frames per segment
  FRAMECNT=$(echo "($end - $start) + 1" | bc -l);
  DURATION=$(echo $FRAMECNT/$FPS | bc -l);
  echo "Found master shot: id: $shotid, start: $start, end: $end, start timestamp: $TIMESTAMP, #frames: $FRAMECNT, duration: $DURATION"

  if (($(bc <<< "$TIMESTAMP < 1 && $TIMESTAMP != 0")))
  then
    TIMESTAMP=0$(echo "$TIMESTAMP")
  fi

  if (($(bc <<< "$DURATION < 1 && $DURATION != 0")))
  then
    DURATION=0$(echo "$DURATION")
  fi

  OUTPUT=${VDO_FILENAME%.*}"_"$shotid"_"$start-$end"_"$FPS"_"$WIDTH"x"$HEIGHT
  COMMAND=""
  if (($(bc <<< "$FRAMECNT == 1"))) #save an image
  then
    OUTPUT="${OUTPUT}.jpg"
    COMMAND="$TOOL -y -i $VDO_FILE -ss $TIMESTAMP -frames:v 1 $OUTDIR/$OUTPUT"

  else
    OUTPUT="${OUTPUT}.mp4"
    COMMAND="$TOOL -y -i $VDO_FILE -ss $TIMESTAMP -t $DURATION -r $FPS -c:v libx264 -an -sn $OUTDIR/$OUTPUT"
  fi

  #outputfile
  echo "Output File: $OUTDIR/$OUTPUT"

  #DURATION=$(date -d@$DURATION -u +%H:%M:%S.%s)

  echo $SEP
  echo "Command: $COMMAND"
  eval $COMMAND
  echo $SEP

done < $MSB_FILE
