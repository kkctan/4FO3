#!/bin/bash

#increment factor, keep less than 1
#0.9993 is pretty fast, should've used 0.99978 or greater
scale=0.9993

#timer start
start=$SECONDS

#variable to hold the executable file name
exeFileName=mandelbulb

#variable to hold params file name
paramsFileName=paramsBox.dat

#string to hold the image name
imageName="image.bmp"

#set variables to w/e the start camPos is (in paramsBox.dat)
varX=14.0
varY=8.0
varZ=10.0

#for loop, runs for the number of frames
for ((i=0; i<7200; i++))
do

  echo "i = " $i

  #attempt to replace 4th line in paramsBulb.dat with vars
  sed -i "4s/.*/$varX $varY $varZ/" $paramsFileName
  
  #set var holding image name
  imageName=$(printf ff%05d.bmp $i)

  #set the image name
  sed -i "28s/.*/$imageName/" $paramsFileName

  #run mandelbox, might need another paramsBulb.dat so the original still exists
  ./$exeFileName $paramsFileName

  # can't run convert.sh on all gpu machines (tesla,gpu1 and moore can)
  ./convert.sh

  #update vars
  varX=$(echo "$varX * $scale" | bc -l)
  varY=$(echo "$varY * $scale" | bc -l)
  varZ=$(echo "$varZ * $scale" | bc -l)

echo $varX $varY $varZ

done

#make the video here after frames are generated

#timer end
duration=$(( SECONDS - start ))
sed -i "2s/.*/$duration/" timer.txt