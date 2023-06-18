#ifndef _POINT_
#define _POINT_

#include "util/util.h"

class Point
{
public:
  double x = 0.0;
  double y = 0.0;
  double z = 0.0;
  // Default constructor
  constexpr Point() = default;
  // Parameterized constructor
  constexpr Point(double x, double y, double z) : x(x), y(y), z(z){};
  // Default copy constructor
  Point(const Point &) = default;
  // Copy assignment operator
  Point &operator=(const Point &other) {
    if (this != &other) {
      x = other.x;
      y = other.y;
      z = other.z;
    }
    return *this;
  }
  // Defaulted move constructor and move assignment operator
  Point(Point &&) noexcept = default;
  Point &operator=(Point &&) noexcept = default;

  // Member functions
  void print() const;

  // Inline member functions
  inline constexpr double getDistance(const Point &p) const {
    double tx = x - p.x, ty = y - p.y, tz = z - p.z;
    return std::sqrt(tx*tx + ty*ty + tz*tz);
  }

  // Deconstructor
  ~Point() = default;
};

// non-member functions
inline constexpr double getDistance(const Point &l, const Point &r) {
  return l.getDistance(r);
}

// Overloaded operators
constexpr Point operator+(const Point &l, const Point &r){
  return Point(l.x + r.x, l.y + r.y, l.z + r.z);
}
constexpr Point operator-(const Point &l, const Point &r){
  return Point(l.x - r.x, l.y - r.y, l.z - r.z);
}
constexpr Point operator*(const Point &l, const Point &r){
  return Point(l.x * r.x, l.y * r.y, l.z * r.z);
}
constexpr Point operator*(const double n, const Point &p){
  return Point(n * p.x, n * p.y, n * p.z);
}
constexpr Point operator*(const Point &l, const double n){
  return n * l;
}
constexpr Point operator/(const Point &l, const Point &r){
  if (r.x == 0 || r.y == 0 || r.z == 0)
    throw std::invalid_argument("Division by zero");
  return Point(l.x / r.x, l.y / r.y, l.z / r.z);
}
constexpr Point operator/(const double n, const Point &p){
  if (p.x == 0 || p.y == 0 || p.z == 0)
    throw std::invalid_argument("Division by zero");
  return Point(n / p.x, n / p.y, n / p.z);
}
constexpr Point operator/(const Point &p, const double n){
  if (n == 0)
    throw std::invalid_argument("Division by zero");
  return Point(p.x / n, p.y / n, p.z / n);
}
constexpr bool operator==(const Point &l, const Point &r){
  bool x = util::almost_equal(l.x, r.x, 100);
  bool y = util::almost_equal(l.y, r.y, 100);
  bool z = util::almost_equal(l.z, r.z, 100);

  return x && y && z;
}
constexpr bool operator!=(const Point &l, const Point &r){
  return !(l == r);
}

#endif // _POINT_H_
