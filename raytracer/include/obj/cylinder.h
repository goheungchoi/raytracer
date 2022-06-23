#ifndef _CYLINDER_
#define _CYLINDER_

#include "object.h"

class Cylinder : virtual public Object {
 public:
  Point origin;
  Vector3D dir;
  double radius;
  double length;

  Cylinder();
  Cylinder(std::string name, color clr, double opacity,
           double index_of_refraction, Point origin, Vector3D dir,
           double radius, double length);
  virtual ~Cylinder(){};
  Point* getIntersection(const Line& l);
  Vector3D* getNormalVector(const Point& p, const Line& l) {
    return new Vector3D();
  };
  Point* getOdLambda(const Point& p) {}

 private:
  int lineIntersection(double a, double b, double c);
};

#endif