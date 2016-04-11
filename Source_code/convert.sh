#!/bin/bash

#sconvert to .jpg
for file in *.bmp;
do filename=$(basename $file .bmp);
	convert $file $filename.jpg;
	rm $file; 
done
