// vector3D.h
#ifndef _VECTOR_3D_H_
#define _VECTOR_3D_H_

#include "util/util.h"

#include "point.h"

class Vector3D {
public:
  double x = 0.0;
  double y = 0.0;
  double z = 0.0;
  // Default constructor
  constexpr Vector3D() = default;
  // Parameterized constructors
  constexpr Vector3D(double x, double y, double z) : x(x), y(y), z(z) {};
  constexpr Vector3D(Point tl, Point hd) : x(hd.x - tl.x), y(hd.y - tl.y), z(hd.z - tl.z) {};
  constexpr Vector3D(util::arr3 arr) : x(arr.x), y(arr.y), z(arr.z) {}
  // Default copy constructor
  Vector3D(const Vector3D &v) = default;
  // Copy assignment operator
  Vector3D& operator=(const Vector3D& other) = default;
  Vector3D& operator=(const util::arr3& arr);
  // Defaulted move constructor and move assignment operator
  Vector3D(Vector3D&&) noexcept = default;
  Vector3D& operator=(Vector3D&&) noexcept = default;
  Vector3D& operator=(util::arr3&&) noexcept;

  // Member functions
  inline constexpr double dot(const Vector3D &v) const {
    return x*v.x + y*v.y + z*v.z;
  }
  inline constexpr double mag() const {
    return sqrt(x*x + y*y + z*z);
  }
  inline constexpr Vector3D unit() const {
    double tmp = mag();
    return Vector3D(x/tmp, y/tmp, z/tmp);
  }
  inline constexpr Vector3D scale(const double num) const {
    return Vector3D(x*num, y*num, z*num);
  }
  inline constexpr Point toPoint() const { return Point(x, y, z); }
  void print() const {
    std::cout << "Vector: (" << x << ", " << y << ", " << z << ")" << std::endl;
  }
  
  /**
   * @brief calculate angle between two vectors
   *
   * @param v another vector
   * @return double angle in degree
   */
  inline constexpr double getAngle(Vector3D v) const {
    return std::acos(unit().dot(v.unit())) * 180.0/util::PI;
  }
  
  inline constexpr Vector3D rotate(double xtheta, double ytheta, double ztheta) const {
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

  // Overloaded operant
  constexpr Vector3D operator-() const { return Vector3D(-x, -y, -z); }

  constexpr Vector3D& operator+=(const Vector3D& rhs) {                           
    x += rhs.x; y += rhs.y; z += rhs.z;
    return *this; // return the result by reference
  }
  constexpr Vector3D& operator-=(const Vector3D& rhs) {
    x -= rhs.x; y -= rhs.y; z -= rhs.z;
    return *this;
  }
  constexpr Vector3D& operator*=(const double n) {
    x *= n; y *= n; z *= n;
    return *this;
  }
  constexpr Vector3D& operator/=(const double n) {
    if (n == 0)
      throw std::invalid_argument("Division by zero");
    x /= n; y /= n; z /= n;
    return *this;
  }
};

// non-member functions
inline constexpr double dot(const Vector3D& l, const Vector3D& r) {
  return l.dot(r);
}

// Overloaded operators
constexpr Vector3D operator+(const Vector3D& l, const Vector3D& r){
  return Vector3D(l.x + r.x, l.y + r.y, l.z + r.z);
}
constexpr Vector3D operator-(const Vector3D& l, const Vector3D& r){
  return Vector3D(l.x - r.x, l.y - r.y, l.z - r.z);
}
constexpr Vector3D operator*(const Vector3D& l, const Vector3D& r){
  double i = l.y * r.z - l.z * r.y;
  double j = l.x * r.z - l.z * r.x;
  double k = l.x * r.y - l.y * r.x;
  return Vector3D(i, -j, k);
}
constexpr Vector3D operator*(const double n, const Vector3D& v){
  return Vector3D(n * v.x, n * v.y, n * v.z);
}
constexpr Vector3D operator*(const Vector3D& v, const double n){
  return n * v;
}
constexpr Vector3D operator/(const Vector3D& l, const Vector3D& r){
  if (r.x == 0 || r.y == 0 || r.z == 0)
    throw std::invalid_argument("Division by zero");
  return Vector3D(l.x / r.x, l.y / r.y, l.z / r.z);
}
constexpr Vector3D operator/(const double n, const Vector3D& v){
  if (v.x == 0 || v.y == 0 || v.z == 0)
    throw std::invalid_argument("Division by zero");
  return Vector3D(n / v.x, n / v.y, n / v.z);
}
constexpr Vector3D operator/(const Vector3D& v, const double n){
  if (n == 0)
    throw std::invalid_argument("Division by zero");
  return Vector3D(v.x/n, v.y/n, v.z/n);
}

#endif // _VECTOR_3D_H_
