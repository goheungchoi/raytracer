#ifndef UTIL_H
#define UTIL_H

#include <memory>
#include <limits>
#include <exception>
#include <optional>
#include <iostream>
// #include <numbers>   // c++20
#include <cmath>
#include <string>
#include <vector>

namespace util
{
  // constexpr double PI = std::numbers::pi;
  constexpr double PI = 3.14159265358979323846;

  /**
   * @brief Compare two non-integer numbers.
   * https://en.cppreference.com/w/cpp/types/numeric_limits/epsilon
   * 
   * @tparam T type of two numbers
   * @param x 
   * @param y 
   * @param ulp units in the last place. The lower, the strict comparison.
   * @return std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type 
   */
  template<class T>
  inline constexpr
  typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type
      almost_equal(T x, T y, int ulp = 100, T abs_tol = 1e-5)
  {
      // the machine epsilon has to be scaled to the magnitude of the values used
      // and multiplied by the desired precision in ULPs (units in the last place)
      return std::abs(x - y) <= std::numeric_limits<T>::epsilon()*std::abs(x + y)*ulp
          // unless the result is subnormal
          || std::abs(x - y) < std::numeric_limits<T>::min()
          || std::abs(x - y) < abs_tol;
  };

  struct arr3 {
    double x{0.0};
    double y{0.0};
    double z{0.0};
  };
  // Overloaded operators
  constexpr arr3 operator+(const arr3& l, const arr3& r){
      return {l.x + r.x, l.y + r.y, l.z + r.z};
  }
  constexpr arr3 operator-(const arr3& l, const arr3& r){
      return {l.x - r.x, l.y - r.y, l.z - r.z};
  }
  constexpr arr3 operator*(const arr3& l, const arr3& r){
      return {l.x * r.x, l.y * r.y, l.z * r.z};
  }
  constexpr arr3 operator*(const double n, const arr3& p){
      return {n * p.x, n * p.y, n * p.z};
  }
  constexpr arr3 operator*(const arr3& l, const double n){
      return n * l;
  }
  constexpr arr3 operator/(const arr3& l, const arr3 &r){
      if (r.x == 0 || r.y == 0 || r.z == 0)
          throw std::invalid_argument("Division by zero");
      return {l.x / r.x, l.y / r.y, l.z / r.z};
  }
  constexpr arr3 operator/(const double n, const arr3 &p){
      if (p.x == 0 || p.y == 0 || p.z == 0)
          throw std::invalid_argument("Division by zero");
      return {n / p.x, n / p.y, n / p.z};
  }
  constexpr arr3 operator/(const arr3 &p, const double n) {
      if (n == 0)
          throw std::invalid_argument("Division by zero");
      return {p.x / n, p.y / n, p.z / n};
  }
  constexpr bool operator==(const arr3& l, const arr3& r){
      bool x = util::almost_equal(l.x, r.x, 100);
      bool y = util::almost_equal(l.y, r.y, 100);
      bool z = util::almost_equal(l.z, r.z, 100);
      
      return x && y && z; 
  }
  constexpr bool operator!=(const arr3& l, const arr3& r){
      return !(l == r);
  }

} // namespace util



#endif // UTIL_H