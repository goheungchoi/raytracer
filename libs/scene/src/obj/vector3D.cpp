// vector3D.cpp
#include "obj/vector3D.h"

double Vector3D::dot(const Vector3D &v) const {
  return x*v.x + y*v.y + z*v.z;
}

double dot(const Vector3D &l, const Vector3D &r) {
  return l.dot(r);
}

Vector3D Vector3D::unit() const {
  return (*this) / mag();
}

double Vector3D::mag() const {
  return sqrt(x*x + y*y + z*z);
}

Vector3D Vector3D::scale(const double num) const {
  return (*this) * num;
}

Point Vector3D::toPoint() const { return Point(x, y, z); }
/**
 * @brief calculate angle between two vectors
 *
 * @param v another vector
 * @return double angle in degree
 */
double Vector3D::getAngle(Vector3D v) const {
  return std::acos(unit().dot(v.unit())) * 180.0/util::PI;
}
/**
 * @brief rotate vector around 
 *
 * @param xtheta x angle in degree
 * @param ytheta y angle in degree
 * @param ztheta z angle in degree
 * @return rotated vector
 */
Vector3D Vector3D::rotate(double xtheta, double ytheta, double ztheta) {
  double xrad = xtheta * util::PI / 180.0;
  double yrad = ytheta * util::PI / 180.0;
  double zrad = ztheta * util::PI / 180.0;

  double temp1_1 = x * cos(yrad) * cos(zrad);
  double temp1_2_1 = sin(xrad) * sin(yrad) * cos(zrad);
  double temp1_2_2 = cos(xrad) * sin(zrad);
  double temp1_2 = y * (temp1_2_1 - temp1_2_2);
  double temp1_3_1 = cos(xrad) * sin(yrad) * cos(zrad);
  double temp1_3_2 = sin(xrad) * sin(zrad);
  double temp1_3 = z * (temp1_3_1 + temp1_3_2);
  double temp1 = temp1_1 + temp1_2 + temp1_3;

  double temp2_1 = x * cos(yrad) * sin(zrad);
  double temp2_2_1 = sin(xrad) * sin(yrad) * sin(zrad);
  double temp2_2_2 = cos(xrad) * cos(zrad);
  double temp2_2 = y * (temp2_2_1 + temp2_2_2);
  double temp2_3_1 = cos(xrad) * sin(yrad) * sin(zrad);
  double temp2_3_2 = sin(xrad) * cos(zrad);
  double temp2_3 = z * (temp2_3_1 - temp2_3_2);
  double temp2 = temp2_1 + temp2_2 + temp2_3;

  double temp3_1 = x * (-sin(yrad));
  double temp3_2 = y * sin(xrad) * cos(yrad);
  double temp3_3 = z * cos(xrad) * cos(yrad);
  double temp3 = temp3_1 + temp3_2 + temp3_3;
  return Vector3D(temp1, temp2, temp3);
}