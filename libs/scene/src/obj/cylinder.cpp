#include "obj/cylinder.h"

#include <math.h>

Cylinder::Cylinder() 
: Object({0, 0, 0}, 1.0, 1.0), origin(Point()), dir(Vector3D()), radius(0.0), length(0.0) {}

Cylinder::Cylinder(obj::color_t clr, double opacity, double iRefraction, 
  Point origin, Vector3D dir, double radius, double length)
: Object(color, opacity, iRefraction), origin(origin), dir(dir), radius(radius), length(length) {}

std::optional<Point> Cylinder::getIntersection(const Ray& r) const {
  double t = 0.0;
  double a1 = r.v.x*r.v.x;
  double a2 = r.v.y*r.v.y;
  double a = a1 + a2;
  double b1 = r.v.x*(r.o.x - origin.x);
  double b2 = r.v.y*(r.o.y - origin.y);
  double b = 2.0*(b1 + b2);

  double c1 = origin.x - r.o.x;
  double c2 = origin.y - r.o.y;
  double c4 = radius*radius;
  double c = c1*c1 + c2*c2 - c4;

  int res = Cylinder::lineIntersection(a, b, c);
  if (res == NO_INTERSECTION) { // No intersection
    return std::nullopt;
  } else if (res == ONE_INTERSECTION) {  // One intersection
    t = -b / (2.0 * a);
  } else if (res == TWO_INTERSECTION) {  // two intersection
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
      return std::nullopt;
  } else {  // This should not happen
    throw std::invalid_argument("error: Cylinder::getIntersection: something went wrong:(");
  }

  double x = r.o.x + r.v.x*t;
  double y = r.o.y + r.v.y*t;
  double z = r.o.z + r.v.z*t;
  // if the intersetion is out of length
  if (origin.z+length < z || origin.z > z) 
    return std::nullopt;
  return Point(x, y, z);
}
