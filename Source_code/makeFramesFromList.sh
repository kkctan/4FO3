#!/bin/bash

# ---------- !!!!! WARNING !!!!! ----------
# THIS SCRIPT IS UNTESTED AND UNFINISHED

camPosX=1.2
camPosY=.26
camPosZ=.2

camTargetX=0
camTargetY=0
camTargetZ=0

camWalkX=0
camWalkY=0
camWalkZ=0

camTurningX=0
camTurningY=0
camTurningZ=0

nextPointX=0
nextPointY=0
nextPointZ=0

#num of frames, so 10 seconds of travel at 30fps
travelTime=300
#arbitrary constant/factor/percent for the camTarget to pass the nextPoint by
aPercent=1.01

#line number to read from and a line string to save to
#might be able to remove declarations up here and just have the ones in the for loop
line_num=0
line_str="a"

#array to go through line string
line_array

#var to hold params file name
paramsFileName=paramsBulb.dat

#var to hold path file name, each line holds coords e.g. (1 2 3) or (.243 .114 1)
pathFileName=pathList.dat

#var to hold executable's name
exeName=mandelbulb

for ((i=0; i<7200; i++))
do
	#call mandelbox to render image
    ./mandelbox $paramsFileName
	#call convert.sh
	./convert.sh
	
	compareTemp = $(($i%300))
	#if at the 10 second mark (10*30fps)
	if [ "$compareTemp"==0]; then
		#!!!!! how do I read the next line?
		#get next point, read from file?
		# and set nextPointX/Y/Z
		line_num = $line_num + 1
		line_str = $(sed -n "${line_num}p" "$pathfileName")
		line_array = ( $line_str )
		
		nextPointX = ${line_array[0]}
		nextPointY = ${line_array[1]}
		nextPointZ = ${line_array[2]}
		
		
		camWalkX = $(echo "($nextPointX - $camPosX) / $travelTime" | bc -1)
		camWalkY = $(echo "($nextPointY - $camPosY) / $travelTime" | bc -1)
		camWalkZ = $(echo "($nextPointZ - $camPosZ) / $travelTime" | bc -1)
		
		camTurningX = $(echo "($nextPointX - $camTargetX) / $travelTime * $aPercent" | bc -1)
		camTurningY = $(echo "($nextPointX - $camTargetY) / $travelTime * $aPercent" | bc -1)
		camTurningZ = $(echo "($nextPointX - $camTargetZ) / $travelTime * $aPercent" | bc -1)
	fi
	
	camPosX = $(echo "$camPosX + $ camWalkX" | bc -1)
	camPosY = $(echo "$camPosY + $ camWalkY" | bc -1)
	camPosZ = $(echo "$camPosZ + $ camWalkZ" | bc -1)
	
	camTargetX = $(echo "$camTargetX + $ camTurningX" | bc -1)
	camTargetY = $(echo "$camTargetY + $ camTurningY" | bc -1)
	camTargetZ = $(echo "$camTargetZ + $ camTurningZ" | bc -1)
	
	
	
	#write vars to paramsBulb.dat to update, camPos to line 4, camTarget to line 7
	#replace this with w/e works for writing to file
    sed -i '4s/.*/ $camPosX $camPosY $camPosZ' $paramsFileName
	sed -i '7s/.*/ $camTargetX $camTargetY $camTargetZ' $paramsFileName
	 
done

#call ffmpeg here
#ffmpeg -framerate 30/1 -pattern_type glob -i '*.jpg' -c:v libx264 -r 40 -pix_fmt yuv420p out.mp4
#ffmpeg -i out.mp4 -i timeHZ.mp3 -codec copy -shortest outAudio.mp4
