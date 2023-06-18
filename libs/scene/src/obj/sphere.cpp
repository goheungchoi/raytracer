#include "obj/sphere.h"

#include <math.h>

Sphere::Sphere() : Object({0, 0, 0}, 1.0, 1.0) , origin(Point()), radius(0.0) {}

Sphere::Sphere(obj::color_t color, double opacity, double iRefraction, Point origin, double radius)
: Object(color, opacity, iRefraction), origin(origin), radius(radius) {}

std::optional<Point> Sphere::getIntersection(const Ray& r) const {
  double t = 0.0;
  double a = 1.0;
  // B = 2*(r.v.x*(r.o.x–origin.x) + r.v.y*(r.o.y–origin.y) + r.v.z*(r.o.z–origin.z)) 
  double b1 = r.v.x*(r.o.x - origin.x);
  double b2 = r.v.y*(r.o.y - origin.y);
  double b3 = r.v.z*(r.o.z - origin.z);
  double b = 2*(b1 + b2 + b3);
  // C = (r.o.x–origin.x)^2 + (r.o.y-origin.y)^2 + (r.o.z-origin.z)^2 – radius^2
  double c1 = origin.x - r.o.x;
  double c2 = origin.y - r.o.y;
  double c3 = origin.z - r.o.z;
  double c4 = radius*radius;
  double c = c1*c1 + c2*c2 + c3*c3 - c4;

  int res = Sphere::lineIntersection(a, b, c);
  if (res == NO_INTERSECTION) {
    return std::nullopt;
  } else if (res == ONE_INTERSECTION) {  // One intersection
    t = -b / (2.0*a);
  } else if (res == TWO_INTERSECTION) {  // Two intersection
    double temp = b*b - 4.0*a*c;
    double temp1 = sqrt(temp);
    double pos_temp = temp1 - b;
    double neg_temp = -temp1 - b;
    double pos_res = pos_temp / (2.0*a);
    double neg_res = neg_temp / (2.0*a);
    if (neg_res > 0 && !util::almost_equal(neg_res, 0.0, 100))  // both are positive
      t = neg_res;
    else if (pos_res > 0 && !util::almost_equal(pos_res, 0.0, 100))  // neg_res is negative, yet pos_res is positive
      t = pos_res;
    else 
      return std::nullopt;  // both are negative
  } else {  // This should not happen
    throw std::invalid_argument("error: Sphere::getIntersection: something went wrong:(");
  }

  double x = r.o.x + r.v.x*t;
  double y = r.o.y + r.v.y*t;
  double z = r.o.z + r.v.z*t;
  return Point(x, y, z);
}

std::optional<Vector3D> Sphere::getNormalVector(const Point& p, const Ray& r) const {
  if (isOnSurface(p)) {
    Vector3D unitN(Vector3D(origin, p).unit());
    if (normal_mapping) {
      obj::text_coor_t tc = getTextureCoor(p);
      Vector3D m(normalMap.getBilinearInterpolation(tc));
      Vector3D newN(rotateVectorTBN(m, unitN));
      // is the ray inside the sphere?
      Vector3D v(r.v);
      // if surface_normal~I > inversed_surface_normal~I => ray is inside
      if ((-v).getAngle(newN) > (-v).getAngle(-newN))
        return -newN;
      else
        return newN;
    } else {
      Vector3D v(r.v);
      if ((-v).getAngle(unitN) > (-v).getAngle(-unitN))
        return -unitN;
      else
        return unitN;
    }
  }
  return std::nullopt;
}

std::optional<util::arr3> Sphere::getOdLambda(const Point& p) const {
  if (isOnSurface(p)) {
    if (texture_mapping) {
      obj::text_coor_t tc = getTextureCoor(p);
      return textureMap.getBilinearInterpolation(tc);
    } else 
      return util::arr3{color.dr, color.dg, color.db};
  } else
    throw std::invalid_argument("Sphere::getOdLambda - wrong point");
}

obj::text_coor_t Sphere::getTextureCoor(const Point& p) const {
  double phi = acos((p.y-origin.y)/radius);
  double theta = atan2(p.x-origin.x, p.z-origin.z);
  return {(theta+util::PI)/(util::PI+util::PI), phi/util::PI};
}

Vector3D Sphere::rotateVectorTBN(const Vector3D& m, const Vector3D& N) const {
  double temp = sqrt(N.x*N.x + N.y*N.y);
  Vector3D T(-N.y/temp, N.x/temp, 0);
  Vector3D B = N*T;
  double nx = T.x*m.x + B.x*m.y + N.x*m.z;
  double ny = T.y*m.x + B.y*m.y + N.y*m.z;
  double nz = T.z*m.x + B.z*m.y + N.z*m.z;
  return Vector3D(nx, ny, nz);
}
