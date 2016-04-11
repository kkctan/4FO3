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


inline double MandelBoxDE(const vec3 &p0, float rMin, float rFixed, int num_iter, float escape_time, double c1, double c2)
{
  vec3 z;
  z = p0;
  
  double dr = 1.0;
  double r = 0.0;

  double Bailout = rMin;
  double Power = rFixed;

  for (int i=0; i < num_iter; i++) 
    {
      // kt
      // - commented out the Magnitude call below and plopped
      // in it's functionality below (just sets r)
      r = sqrt( z.x*z.x + z.y*z.y + z.z*z.z );
      //Magnitude(r,z);
      if(r > Bailout) break; 

      double theta = acos(z.z/r);
      double phi   = atan2(z.y, z.x);
      dr = pow(r, Power - 1.0) * Power * dr + 1.0;

      double zr = pow(r, Power);
      theta     = theta * Power;
      phi       = phi * Power;

      z.x = zr*sin(theta)*cos(phi);
      z.y = zr*sin(phi)*sin(theta);
      z.z = zr*cos(theta);

      z.x = z.x + p0.x;
      z.y = z.y + p0.y;
      z.z = z.z + p0.z;
    }

  return 0.5*log(r)*r/dr;
}

//#pragma acc routine seq
inline double DE(const vec3 &p, float rMin, float rFixed, float scale, int num_iter, float escape_time)
{
  double c1 = fabs(scale - 1.0);
  double c2 = pow( fabs(scale), 1 - num_iter);
  double d = MandelBoxDE(p, rMin, rFixed, num_iter, escape_time, c1, c2);
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



