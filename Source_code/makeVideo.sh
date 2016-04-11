#!/bin/bash

#remake the build
make clean
make

#call the makeFrames.sh script
./makeFrames.sh


#creates first 4 seconds from opening frame
ffmpeg -loop 1 -i OpeningFrameBox.jpg -c:v libx264 -t 4 -pix_fmt yuv420p out1.mp4

#video 2: the mandelbox frames
ffmpeg -framerate 30/1 -pattern_type glob -i '*.jpg' -c:v libx264 -r 40 -pix_fmt yuv420p out2.mp4

#takes 2 videos and adds together (out1 and out2)
ffmpeg -i out1.mp4 -c copy -bsf:v h264_mp4toannexb -f mpegts temp1.ts
ffmpeg -i out2.mp4 -c copy -bsf:v h264_mp4toannexb -f mpegts temp2.ts
ffmpeg -i "concat:temp1.ts|temp2.ts" -c copy -bsf:a aac_adtstoasc out.mp4

#adds song time by Hans Zimmer
ffmpeg -i out.mp4 -i timeHZ.mp3 -codec copy -shortest outAudio.mp4
