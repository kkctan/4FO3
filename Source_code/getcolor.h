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

//#include <cmath>
//#include <algorithm>

//using namespace std;

//---lightning and colouring---------DECLARATIONS
static vec3 CamLight = {.x=1.0, .y=1.0, .z=1.0};

static double CamLightW = 1.8;// 1.27536;
static double CamLightMin = 0.3;// 0.48193;
//-----------------------------------
static const vec3 baseColor = {.x=1.0, .y=1.0, .z=1.0};
static const vec3 backColor = {.x=0.0, .y=0.0, .z=0.4};


//-----------------------------------
inline double clamp(double d, double min, double max) 
{
  const double t = d < min ? min : d;
  return t > max ? max : t;
}

inline void clamp(vec3 &v, double min, double max) 
{
  v.x = clamp(v.x,min,max);
  v.y = clamp(v.y,min,max);
  v.z = clamp(v.z,min,max);
}

// kt
// - removing const from n, color, pos and direction
inline void lighting( vec3 &n,  vec3 &color,  vec3 &pos,  vec3 &direction,  vec3 &outV)
{
  //vec3 nn = n - 1.0;
  vec3 nn;
  SUBTRACT_NUM(nn,n,1.0);
  double temp;
  DOT2(temp, direction, nn); //direction, nn ordering here????
  double ambient = MAX(CamLightMin, temp)*CamLightW; 
  MUL_NUM(outV, CamLight, ambient);
  MUL_VEC(outV, outV, color);//color refers to hitColor in this case
}

// kt
// - removing const from pix_data_hit/normal, from and direction
inline void getColour(bool pix_data_escaped,  vec3 &pix_data_hit,  vec3 &pix_data_normal, int colourType, float brightness,
	        vec3 &from,  vec3  &direction, int k, double *color)
{
  //colouring and lightning
  vec3 hitColor = baseColor;
  
  if (pix_data_escaped == false) 
    {
      //apply lighting
      lighting(pix_data_normal, hitColor, pix_data_hit, direction, hitColor);
      
      //add normal based colouring
      if(colourType == 0 || colourType == 1)
	{
	  //hitColor = hitColor * pix_data_normal;
	  MUL_VEC(hitColor, hitColor, pix_data_normal);
	  //hitColor = (hitColor + 1.0)/2.0;
	  ADD_NUM(hitColor, hitColor, 1.0);
	  DIV_NUM(hitColor, hitColor, 2.0);
	  //hitColor = hitColor*brightness;
	  MUL_NUM(hitColor, hitColor, brightness);
	  
	  //gamma correction
	  clamp(hitColor, 0.0, 1.0);
	  MUL_VEC(hitColor, hitColor, hitColor); //hitColor = hitColor*hitColor;
	}
      if(colourType == 1)
	{
	  //"swap" colors
	  double t = hitColor.x;
	  hitColor.x = hitColor.z;
	  hitColor.z = t;
	}
    }
  else 
    //we have the background colour
    MATCH_VEC(hitColor, backColor); //hitColor = backColor;
  
	color[k+2]=hitColor.x;
	color[k+1]=hitColor.y;
	color[k]=hitColor.z;
}
