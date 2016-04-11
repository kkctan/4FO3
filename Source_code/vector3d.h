#ifndef vec3_h
#define vec3_h

//#ifdef _OPENACC
//#include <accelmath.h>
//#else
//#include <math.h>
//#endif


typedef struct 
{
  double x, y, z;
}  vec3;

#define SET_POINT(p,v) { p.x=v[0]; p.y=v[1]; p.z=v[2]; }

#define SUBTRACT_POINT(p,v,u)			\
  {						\
  p.x=(v[0])-(u[0]);				\
  p.y=(v[1])-(u[1]);				\
  p.z=(v[2])-(u[2]);				\
}

#define SUBTRACT_NUM(p,v,u)			\
  {						\
  p.x=(v.x)-u;				\
  p.y=(v.y)-u;				\
  p.z=(v.z)-u;				\
}

#define ADD_NUM(p,v,u)			\
  {						\
  p.x=(v.x)+u;				\
  p.y=(v.y)+u;				\
  p.z=(v.z)+u;				\
}

#define MUL_NUM(p,v,u)			\
  {						\
  p.x=(v.x)*u;				\
  p.y=(v.y)*u;				\
  p.z=(v.z)*u;				\
}

#define DIV_NUM(p,v,u)			\
  {						\
  p.x=(v.x)*(1.0/u);				\
  p.y=(v.y)*(1.0/u);				\
  p.z=(v.z)*(1.0/u);				\
}

#define MUL_VEC(p,v,u)			\
  {						\
  p.x=(v.x)*(u.x);				\
  p.y=(v.y)*(u.y);				\
  p.z=(v.z)*(u.z);				\
}

#define ADD_VEC(p,v,u)			\
  {						\
  p.x=(v.x)+(u.x);				\
  p.y=(v.y)+(u.y);				\
  p.z=(v.z)+(u.z);				\
}

#define SUBTRACT_VEC(p,v,u)			\
  {						\
  p.x=(v.x)-(u.x);				\
  p.y=(v.y)-(u.y);				\
  p.z=(v.z)-(u.z);				\
}

#define MATCH_VEC(p,v)			\
  {						\
  p.x=(v.x);				\
  p.y=(v.y);				\
  p.z=(v.z);				\
}

#define NORMALIZE(p) {					\
    double fMag = ( p.x*p.x + p.y*p.y + p.z*p.z );	\
    if (fMag != 0)					\
      {							\
	double fMult = 1.0/sqrt(fMag);			\
	p.x *= fMult;					\
	p.y *= fMult;					\
	p.z *= fMult;					\
      }							\
  }


#define MAGNITUDE(m,p) 	({ m=sqrt( p.x*p.x + p.y*p.y + p.z*p.z ); })

#define DOT(d,p) {  d=( p.x*p.x + p.y*p.y + p.z*p.z ); }

#define DOT2(t,d,p) {  t=( d.x*p.x + d.y*p.y + d.z*p.z ); }

#define MAX(a,b) ( ((a)>(b))? (a):(b))

#define VEC(v,a,b,c) { v.x = a; v.y = b; v.z = c; }

#endif
