#!/bin/bash

#This script will convert videos into pictures
#It expects the directories in certain structure
if [ $# -lt 2 ] ; then
  echo "Usage: $0 inDir outDir [width=160 [height=120]]"
  exit 1
fi

inDir=`readlink -f "$1"`
outDir=`readlink -f "$2"`

width=${3:-160}
height=${4:-120}

#Expand wildcards
shopt -s extglob

cd "$inDir"
curDir=`pwd`

for dir in * ; do
  cd "$dir"
  for side in * ; do
    saveDir="$outDir/${width}x${height}/${dir}_${side}"
    mkdir -p "$saveDir"
    echo $saveDir
    for file in $side/*.h264 ; do
      fn=`basename $file`
      tag=${fn##${side}}
      ffmpeg -i $file -vf scale=${width}:${height} $saveDir/image_${tag%%.h264}-%d.jpg
    done
  done
  #Go back to correct directory
  cd "$curDir"
done

cd $outDir
echo "Stats for images generated:"
for dir in * ; do nf=`ls -l  $dir | wc -l`; echo "$dir   $nf"; done

