#ifndef _TRIANGLE_MESH_
#define _TRIANGLE_MESH_

#include "object.h"
#include "triangle.h"
#include "vector3D.h"

class TriangleMesh : virtual public Object {
 public:
  std::vector<Triangle*>* triangles;

  TriangleMesh();
  TriangleMesh(std::string name, color clr, double opacity,
               double index_of_refraction);
  ~TriangleMesh();
  Point* getIntersection(const Line& l);
  Vector3D* getNormalVector(const Point& p);
  Vector3D* getNormalVector(const Point& p, const Line& l) override;
  Point* getOdLambda(const Point& p);

 private:
  int lineIntersection(double a, double b, double c);
};

#endif