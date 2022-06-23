#ifndef _TRIANGLE_
#define _TRIANGLE_

#include "object.h"

class Triangle : virtual public Object {
 public:
  bool smooth_shading = false;
  Point p0, p1, p2;
  text_coor c0, c1, c2;
  Vector3D vn0, vn1, vn2;
  Vector3D e1, e2;
  Vector3D T, B, N;
  double A;

  Triangle();
  Triangle(std::string name, color clr, double opacity,
           double index_of_refraction, Point p0, Point p1, Point p2);
  void getTextureVectors();
  virtual ~Triangle(){};
  Point* getIntersection(const Line& l);
  Vector3D* getNormalVector(const Point& p);
  Vector3D* getNormalVector(const Point& p, const Line& l) override;
  Point* getOdLambda(const Point& p);
  Vector3D rotateVectorTBN(const Vector3D& m);
  text_coor getTextureCoor(const Point& p);
  Point getBarycentricCoor(const Point& p);
  bool isBarycentricCoorInTriangle(double alpha, double beta, double gamma);
  bool isInTriangle(const Point& p);
  bool isTextureMapping();
  bool isNormalMapping();

 private:
  int lineIntersection(const Line& l);
};

#endif