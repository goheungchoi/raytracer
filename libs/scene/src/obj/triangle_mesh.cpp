#include "obj/triangle_mesh.h"

// TODO: Make BVH of triangles.

std::optional<Point> TriangleMesh::getIntersection(const Ray& r) const {
  if (triangles.empty()) return std::nullopt;

  // Find the point closest to the ray origin.
  std::optional<Point> closestPoint;
  double closestDistance = INFINITY;
  for (const auto& triangle : triangles) {
    if (auto p = (*triangle).getIntersection(r)) {
      double distance = (*p).getDistance(r.o);
      if (!closestPoint || distance < closestDistance) {
        closestPoint = p;
        closestDistance = distance;
      }
    }
  }
  return closestPoint;
}

std::optional<Vector3D> TriangleMesh::getNormalVector(const Point& p) const {
  for (auto& triangle : triangles) {
    std::optional<Vector3D> n ;
    if (auto n = (*triangle).getNormalVector(p)) {
      if ((*triangle).isNormalMapping() && normal_mapping) {
        obj::text_coor_t tc = (*triangle).getTextureCoor(p);
        Vector3D m(normalMap.getBilinearInterpolation(tc));
        return Vector3D(triangle->rotateVectorTBN(m));
      }
      return n;
    }
  }
  return std::nullopt;
}

std::optional<Vector3D> TriangleMesh::getNormalVector(const Point& p, const Ray& l) const {
  for (auto& triangle : triangles) {
    if (auto normal = (*triangle).getNormalVector(p)) {
      if ((*triangle).isNormalMapping() && normal_mapping) {  // when normal mapped
        obj::text_coor_t tc = (*triangle).getTextureCoor(p);
        Vector3D m(normalMap.getBilinearInterpolation(tc));
        Vector3D n((*triangle).rotateVectorTBN(m));
        // is the ray inside the triangle mesh?
        Vector3D v(l.v);
        // if surface_normal~I > inversed_surface_normal~I 
        // => ray is inside
        if ((-v).getAngle(n) > (-v).getAngle(-n))
          return -n;
        else
          return n;
      } else {  // default case
        Vector3D v(l.v);
        if ((-v).getAngle(*normal) > (-v).getAngle(-*normal))
          return -(*normal);
        else
          return (*normal);
      }
    }
  }
  return std::nullopt;
}

std::optional<util::arr3> TriangleMesh::getOdLambda(const Point& p) const {
  for (auto& triangle : triangles) {
    if ((*triangle).isInTriangle(p)) {
      if ((*triangle).isTextureMapping() && texture_mapping) {
        obj::text_coor_t tc = (*triangle).getTextureCoor(p);
        return textureMap.getBilinearInterpolation(tc);
      }
      return (*triangle).getOdLambda(p);
    }
  }
  return std::nullopt;
}

obj::text_coor_t TriangleMesh::getTextureCoor(const Point& p) const {
  return {0.0, 0.0};
}

void TriangleMesh::addTriangle(const Triangle& triangle) {
  triangles.push_back(std::make_unique<Triangle>(triangle));
}

void TriangleMesh::addTriangle(Triangle&& triangle) {
  triangles.push_back(std::make_unique<Triangle>(std::move(triangle)));
}

int TriangleMesh::lineIntersection(double a, double b, double c) const { return 0; }
