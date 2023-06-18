#ifndef _SPHERE_
#define _SPHERE_

#include "util/util.h"

#include "object.h"

class Sphere : virtual public Object {
public:
  // Member variables
  Point origin;
  double radius;

  // Default constructor
  Sphere();
  // Parameterized constructors
  Sphere(obj::color_t color, double opacity, double iRefraction, 
    Point origin, double radius);
  // Default destructor
  ~Sphere() = default;
  // Override functions
  std::optional<Point> getIntersection(const Ray& r) const override;
  std::optional<Vector3D> getNormalVector(const Point& p, const Ray& r) const override;
  std::optional<util::arr3> getOdLambda(const Point& p) const override;
  obj::text_coor_t getTextureCoor(const Point& p) const override;
  // Member functions
  Vector3D rotateVectorTBN(const Vector3D& m, const Vector3D& N) const;
  // Inline member functions
  inline bool isOnSurface(const Point& p) const {
    double result = origin.getDistance(p) - radius;
    return util::almost_equal(result, 0.0, 100);
  }

  void print() const override {
    std::cout << "Sphere:" << std::endl;
    origin.print();
    std::cout << "Radius: " << radius << std::endl;
  }

private:
  inline constexpr int lineIntersection(double a, double b, double c) const{
    double d = b*b - 4*a*c;
    if (util::almost_equal(d, 0.0, 100)) 
      return ONE_INTERSECTION;
    else if (d > 0) 
      return TWO_INTERSECTION;
    else 
      return NO_INTERSECTION;
  }
};

#endif