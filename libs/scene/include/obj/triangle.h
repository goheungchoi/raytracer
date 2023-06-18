#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_

#include "util/util.h"

#include "object.h"

class Triangle : virtual public Object {
public:
  // Member variables
  bool smooth_shading = false;
  Point p0, p1, p2;
  obj::text_coor_t c0, c1, c2;
  Vector3D vn0, vn1, vn2;
  Vector3D e1, e2;
  Vector3D T, B, N;
  double A;

  // Default constructor
  Triangle();
  // Parameterized constructors
  Triangle(obj::color_t color, double opacity, double iRefraction, 
    Point p0, Point p1, Point p2);
  // Default destructor
  virtual ~Triangle(){};
  void getTextureVectors();
  // Override functions
  std::optional<Point> getIntersection(const Ray& r) const override;
  std::optional<Vector3D> getNormalVector(const Point& p, const Ray& r) const override;
  std::optional<util::arr3> getOdLambda(const Point& p) const override;
  obj::text_coor_t getTextureCoor(const Point& p) const override;
  void print() const override {};
  // Member functions
  /**
   * @brief A helper function for getNormalVector(Point, Ray).
   * It returns the normal vector this triangle.
   * 
   * @param p 
   * @return std::optional<Vector3D> 
   */
  std::optional<Vector3D> getNormalVector(const Point& p) const;
  util::arr3 getBarycentricCoor(const Point& p) const;
  Vector3D rotateVectorTBN(const Vector3D& m) const;
  // Inline member functions
  inline constexpr bool isBarycentricCoorInTriangle(
    double alpha, double beta, double gamma) const {
    double sum = alpha + beta + gamma;
    return util::almost_equal(sum, 1.0, 100) 
      && 0.0 <= alpha && alpha <= 1.0 
      && 0.0 <= beta && beta <= 1.0 
      && 0.0 <= gamma && gamma <= 1.0;
  }
  inline constexpr bool isInTriangle(const Point& p) const {
    double d = N.dot(Vector3D(p0, p));
    if (!util::almost_equal(d, 0.0, 100)) 
      return false;
    util::arr3 coor = getBarycentricCoor(p);
    return isBarycentricCoorInTriangle(coor.x, coor.y, coor.z);
  }
  inline constexpr bool isTextureMapping() const { return texture_mapping; };
  inline constexpr bool isNormalMapping() const { return normal_mapping; };

 private:
  inline constexpr int lineIntersection(const Ray& l) const {
    double d = N.dot(l.v);
    if (util::almost_equal(d, 0.0, 100)) {  // line and plane are parallel
      Vector3D temp(l.o, p0);
      if (util::almost_equal(N.dot(temp), 0.0, 100))
        return INF_INTERSECTION;  // line is on the plane
      else
        return NO_INTERSECTION;
    } else // one intersection
      return ONE_INTERSECTION;
  };
};

#endif  // _TRIANGLE_H_