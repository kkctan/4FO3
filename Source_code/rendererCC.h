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

// DPPF
// - Initialize the deepest_point array here, accessible to main.cc
//double deepest_point[3];
// - Also deepest_length 
//double deepest_length;
// - frame_num, initially in main.cc, used here to help guide camera into
//   the box/buld over the first few frames
//int frame_num;



void renderFractal(const CameraParams &camera_params, const RenderParams &renderer_params, 
		   unsigned char* image, const MandelBoxParams &mandelBox_params)
{

  
  const double eps = pow(10, renderer_params.detail); 
  double farPoint[3]={0,0,0};
  vec3 to, from;
  
	SET_POINT(from,camera_params.camPos)
  
  //prepare render_params for copyin
  const int height = renderer_params.height;
  const int width  = renderer_params.width;
  const int colourType = renderer_params.colourType;
  const float brightness = renderer_params.brightness;
  const int maxRaySteps = renderer_params.maxRaySteps;
  const float maxDistance = renderer_params.maxDistance;
  
  //prepare camera_params for copyin
  double camPos[3] = {camera_params.camPos[0],camera_params.camPos[1],camera_params.camPos[2]};
  double matInvProjModel[16] = {camera_params.matInvProjModel[0], camera_params.matInvProjModel[1], \
										camera_params.matInvProjModel[2], camera_params.matInvProjModel[3], \
										camera_params.matInvProjModel[4], camera_params.matInvProjModel[5], \
										camera_params.matInvProjModel[6], camera_params.matInvProjModel[7], \
										camera_params.matInvProjModel[8], camera_params.matInvProjModel[9], \
										camera_params.matInvProjModel[10], camera_params.matInvProjModel[11], \
										camera_params.matInvProjModel[12], camera_params.matInvProjModel[13], \
										camera_params.matInvProjModel[14], camera_params.matInvProjModel[15]  };
										
  const int viewport[4] = {camera_params.viewport[0], camera_params.viewport[1], camera_params.viewport[2], camera_params.viewport[3]};
  
  //prepare mandelBox_params for copyin
  float rMin = mandelBox_params.rMin;
  float rFixed = mandelBox_params.rFixed;
  float scale = mandelBox_params.scale;
  int num_iter = mandelBox_params.num_iter;
  // kt
  // - added escape_time for mandelbox
  float escape_time = mandelBox_params.escape_time;
  
  //pixelData pix_data;  //the new pixel data is three independent variables
  bool pix_data_escaped;
  vec3 pix_data_hit;
  vec3 pix_data_normal;
  
  
  double time = getTime();
  
  int i,j,k;
  //int sd = sizeof(double);
  int maxAddress = 3*width*height; //maxAddress of image array (I think)
  
  	//double color[3*width*height]; //should acc malloc, and then can ignore copyin?
	double *color = (double*)malloc(3*height*width*sizeof(double));

// Eric
#pragma acc parallel loop copy (color[0:maxAddress], image[0:maxAddress])
	for (int g = 0; g < 3*height*width; g++) {
		color[g]=0.0;
		image[g]=0;
	}
// printf to flag when the process is about to enter the pixels loop
printf("rendering started\n");

#pragma acc parallel present_or_copyin (i,j,k, farPoint[0:3], to, from, eps, \
							pix_data_escaped, pix_data_hit, pix_data_normal, \
							color[0:maxAddress], image[0:maxAddress], \
							camPos[0:3], matInvProjModel[0:16], viewport[0:4], \
							height, width, colourType, brightness, maxRaySteps, maxDistance, \
							rMin, rFixed, scale, num_iter, escape_time) \
					copyout (image[0:maxAddress])
{

// added parallel before 'loop'
#pragma acc loop gang
	{
  for(j = 0; j < height; j++)
    {
      //for each column pixel in the row
#pragma acc loop vector private(k, farPoint, to, pix_data_escaped, pix_data_hit, pix_data_normal)
      {
      for(i = 0; i <width; i++)
	{
	  // get point on the 'far' plane
	  // since we render one frame only, we can use the more specialized method
	  UnProject(i, j, viewport, matInvProjModel, farPoint);
	  
	  // to = farPoint - camera_params.camPos
	  SUBTRACT_POINT(to,farPoint,camPos);
	  NORMALIZE(to);
	  
	  //render the pixel
	  rayMarch(maxDistance, maxRaySteps, from, to, eps, pix_data_escaped, pix_data_hit, pix_data_normal, 
	           rMin, rFixed, scale, num_iter,escape_time);
	  
	  //get the colour at this pixel
	  k = (j * width + i)*3; //address	  
	  getColour(pix_data_escaped, pix_data_hit, pix_data_normal, colourType, brightness, from, to, k, color);
      
	  //save colour into texture

	  image[k+2] = (unsigned char)(color[k+2] * 255);
	  image[k+1] = (unsigned char)(color[k+1] * 255);
	  image[k]   = (unsigned char)(color[k] * 255);
	}
	}//end of inner pragma for
    //  printProgress((j+1)/(double)height,getTime()-time);
    }
    }//end of first pragma for
}
  // DDPF
  // - In serial deepest_point and deepest_length were set to 0 within renderFractal(...).
  // - At after each pixels rayMarch(...) call an if statement would compare the newly updated totalDist.
  // - If that pixel was found to have a totalDist greater than deepest_length it would set the deepest_point
  //   array with the coordinates of pixel_data.hit
  // - We had planned to save the totlDist's of each pixel that had not escaped to cycle through and find
  //   our next deepest_point.
  // - for loop here to find greatest totalDist (the farthest point) and set
  //   deepest_point[0/1/2] to pixel_data_hit.x/y/z
  //
  // - Can also potentially use 

  printf("\n rendering done:\n");
  free(color);
}
