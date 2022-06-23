#include "vector3D.h"

#include <math.h>

Vector3D::Vector3D() {
  this->x = 0;
  this->y = 0;
  this->z = 0;
}

Vector3D::Vector3D(Point tl, Point hd) {
  this->x = hd.x - tl.x;
  this->y = hd.y - tl.y;
  this->z = hd.z - tl.z;
}

Vector3D::Vector3D(double x, double y, double z) {
  this->x = x;
  this->y = y;
  this->z = z;
}

Vector3D::Vector3D(const Vector3D &v) {
  x = v.x;
  y = v.y;
  z = v.z;
}

Vector3D Vector3D::operator+(const Vector3D &v) {
  return Vector3D(this->x + v.x, this->y + v.y, this->z + v.z);
}

Vector3D Vector3D::operator-(const Vector3D &v) {
  return Vector3D(this->x - v.x, this->y - v.y, this->z - v.z);
}

Vector3D Vector3D::operator*(const Vector3D &v) {
  double i = this->y * v.z - this->z * v.y;
  double j = this->x * v.z - this->z * v.x;
  double k = this->x * v.y - this->y * v.x;
  return Vector3D(i, -j, k);
}

Vector3D Vector3D::operator*(const double &num) {
  double new_x = this->x * num;
  double new_y = this->y * num;
  double new_z = this->z * num;
  return Vector3D(new_x, new_y, new_z);
}

Vector3D Vector3D::operator/(const double &num) {
  double new_x = this->x / num;
  double new_y = this->y / num;
  double new_z = this->z / num;
  return Vector3D(new_x, new_y, new_z);
}

Vector3D Vector3D::operator-() { return Vector3D(-x, -y, -z); }

double Vector3D::dotProd(const Vector3D &v) {
  double new_x = this->x * v.x;
  double new_y = this->y * v.y;
  double new_z = this->z * v.z;
  return new_x + new_y + new_z;
}

Vector3D Vector3D::getUnit() {
  double mag = this->getMag();
  return *this / mag;
}

double Vector3D::getMag() {
  double temp1 = pow(this->x, 2);
  double temp2 = pow(this->y, 2);
  double temp3 = pow(this->z, 2);
  double mag = sqrt(temp1 + temp2 + temp3);
  return mag;
}

Vector3D Vector3D::scale(double num) {
  double i = this->x * num;
  double j = this->y * num;
  double k = this->z * num;
  return Vector3D(i, j, k);
}

Point Vector3D::toPoint() { return Point(x, y, z); }
/**
 * @brief calculate angle between two vectors
 *
 * @param v another vector
 * @return double angle in degree
 */
double Vector3D::getAngle(Vector3D v) {
  return acos(this->getUnit().dotProd(v.getUnit())) * 180.0 / PI;
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
  double xrad = xtheta * PI / 180.0;
  double yrad = ytheta * PI / 180.0;
  double zrad = ztheta * PI / 180.0;

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