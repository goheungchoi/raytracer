#ifndef _CYLINDER_
#define _CYLINDER_

#include "util/util.h"

#include "object.h"

class Cylinder : virtual public Object {
public:
  // Member variables
  Point origin;
  Vector3D dir;
  double radius;
  double length;
  // Member variables
  Cylinder();
  // Member variables
  Cylinder(obj::color_t color, double opacity, double iRefraction, 
    Point origin, Vector3D dir, double radius, double length);
  // Default destructor
  virtual ~Cylinder(){};
  // Override functions
  std::optional<Point> getIntersection(const Ray& r) const override;
  std::optional<Vector3D> getNormalVector(const Point& p, const Ray& r) const override 
  { return Vector3D(); };
  std::optional<util::arr3> getOdLambda(const Point& p) const override 
  { return std::nullopt; };
  obj::text_coor_t getTextureCoor(const Point& p) const override 
  { return {}; };
  void print() const override {};
  // Member functions
  
  // Inline member functions
  
 private:
  inline constexpr int lineIntersection(double a, double b, double c) const {
    double d = b*b - 4.0*a*c;
    if (util::almost_equal(d, 0.0, 100))
      return ONE_INTERSECTION;
    else if (d > 0)
      return TWO_INTERSECTION;
    else
      return NO_INTERSECTION;
  }
};

#endif
