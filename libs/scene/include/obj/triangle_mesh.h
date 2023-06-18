#ifndef _TRIANGLE_MESH_
#define _TRIANGLE_MESH_

#include "util/util.h"

#include "object.h"
#include "triangle.h"
#include "vector3D.h"

class TriangleMesh : virtual public Object {
public:
  // Member variables
  std::vector<std::unique_ptr<Triangle>> triangles;

  // Default constructor
  TriangleMesh();
  // Parameterized constructors
  TriangleMesh(obj::color_t color, double opacity, double iRefraction)
  : Object(color, opacity, iRefraction) {};
  // Default destructor
  ~TriangleMesh() {};
  // Move constructor and move assignment operator
  TriangleMesh(TriangleMesh&& other) noexcept : triangles(std::move(other.triangles)) {}
  TriangleMesh& operator=(TriangleMesh&& other) noexcept {
    if (&other == this)
      return *this;

    triangles = std::move(other.triangles);
    return *this;
  }
  // Override functions
  std::optional<Point> getIntersection(const Ray& r) const override;
  std::optional<Vector3D> getNormalVector(const Point& p, const Ray& l) const override;
  std::optional<util::arr3> getOdLambda(const Point& p) const override;
  obj::text_coor_t getTextureCoor(const Point& p) const override;
  void print() const override {};
  // Member functions
  std::optional<Vector3D> getNormalVector(const Point& p) const;
  void addTriangle(const Triangle& triangle);
  void addTriangle(Triangle&& triangle);

 private:
  int lineIntersection(double a, double b, double c) const;
};

#endif  // _TRIANGLE_MESH_
