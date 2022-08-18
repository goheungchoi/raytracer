#ifndef _VECTOR_H
#define _VECTOR_H

#include "point.h"
#define PI 3.14159265

class Vector3D {
 public:
  double x;
  double y;
  double z;

  Vector3D();
  Vector3D(Point tl, Point hd);
  Vector3D(double x, double y, double z);
  Vector3D(const Vector3D &v);
  Vector3D operator+(const Vector3D &v);
  Vector3D operator-(const Vector3D &v);
  Vector3D operator*(const Vector3D &v);
  Vector3D operator*(const double &num);
  Vector3D operator/(const double &num);
  Vector3D operator-();
  double dotProd(const Vector3D &v);
  double getMag();
  Vector3D getUnit();
  Vector3D scale(double num);
  Point toPoint();
  double getAngle(Vector3D v);
  Vector3D rotate(double xtheta, double ytheta, double ztheta);
};

#endif