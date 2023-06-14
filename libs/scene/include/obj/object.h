#ifndef _OBJECT_
#define _OBJECT_

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "ray.h"

/**
 * @brief represents the color values of an object
 */
struct color {
  float dr;  // ambient constant of red color
  float dg;  // ambient constant of green color
  float db;  // ambient constant of blue color
  float sr;  // diffuse constant of red color
  float sg;  // diffuse constant of green color
  float sb;  // diffuse constant of blue color
  float ka;  // specular constant of red color
  float kd;  // specular constant of green color
  float ks;  // specular constant of blue color
  int n;     // shininess constant
};

/**
 * @brief represents the color of a pixel
 */
struct pixel {
  int r;
  int g;
  int b;
};

/**
 * @brief represents the texture coordinate
 */
typedef struct text_coor {
  double u;
  double v;

  text_coor operator+(const text_coor& coor);
  text_coor operator-(const text_coor& coor);
  text_coor operator*(const text_coor& coor);
  text_coor operator/(const text_coor& coor);
  text_coor operator+(const double& num);
  text_coor operator-(const double& num);
  text_coor operator*(const double& num);
  text_coor operator/(const double& num);
} text_coor;

/**
 * @brief represents normal mapping and its calculation
 */
typedef struct map {
  int w, h;
  std::vector<Point> map;
  Point getImage(int i, int j);
  Point getBilinearInterpolation(const text_coor& c);
} map;

class Object {
 public:
  bool texture_mapping = false;
  map texture_map;
  bool normal_mapping = false;
  map normal_map;
  std::string name;
  color clr;
  double opacity, index_of_refraction;
  Object();
  Object(std::string name, color clr, double opacity,
         double index_of_refraction);
  virtual ~Object();
  virtual Point* getIntersection(const Line& l) = 0;
  virtual Vector3D* getNormalVector(const Point& p, const Line& l) = 0;
  virtual Point* getOdLambda(const Point& p) = 0;
  static bool equals(double a, double b) {
    double e = 0.000001;
    return fabs(a - b) < e && -fabs(a - b) < e;
  };
};

#endif