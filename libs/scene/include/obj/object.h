#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "util/util.h"

#include "ray.h"

namespace obj {

  /**
   * @brief represents the color properties of an object
   */
  struct color_t {
    double dr{0.0};  // ambient constant of red color
    double dg{0.0};  // ambient constant of green color
    double db{0.0};  // ambient constant of blue color
    double sr{0.0};  // diffuse constant of red color
    double sg{0.0};  // diffuse constant of green color
    double sb{0.0};  // diffuse constant of blue color
    double ka{0.0};  // specular constant of red color
    double kd{0.0};  // specular constant of green color
    double ks{0.0};  // specular constant of blue color
    int n{0};     // shininess constant
  };

  /**
   * @brief represents the texture coordinate
   */
  struct text_coor_t {
    double u{0.0};
    double v{0.0};

    // Overloaded operators
    constexpr text_coor_t operator+(const text_coor_t& coor) const {
      return {u + coor.u, v + coor.v};
    }
    constexpr text_coor_t operator-(const text_coor_t& coor) const {
      return {u - coor.u, v - coor.v};
    }
    constexpr text_coor_t operator*(const text_coor_t& coor) const {
      return {u * coor.u, v * coor.v};
    }
    constexpr text_coor_t operator/(const text_coor_t& coor) const {
      if (coor.u == 0 || coor.v == 0)
        throw std::invalid_argument("Division by zero");
      return {u / coor.u, v / coor.v};
    }
    constexpr text_coor_t operator+(const double n) const {
      return {u + n, v + n};
    }
    constexpr text_coor_t operator-(const double n) const {
      return {u - n, v - n};
    }
    constexpr text_coor_t operator*(const double n) const {
      return {u * n, v * n};
    }
    constexpr text_coor_t operator/(const double n) const {
      if (n == 0)
        throw std::invalid_argument("Division by zero");
      return {u / n, v / n};
    }
  };

  // Non-member Overloaded operators
  constexpr text_coor_t operator+(const double n, const text_coor_t& coor) {
    return coor + n;
  }
  constexpr text_coor_t operator-(const double n, const text_coor_t& coor) {
    return {n - coor.u, n - coor.v};
  }
  constexpr text_coor_t operator*(const double n, const text_coor_t& coor) {
    return coor * n;
  }
  constexpr text_coor_t operator/(const double n, const text_coor_t& coor) {
    if (coor.u == 0 || coor.v == 0)
      throw std::invalid_argument("Division by zero");
    return {n / coor.u, n / coor.v};
  }

  /**
   * @brief represents normal mapping and its calculation
   */
  struct map_t {
    int w, h;
    std::vector<util::arr3> map;
    /**
     * @brief Get the pixel on this map.
     * 
     * @param x column number
     * @param y row number
     * @return util::arr3 3-tuple of RGB
     */
    util::arr3 mapAt(int x, int y) const;
    util::arr3 getBilinearInterpolation(const text_coor_t& c) const;
  };
}


constexpr int NO_INTERSECTION{0};
constexpr int ONE_INTERSECTION{1};
constexpr int TWO_INTERSECTION{2};
constexpr int INF_INTERSECTION{100};

class Object {
public:
  static long unsigned int total;
  long unsigned int id;
  bool texture_mapping = false;
  obj::map_t textureMap;
  bool normal_mapping = false;
  obj::map_t normalMap;
  obj::color_t color;
  double opacity, iRefraction;
  // Default constructor
  Object() : id(total++) {};
  // Parameterized constructors
  Object(obj::color_t color, double opacity, double iRefraction);
  // Default destructor
  virtual ~Object() {};
  // Member functions
  virtual std::optional<Point> getIntersection(const Ray& r) const = 0;
  virtual std::optional<Vector3D> getNormalVector(const Point& p, const Ray& r) const = 0;
  virtual std::optional<util::arr3> getOdLambda(const Point& p) const = 0;
  virtual obj::text_coor_t getTextureCoor(const Point& p) const = 0;
  virtual void print() const = 0;
};

#endif  // _OBJECT_H_
