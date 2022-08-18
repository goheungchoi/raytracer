#include "obj/sphere.h"

#include <math.h>

Sphere::Sphere() : Object("", color{0, 0, 0}, 1.0, 1.0) {
  this->origin = Point();
  this->radius = 0.0;
}

Sphere::Sphere(std::string name, color clr, double opacity,
               double index_of_refraction, Point origin, double radius)
    : Object(name, clr, opacity, index_of_refraction) {
  this->origin = origin;
  this->radius = radius;
}

Point* Sphere::getIntersection(const Line& l) {
  double t = 0.0;
  double a = 1.0;
  // B = 2 * (l.v.x * (l.o.x – origin.x) + l.v.y * (l.o.y – origin.y)
  //  + l.v.z * (l.o.z – origin.z)) 
  // C = (l.o.x – origin.x)^2 + (l.o.y - origin.y)^2 + (l.o.z
  //  - origin.z)^2 – radius^2
  double b1 = l.v.x * (l.o.x - origin.x);
  double b2 = l.v.y * (l.o.y - origin.y);
  double b3 = l.v.z * (l.o.z - origin.z);
  double b = 2 * (b1 + b2 + b3);

  double c1 = origin.x - l.o.x;
  double c2 = origin.y - l.o.y;
  double c3 = origin.z - l.o.z;
  double c4 = radius * radius;
  double c = c1 * c1 + c2 * c2 + c3 * c3 - c4;

  int isIntersection = Sphere::lineIntersection(a, b, c);
  if (isIntersection > 0) {
    if (isIntersection == 1) {
      t = -b / (2 * a);
    } else {
      double temp = b * b - 4 * a * c;
      double temp1 = sqrt(temp);
      double pos_temp = temp1 - b;
      double neg_temp = -temp1 - b;
      double pos_res = pos_temp / (2 * a);
      double neg_res = neg_temp / (2 * a);
      if (neg_res > 0 && !Object::equals(neg_res, 0.0))  // both are positive
        t = neg_res;
      else if (pos_res > 0 &&
               !Object::equals(
                   pos_res,
                   0.0))  // neg_res is negative, yet pos_res is positive
        t = pos_res;
      else  // both are negative
        return nullptr;
    }
  } else {
    return nullptr;
  }

  double x = l.o.x + l.v.x * t;
  double y = l.o.y + l.v.y * t;
  double z = l.o.z + l.v.z * t;

  return new Point(x, y, z);
}

int Sphere::lineIntersection(double a, double b, double c) {
  double d = b * b - 4 * a * c;
  if (Object::equals(d, 0.0))
    return 1;
  else if (d > 0)
    return 2;
  else
    return 0;
}

Vector3D* Sphere::getNormalVector(const Point& p, const Line& l) {
  if (isOnSurface(p)) {
    Vector3D unit_n = Vector3D(origin, p).getUnit();
    if (normal_mapping) {
      text_coor tc = getTextureCoor(p);
      Point vec = normal_map.getBilinearInterpolation(tc);
      Vector3D m(vec.x, vec.y, vec.z);
      Vector3D normal_vector(rotateVectorTBN(m, unit_n));
      // is the ray inside the sphere?
      Vector3D v(l.v);
      // if surface_normal~I > inversed_surface_normal~I => ray is inside
      if ((-v).getAngle(normal_vector) > (-v).getAngle(-normal_vector))
        return new Vector3D(-normal_vector);
      else
        return new Vector3D(normal_vector);
    } else {
      Vector3D v(l.v);
      if ((-v).getAngle(unit_n) > (-v).getAngle(-unit_n))
        return new Vector3D(-unit_n);
      else
        return new Vector3D(unit_n);
    }
  } else
    return nullptr;
}

Point* Sphere::getOdLambda(const Point& p) {
  if (isOnSurface(p)) {
    if (texture_mapping) {
      text_coor tc = getTextureCoor(p);
      Point vec = texture_map.getBilinearInterpolation(tc);
      return new Point(vec);
    } else
      return new Point(clr.dr, clr.dg, clr.db);
  } else
    throw std::invalid_argument("Sphere::getOdLambda - wrong point");
}

bool Sphere::isOnSurface(const Point& p) {
  double result = origin.getDistance(p) - radius;
  Vector3D unit_n = Vector3D(origin, p).getUnit();
  Vector3D* vec = new Vector3D(unit_n.x, unit_n.y, unit_n.z);
  return Object::equals(result, 0.0);
}

text_coor Sphere::getTextureCoor(const Point& p) {
  double phi = acos((p.y - origin.y) / radius);
  double theta = atan2(p.x - origin.x, p.z - origin.z);
  return text_coor{(theta + PI) / 2.0 / PI, phi / PI};
}

Vector3D Sphere::rotateVectorTBN(Vector3D& m, Vector3D& N) {
  double temp = sqrt(N.x * N.x + N.y * N.y);
  Vector3D T(-N.y / temp, N.x / temp, 0);
  Vector3D B = N * T;
  double nx = T.x * m.x + B.x * m.y + N.x * m.z;
  double ny = T.y * m.x + B.y * m.y + N.y * m.z;
  double nz = T.z * m.x + B.z * m.y + N.z * m.z;
  return Vector3D(nx, ny, nz);
}