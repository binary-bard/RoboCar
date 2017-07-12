#!/bin/bash

#This script will create train, val and test directories for network training
#It runs after video2images.sh script
if [ $# -lt 2 ] ; then
  echo "Usage: $0 inDir outDir"
  exit 1
fi

#Change directories to absolute path
inDir=`readlink -f "$1"`
outDir=`readlink -f "$2"`

#Expand wildcards
shopt -s extglob

cd "$inDir"
for subDir1 in train val test ; do
  #Create directory
  outSubDir="${outDir}/${subDir1}"
  mkdir -p "$outSubDir"
  for subDir2 in * ; do
    #Make a sub-directory for each directory corresponding to data dir
    mkdir -p "${outSubDir}/${subDir2}"
    absPath=`readlink -f "$subDir2"`
    if [ "$subDir1" == "train" ] ; then
      echo "Creating links from $absPath to ${outSubDir}/${subDir2}"
      ln -s "$absPath"/* "${outSubDir}/${subDir2}"
    fi
  done
done

#Fraction to use 20% for val and test, 60% for train
# At this point, train directory has all the links to data files
cd "$outDir/train"
for subDir in * ; do
  cd "$subDir"
  #Total files
  nTotal=`ls -l | wc -l`
  echo -n "$nTotal files in "
  pwd
  # Move 20% of the links to val and 20% to test
  nUse=$(($nTotal/5))
  echo "Moving $nUse links to  ../../val/$subDir"
  files=`ls | shuf -n $nUse`
  mv $files ../../val/$subDir
  echo "Moving $nUse links to  ../../test/$subDir"
  files=`ls | shuf -n $nUse`
  mv $files ../../test/$subDir
  cd ..
done

cd $outDir
echo "Restructured data files:"
for dir in train val test ; do
  for subdir in $dir/*; do
    echo -n "$subdir "
    ls -l $subdir | wc -l
  done
done

