// vector3D.h
#ifndef _VECTOR_3D_H_
#define _VECTOR_3D_H_

#include "util.h"

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
  // Default copy constructor
  Vector3D(const Vector3D &v) = default;
  // Copy assignment operator
  Vector3D& operator=(const Vector3D& other) = default;
  // Defaulted move constructor and move assignment operator
  Vector3D(Vector3D&&) noexcept = default;
  Vector3D& operator=(Vector3D&&) noexcept = default;

  // Member functions
  double dot(const Vector3D &v) const;
  double mag() const;
  Vector3D unit() const;
  Vector3D scale(const double num) const;
  Point toPoint() const;
  double getAngle(Vector3D v) const;
  Vector3D rotate(double xtheta, double ytheta, double ztheta);

  // Overloaded operant
  constexpr Vector3D operator-() { return Vector3D(-x, -y, -z); }

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
double dot(const Vector3D &l, const Vector3D &r);

// Overloaded operators
constexpr Vector3D operator+(const Vector3D& l, const Vector3D& r){
  return Vector3D(l.x + r.x, l.y + r.y, l.z + r.z);
}
constexpr Vector3D operator-(const Vector3D& l, const Vector3D& r){
  return Vector3D(l.x - r.x, l.y - r.y, l.z - r.z);
}
constexpr Vector3D operator*(const Vector3D& l, const Vector3D& r){
  return Vector3D(l.x * r.x, l.y * r.y, l.z * r.z);
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