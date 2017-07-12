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

rundir=`dirname $0`
#saveDir=$1
width=640
height=480
[ $# -ge 2 ] && width=$2
[ $# -eq 3 ] && height=$3

echo "To do the recording, set the required camera angle."
echo "Then, position the car at asked positions and move it along that position around the track."
echo "out means just outside that lane, in means in the middle of that lane"
echo "and center means on the center line."

for cam_pos in center left30 right30 ; do
  echo "Set Camera angle: $cam_pos"
  echo "Press enter when ready. You will be asked to set position next"
  read input

  for pos in rightout rightin middle leftin leftout ; do
    echo "Set car to position: $pos"
    echo "Press enter when ready"
    read input
    saveDir="$1/$cam_pos/$pos"
    echo "Saving video to $saveDir. Press Ctrl+C to stop recoding when done"
    mkdir -p $saveDir
    #raspivid -t 0 -w $width -h $height -o $saveDir/$(date +'%Y_%m_%d_%I_%M_%p').h264
    cmd="raspivid -t 0 -fps 50 -w $width -h $height -o $saveDir/$(date +'%Y_%m_%d_%I_%M_%p').h264"
    python $rundir/capture_sensor_data.py -l $saveDir/$(date +'%Y_%m_%d_%I_%M_%p').log -r "$cmd"
  done
done
