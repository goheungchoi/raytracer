#include "cylinder.h"

#include <math.h>

Cylinder::Cylinder() : Object("", color{0, 0, 0}, 1.0, 1.0) {
  this->origin = Point();
  this->dir = Vector3D();
  this->radius = 0.0;
  this->length = 0.0;
}

Cylinder::Cylinder(std::string name, color clr, double opacity,
                   double index_of_refraction, Point origin, Vector3D dir,
                   double radius, double length)
    : Object(name, clr, opacity, index_of_refraction) {
  this->origin = origin;
  this->dir = dir;
  this->radius = radius;
  this->length = length;
}

Point* Cylinder::getIntersection(const Line& l) {
  double t = 0.0;
  double a1 = l.v.x * l.v.x;
  double a2 = l.v.y * l.v.y;
  double a = a1 + a2;
  double b1 = l.v.x * (l.o.x - origin.x);
  double b2 = l.v.y * (l.o.y - origin.y);
  double b = 2 * (b1 + b2);

  double c1 = origin.x - l.o.x;
  double c2 = origin.y - l.o.y;
  double c4 = radius * radius;
  double c = c1 * c1 + c2 * c2 - c4;

  int isIntersection = Cylinder::lineIntersection(a, b, c);
  if (isIntersection > 0) {
    if (isIntersection == 1) {  // one intersection
      t = -b / (2 * a);
    } else {  // two intersection
      double temp = b * b - 4 * a * c;
      double temp1 = sqrt(temp);
      double pos_temp = temp1 - b;
      double neg_temp = -temp1 - b;
      double pos_res = pos_temp / (2 * a);
      double neg_res = neg_temp / (2 * a);
      if (neg_res > 0)  // both are positive
        t = neg_res;
      else if (pos_res > 0)  // neg_res is negative, yet pos_res is positive
        t = pos_res;
      else  // both are negative
        return nullptr;
    }
  } else {  // if there is no intersection
    return nullptr;
  }
  double x = l.o.x + l.v.x * t;
  double y = l.o.y + l.v.y * t;
  double z = l.o.z + l.v.z * t;
  // if the intersetion is out of length
  if (origin.z + length < z || origin.z > z) return nullptr;
  return new Point(x, y, z);
}

int Cylinder::lineIntersection(double a, double b, double c) {
  double d = b * b - 4 * a * c;
  if (Object::equals(d, 0.000000000))
    return 1;
  else if (d > 0)
    return 2;
  else
    return 0;
}