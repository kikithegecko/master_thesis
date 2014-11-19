#include "threedollar.h"

struct Point centroid(struct Point* points, int length){
  struct Point c;
  int i;
  c.x = 0;
  c.y = 0;
  c.z = 0;
  for(i = 0; i < length; i++){
    c.x += points[i].x;
    c.y += points[i].y;
    c.z += points[i].z;
  }
  c.x /= length;
  c.y /= length;
  c.z /= length;
  return c;
}

struct Point cross_product(struct Point p, struct Point q){
  struct Point r;
  r.x = p.y * q.z - p.z * q.y;
  r.y = p.z * q.x - p.x * q.z;
  r.z = p.x * q.y - p.y * q.x;
  return r;
}

float distance(struct Point p, struct Point q){
  return sqrt((q.x - p.x)*(q.x - p.x) + (q.y - p.y)*(q.y - p.y) + (q.z - p.z)*(q.z - p.z));
}

struct Point* rotate(struct Point* points, int n, struct Point axis, float angle){
  struct Point v;
  int i;
  float l = sqrt(axis.x*axis.x + axis.y*axis.y + axis.z*axis.z);
  if(l != 0){
    v.x = axis.x;
    v.y = axis.y;
    v.z = axis.z;
  }
  else{
    v = axis;
  }
  struct Point* newpoints = malloc(sizeof(struct Point)*n);
  for(i = 0; i < n; i++){
    struct Point cp = cross_product(points[i], axis);
    float sp = points[i].x * axis.x + points[i].y * axis.y + points[i].z * axis.z;
    newpoints[i].x = cos(angle) * points[i].x + sin(angle) * cp.x + (1 - cos(angle)) * sp * axis.x;
    newpoints[i].y = cos(angle) * points[i].y + sin(angle) * cp.y + (1 - cos(angle)) * sp * axis.y;
    newpoints[i].z = cos(angle) * points[i].z + sin(angle) * cp.z + (1 - cos(angle)) * sp * axis.z;
  }
  return newpoints;
}

struct Point* preprocess(struct Point* points, int n, int bb_size){
  // resample a points path into n evenly spaced points
  float dist = 0;
  
// rotate points so that their indicative angle is at 0 degrees
// scale points so that the resulting bounding box will be of size*size dimension;
// then translate points to the origin.
}
