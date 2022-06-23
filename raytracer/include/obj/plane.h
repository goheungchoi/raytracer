#ifndef _PLANE_
#define _PLANE_

#include "object.h"

typedef struct b_plane_coor {
  double alpha;
  double beta;
  double gamma;
  double delta;
} b_plane_coor;

class Plane : virtual public Object {
 public:
  Vector3D n, dir;
  Point p0;
  double w, h;
  Point p00, p01, p10, p11;
  text_coor c00, c01, c10, c11;
  Vector3D T, B;

  Plane();
  Plane(std::string name, color clr, double opacity, double index_of_refraction,
        Vector3D n, Point p0, Vector3D dir, double w, double h);
  /**
   * @brief calculates values such as the positions of edges and T, B vectors.
   *
   */
  void init();

  virtual ~Plane(){};
  Point* getIntersection(const Line& l);
  Vector3D* getNormalVector(const Point& p, const Line& l) override;
  Point* getOdLambda(const Point& p);
  text_coor getTextureCoor(const Point& p);
  bool isOnSurface(const Point& p);
  b_plane_coor getBarycentricCoor(const Point& p);
  bool isBarycentricCoorOnPlane(double alpha, double beta, double gamma,
                                double delta);
  Vector3D rotateVectorTBN(const Vector3D& m);

 private:
  int lineIntersection(const Line& l);
};

#endif