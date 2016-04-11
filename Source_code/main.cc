/*
   This file is part of the Mandelbox program developed for the course
    CS/SE  Distributed Computer Systems taught by N. Nedialkov in the
    Winter of 2015-2016 at McMaster University.

    Copyright (C) 2015-2016 T. Gwosdz and N. Nedialkov

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <cmath>
#include <algorithm>
#include <assert.h>

#ifdef _OPENACC
#include <accelmath.h>
#else
#include <math.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "camera.h"
#include "renderer.h"
#include "mandelbox.h"

//void getParameters(char *filename, CameraParams *camera_params, RenderParams *renderer_params,
//		   MandelBoxParams *mandelBox_paramsP);
//void init3D       (CameraParams *camera_params, const RenderParams *renderer_params);
//void renderFractal(const CameraParams &camera_params, const RenderParams &renderer_params, unsigned char* image);
//void saveBMP      (const char* filename, const unsigned char* image, int width, int height);

//in-line includes
#include "savebmp.h"
#include "timing.h"
#include "print.h"

#include "vector3d.h"
#include "color.h"
#include "getcolor.h"

#include "raymarching.h"

#include "3dCC.h"
#include "rendererCC.h" 

#include "getparams.h"
//#include "mandelboxde.h"
//#include "distance_est.h"
//#include "3d.h"
#include "init3D.h"
//

// DPPF (comments blocks relating to the old deepest point path will have DPPF at the beginning)
// - Serial path finder (using "deepest point" from raymarching) unsuccessfully in porting over to parallel code.
// - 'Deepest point' we tried using was the pixel with the greatest totalDist during raymarching that had not escaped.
// - MAX_FRAMES was used with a for loop to track which frame to generate
//#define MAX_FRAMES 7200

// DPPF
// - FPS used to calculate how far the camera position and target should move (larger number decreased movement)
//#define FPS 30
// - TRAVEL_TIME used to calculate far the camera position and target should move
//  (larger number decreased movement)
//#define TRAVEL_TIME 2
// - CAM_TARGET_SPEED used to set how much faster the camera target would be to track towards the deepest point
//   compared to the camera position movement.
// - The larger the number, the faster(or closer) the camera would move towards the deep point
//#define CAM_TARGET_SPEED 3

MandelBoxParams mandelBox_params;

int main(int argc, char** argv)
{

  // DPPF
  // - char array frame_file_name to store what the image would be named e.g. ff00001.bmp, ff00002.bmp, etc.
  //char frame_file_name[20];

  CameraParams    camera_params;
  RenderParams    renderer_params;
  
  getParameters(argv[1], &camera_params, &renderer_params, &mandelBox_params);

  int image_size = renderer_params.width * renderer_params.height;

  unsigned char *image = (unsigned char*)malloc(3*image_size*sizeof(unsigned char));

  int frame_num = 0;

  //DPPF
  // - for loop to pump out frames, frame_num was initialized in rendererCC.h
  //for (frame_num = 0; frame_num < MAX_FRAMES ; frame_num++){

    init3D(&camera_params, &renderer_params);

    renderFractal(camera_params, renderer_params, image, mandelBox_params);

	// DPPF
    // - Set frame_file_name to save to, frame_num was initialized in rendererCC.h
    //sprintf(frame_file_name , "ff%05d.bmp",frame_num);
    // - Call saveBMP using the custom file name based on the frame number
    //saveBMP(frame_file_name, image, renderer_params.width, renderer_params.height);

    saveBMP(renderer_params.file_name, image, renderer_params.width, renderer_params.height);

    // - convert .bmp to .jpg
    //system("./convert.sh");
  
    // DPPF
	// - A fter a frame was saved we would calculate the next set of camera position and target coords.
	// - The 'deepest_point' x/y/z values (0/1/2 in the array) was to be set in rendererCC.h.
	// - Right after rayMarch(...) was completed, rendererCC.h had access to the updated totalDist of the
	//   current pixel, and if it was greater than the current totalDist, it would save that pixel_data.hit
	//   to deepest_point
	//
	// - The direction_vector and target_vector are calculated by taking the difference vector between
	//   the point we wanted to travel towards (deepest_point) and our position/target.
	// - Then reduce the length of direction and target vectors based on how long we wish to take 
	//   (in seconds) to arrive at or look at the deepest point.
	// - Finally we apply the appropriate 'step' amount to the camera position and target.
	//
	// - This worked ok when looking at a mostly spherical mandelbulb but often lead the camera through
	//   a mandelbox fairly quickly and we end up staring outwards from the box with the camera focusing 
	//   on an end/edge piece
	/*
    direction_vector[0] = (deepest_point[0] - camera_params.camPos[0]) / (FPS*TRAVEL_TIME);
    direction_vector[1] = (deepest_point[1] - camera_params.camPos[1]) / (FPS*TRAVEL_TIME);
    direction_vector[2] = (deepest_point[2] - camera_params.camPos[2]) / (FPS*TRAVEL_TIME);
	
	target_vector[0] = (deepest_point[0] - camera_params.camTarget[0]) /  (FPS*TRAVEL_TIME/CAM_TARGET_SPEED);
    target_vector[1] = (deepest_point[1] - camera_params.camTarget[1]) /  (FPS*TRAVEL_TIME/CAM_TARGET_SPEED);
    target_vector[2] = (deepest_point[2] - camera_params.camTarget[2]) /  (FPS*TRAVEL_TIME/CAM_TARGET_SPEED);

	camera_params.camPos[0] = camera_params.camPos[0]+direction_vector[0];
	camera_params.camPos[1] = camera_params.camPos[1]+direction_vector[1];
	camera_params.camPos[2] = camera_params.camPos[2]+direction_vector[2];
  
	camera_params.camTarget[0] = camera_params.camTarget[0]+target_vector[0];
	camera_params.camTarget[1] = camera_params.camTarget[1]+target_vector[1];
	camera_params.camTarget[2] = camera_params.camTarget[2]+target_vector[2];
	*/

  // DPPF
  // end of frames for loop
  //}

  //create video
  /*
  system("ffmpeg -framerate 30/1 -pattern_type glob -i '*.jpg' -c:v libx264 -r 40 -pix_fmt yuv420p out.mp4");
  system("ffmpeg -i out.mp4 -i timeHZ.mp3 -codec copy -shortest outAudio.mp4");
  */ 

  free(image);

  return 0;
}
