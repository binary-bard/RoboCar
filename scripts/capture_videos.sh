#!/bin/bash

#This script will capture the videos 
if [ $# -lt 1 ] ; then
  echo "Usage: $0 saveDir [width [height]]"
  exit 1
fi

capture_cmd="/usr/bin/raspivid"
if [ ! -x $capture_cmd ] ; then
  ls -l $capture_cmd && echo "Error: Please check execute permissions for $capture_cmd. This script relies on it."
  exit 2
fi
#saveDir=$1
width=640
height=480
[ $# -ge 2 ] && width=$2
[ $# -eq 3 ] && height=$3

for cam_pos in center left30 right30 ; do
  echo "Move the car around setting the camera in $cam_pos. Press enter when ready"
  read input

  for pos in rightout rightin middle leftin leftout ; do
    echo "Position the car at $pos and move it along that position around the track."
    echo "out means just outside that lane, in means in the middle of that lane"
    echo "and center means on the center line."
    echo "Press enter when ready and Ctrl+C to stop recoding when done"
    read input
    saveDir="$1/$cam_pos/$pos"
    echo $saveDir
    mkdir -p $saveDir
    raspivid -t 0 -w $width -h $height -o $saveDir/$(date +"%Y_%m_%d_%I_%M_%p").h264
  done
done
