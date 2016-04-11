
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
#include "color.h"
//#include "mandelbox.h"
#ifdef _OPENACC
#include <accelmath.h>
#else
#include <math.h>
#endif

// kt
// - renamed mandelboxde.cc to mandelboxde_ori.cc to preserve original,
// shouldn't have any part in the make/compilation


// kt
// - renamed mandelbulb_dist_est.cc to mandelboxde.cc
// - changed function name (MandelBulbDistanceEstimator to MandelBoxDE)
// - added 2 additional params (double c1, double c2) to make the compiler happy
// , doesnt actually influence the bulb generation (as far as I can tell)
// - added escape_time after num_iter
double MandelBoxDE(const vec3 &p0, float rMin, float rFixed, int num_iter, float escape_time, double c1, double c2)
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






