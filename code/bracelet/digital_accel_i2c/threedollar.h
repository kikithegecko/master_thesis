#include <math.h>

#define SAMPLE_SIZE 150
#define RESAMPLE_SIZE 64 //TODO omit?

struct Point{
  float x;
  float y;
  float z;
};

struct Bounding_Box{
  float min_x;
  float max_x;
  float min_y;
  float max_y;
  float min_z;
  float max_z;
};

// helper function for calculating the centroid
struct Point centroid(struct Point* points, int length);

// helper function for calculating the cross product
// between two vectors/points
struct Point cross_product(struct Point p, struct Point q);

// helper function for step 1:
// calculate eucledian distance
float distance(Point p, Point q);

// helper function for step 2:
// vector rotation in axis-angle representation
// using Rodrigues' rotation formula
struct Point* rotate(struct Point* points, int n, struct Point axis, float angle);

// STEP 1+2+3:
// resample a points path into n evenly spaced points
// rotate points so that their indicative angle is at 0 degrees
// scale points so that the resulting bounding box will be of size*size dimension;
// then translate points to the origin.
struct Point* preprocess(struct Point* points, int n, int bb_size);

// STEP 4:	
// match points against a set of templates.
// returns the matched template's index in the template list.
int recognize(struct Point* points, struct Point* templates, int num_templates);

float distance_at_angle(struct Point* points, struct Point* template, float alpha, float beta, float gamma);
