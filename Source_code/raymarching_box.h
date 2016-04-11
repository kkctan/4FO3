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
//#include <assert.h>
//#include <algorithm>

//#include "color.h"

//extern double DE(const vec3 &p, float rMin, float rFixed, float scale, int num_iter);
//void normal (const vec3 & p, vec3 & normal, float rMin, float rFixed, float scale, int num_iter);

// kt
// - includes for box
#include <cmath>
#include <cstdio>
#include <algorithm>
//#include "color.h"
#include "mandelbox.h"
//#include <math.h>

// kt
// - ripped from mandelboxde_bulb
#include "color.h"
//#include "mandelbox.h"
#ifdef _OPENACC
#include <accelmath.h>
#else
#include <math.h>
#endif



#define SQR(x) ((x)*(x))

#define COMPONENT_FOLD(x) { (x) = fabs(x) <= 1? (x) : copysign(2,(x))-(x); }


inline double MandelBoxDE(const vec3 &p0, float rMin, float rFixed, float scale, int num_iter, float escape_time, double c1, double c2)
{
  vec3 p = p0;
  double rMin2   = SQR(rMin);
  double rFixed2 = SQR(rFixed);
  double escape  = SQR(escape_time);
  double dfactor = 1; 
  double r2      =-1;
  const double rFixed2rMin2 = rFixed2/rMin2;

  int i = 0;
  while (i< num_iter && r2 < escape)
    {
      COMPONENT_FOLD(p.x);
      COMPONENT_FOLD(p.y);
      COMPONENT_FOLD(p.z);
      
      //r2 = p.Dot(p);      
      DOT(r2,p);

      if (r2<rMin2)
	{
	  //p = p*(rFixed2rMin2);
         MUL_NUM(p,p,rFixed2rMin2);
	  dfactor *= (rFixed2rMin2);
	}
      else
      if ( r2<rFixed2) 
	{
	  const double t = (rFixed2/r2);
	  //p = p*(rFixed2/r2);;
         MUL_NUM(p,p,rFixed2/r2);
	  dfactor *= t;
	}
      

      dfactor = dfactor*fabs(scale)+1.0;      
      //p = p*scale+p0;
      MUL_NUM(p,p,scale);
      ADD_VEC(p,p,p0);
      i++;
    }
  
  //return  (p.Magnitude() - c1) / dfactor - c2;
  double m = 0;
  MAGNITUDE(m,p);
  return m/dfactor - c2;

}

//#pragma acc routine seq
inline double DE(const vec3 &p, float rMin, float rFixed, float scale, int num_iter, float escape_time)
{
  double c1 = fabs(scale - 1.0);
  double c2 = pow( fabs(scale), 1 - num_iter);
  double d = MandelBoxDE(p, rMin, rFixed, scale, num_iter, escape_time, c1, c2);
  return d;
}

inline void normal(const vec3 & p, vec3 & normal, float rMin, float rFixed, float scale, int num_iter, float escape_time)
{
  // compute the normal at p
  const double sqrt_mach_eps = 1.4901e-08;

  double m;
  MAGNITUDE(m,p);
  double eps = MAX( m, 1.0 )*sqrt_mach_eps;

  vec3 e1 = {.x=eps, .y=0  , .z=0  };
  vec3 e2 = {.x=0  , .y=eps, .z=0  };
  vec3 e3 = {.x=0  , .y=0  , .z=eps};
  
  vec3 p1;
  vec3 p2;
  //normal = vec3(DE(p+e1)-DE(p-e1), DE(p+e2)-DE(p-e2), DE(p+e3)-DE(p-e3));
  ADD_VEC(p1, p, e1);
  SUBTRACT_VEC(p2, p, e1);
  normal.x= (DE(p1,rMin,rFixed,scale,num_iter,escape_time)-DE(p2,rMin,rFixed,scale,num_iter,escape_time));
  
  ADD_VEC(p1, p, e2);
  SUBTRACT_VEC(p2, p, e2);
  normal.y= (DE(p1,rMin,rFixed,scale,num_iter,escape_time)-DE(p2,rMin,rFixed,scale,num_iter,escape_time));
  
  ADD_VEC(p1, p, e3);
  SUBTRACT_VEC(p2, p, e3);
  normal.z= (DE(p1,rMin,rFixed,scale,num_iter,escape_time)-DE(p2,rMin,rFixed,scale,num_iter,escape_time));
  
  NORMALIZE(normal);
}

//#pragma acc routine seq
// kt
// - added escape_time following num_iter
inline void rayMarch(float maxDistance, int maxRaySteps, const vec3 &from, const vec3  &direction, double eps,
	      bool &pix_data_escaped, vec3 &pix_data_hit, vec3 &pix_data_normal, float rMin, float rFixed, float scale, int num_iter, float escape_time)
{
  double dist = 0.0;
  double totalDist = 0.0;
  
  // We will adjust the minimum distance based on the current zoom

  double epsModified = 0.0;
  
  int steps=0;
  vec3 p;
  do 
    {      
      
	MUL_NUM(p,direction,totalDist);	  //p = from + direction * totalDist;
	ADD_VEC(p,from,p);
      dist = DE(p, rMin, rFixed, scale, num_iter, escape_time);
      
      totalDist += .95*dist;
      
      epsModified = totalDist;
      epsModified*=eps;
      steps++;
    }
  while (dist > epsModified && totalDist <= maxDistance && steps < maxRaySteps);
  
  //vec3 hitNormal;
  if (dist < epsModified) 
    {
      //we didnt escape
      pix_data_escaped = false;
      
      // We hit something, or reached MaxRaySteps
      MATCH_VEC(pix_data_hit,p); //pix_data_hit = p;
      
      //figure out the normal of the surface at this point
      vec3 normPos;
	  vec3 temp;
	  MUL_NUM(temp,direction,epsModified);
	  SUBTRACT_VEC(normPos, p, temp) //p - direction * epsModified;
      normal(normPos, pix_data_normal, rMin, rFixed, scale, num_iter,escape_time);
    }
  else 
    //we have the background colour
    pix_data_escaped = true;
}



