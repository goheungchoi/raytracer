#ifndef _PLANE_
#define _PLANE_

#include "util/util.h"

#include "object.h"

struct b_plane_coor {
  double alpha;
  double beta;
  double gamma;
  double delta;
};

class Plane : virtual public Object {
public:
  // Member variables
  Vector3D n, dir;
  Point p0;
  double w, h;
  Point p00, p01, p10, p11;
  obj::text_coor_t c00, c01, c10, c11;
  Vector3D T, B;

  // Default constructor
  Plane();
  // Parameterized constructors
  Plane(obj::color_t color, double opacity, double iRefraction,
    Vector3D n, Point p0, Vector3D dir, double w, double h);
  // Default destructor
  virtual ~Plane(){};
  // Override functions
  std::optional<Point> getIntersection(const Ray& r) const override;
  std::optional<Vector3D> getNormalVector(const Point& p, const Ray& r) const override;
  std::optional<util::arr3> getOdLambda(const Point& p) const override;
  obj::text_coor_t getTextureCoor(const Point& p) const override;

  // Member functions
  b_plane_coor getBarycentricCoor(const Point& p) const;
  Vector3D rotateVectorTBN(const Vector3D& m) const;
  // Inline Member functions
  inline bool isBarycentricCoorOnPlane(double alpha, double beta, double gamma, double delta) const {
    double sum = alpha + beta + gamma + delta;
    return 
      util::almost_equal(sum, 1.0, 100) 
      && 0 <= alpha && alpha <= 1 
      && 0 <= beta && beta <= 1 
      && 0 <= gamma && gamma <= 1 
      && 0 <= delta && delta <= 1;
  }
  inline bool isOnSurface(const Point& p) const {
    double result = n.dot(Vector3D(p0, p));
    if (util::almost_equal(result, 0.0, 100)) {
      b_plane_coor barycentric_coor = getBarycentricCoor(p);
      return isBarycentricCoorOnPlane(
          barycentric_coor.alpha, barycentric_coor.beta, barycentric_coor.gamma,
          barycentric_coor.delta);
    } else
      return false;
  }

  void print() const override {
    std::cout << "Plane:" << std::endl;
    p0.print();
    n.print();
    dir.print();
    std::cout << "Width: " << w << std::endl;
    std::cout << "Height: " << h << std::endl;
  }

private:
  /**
   * @brief calculates values such as the positions of edges and T, B vectors.
   *
   */
  inline constexpr void init() {
    Vector3D standard_vec(1, 0, 0);
    double theta = dir.getAngle(standard_vec);
    if (dir.y < 0) theta = -theta;
    T = Vector3D(-n).rotate(0, 90, theta).unit();
    B = (n * T).unit();

    Point scaler_h((T*(w/2.0)).toPoint());
    Point scaler_v((B*(h/2.0)).toPoint());
    p00 = p0 - scaler_h + scaler_v;
    p10 = p0 + scaler_h + scaler_v;
    p01 = p0 - scaler_h - scaler_v;
    p11 = p0 + scaler_h - scaler_v;

    c00 = obj::text_coor_t{0, 0};
    c01 = obj::text_coor_t{0, 1};
    c10 = obj::text_coor_t{1, 0};
    c11 = obj::text_coor_t{1, 1};
  }
  inline constexpr int lineIntersection(const Ray& r) const {
    double d = n.dot(r.v);
    // line and plane are parallel
    if (util::almost_equal(d, 0.0, 100)) {
      Vector3D temp(r.o, p0);
      if (util::almost_equal(n.dot(temp), 0.0, 100))
        return INF_INTERSECTION;  // The line is on the plane
      else  // No interaction
        return NO_INTERSECTION;
    }
    // One intersection
    else return ONE_INTERSECTION;
  }
};

#endif