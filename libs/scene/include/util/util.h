#ifndef UTIL_H
#define UTIL_H

#include <exception>
#include <optional>
#include <iostream>
#include <numbers>
#include <cmath>
#include <limits>

namespace util
{
  constexpr double PI = std::numbers::pi;

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
  inline 
  typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type
      almost_equal(T x, T y, int ulp)
  {
      // the machine epsilon has to be scaled to the magnitude of the values used
      // and multiplied by the desired precision in ULPs (units in the last place)
      return std::abs(x - y) <= std::numeric_limits<T>::epsilon() * std::abs(x + y) * ulp
          // unless the result is subnormal
          || std::abs(x - y) < std::numeric_limits<T>::min();
  }
} // namespace util



#endif // UTIL_H