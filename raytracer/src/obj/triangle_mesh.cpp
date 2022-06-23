#include "triangle_mesh.h"

TriangleMesh::TriangleMesh() : Object("", color{0, 0, 0}, 1.0, 1.0) {
  this->triangles = new std::vector<Triangle*>();
}

TriangleMesh::TriangleMesh(std::string name, color clr, double opacity,
                           double index_of_refraction)
    : Object(name, clr, opacity, index_of_refraction) {
  this->triangles = new std::vector<Triangle*>();
}

TriangleMesh::~TriangleMesh() {
  for (Triangle* tri : *triangles) delete tri;
  delete triangles;
}

Point* TriangleMesh::getIntersection(const Line& l) {
  for (Triangle* tri : *triangles) {
    Point* p = tri->getIntersection(l);
    if (p != nullptr) return p;
  }
  return nullptr;
}

Vector3D* TriangleMesh::getNormalVector(const Point& p) {
  for (Triangle* tri : *triangles) {
    Vector3D* n = tri->getNormalVector(p);
    if (n != nullptr) {
      if (tri->isNormalMapping() && normal_mapping) {
        text_coor tc = tri->getTextureCoor(p);
        Point vec = normal_map.getBilinearInterpolation(tc);
        Vector3D m(vec.x, vec.y, vec.z);
        return new Vector3D(tri->rotateVectorTBN(m));
      } else
        return n;
    }
  }
  return nullptr;
}

Vector3D* TriangleMesh::getNormalVector(const Point& p, const Line& l) {
  for (Triangle* tri : *triangles) {
    Vector3D* n = tri->getNormalVector(p);
    if (n != nullptr) {
      if (tri->isNormalMapping() && normal_mapping) {
        text_coor tc = tri->getTextureCoor(p);
        Point vec = normal_map.getBilinearInterpolation(tc);
        Vector3D m(vec.x, vec.y, vec.z);
        Vector3D normal_vector(tri->rotateVectorTBN(m));
        // is the ray inside the triangle mesh?
        Vector3D v(l.v);
        // if surface_normal~I > inversed_surface_normal~I => ray is inside
        if ((-v).getAngle(normal_vector) > (-v).getAngle(-normal_vector))
          return new Vector3D(-normal_vector);
        else
          return new Vector3D(normal_vector);
      } else {
        Vector3D v(l.v);
        if ((-v).getAngle(*n) > (-v).getAngle(-*n))
          return new Vector3D(-*n);
        else
          return new Vector3D(*n);
      }
    }
  }
  return nullptr;
}

Point* TriangleMesh::getOdLambda(const Point& p) {
  for (Triangle* tri : *triangles) {
    if (tri->isInTriangle(p)) {
      if (tri->isTextureMapping() && texture_mapping) {
        text_coor tc = tri->getTextureCoor(p);
        Point vec = texture_map.getBilinearInterpolation(tc);
        return new Point(vec);
      } else
        return tri->getOdLambda(p);
    }
  }
  return nullptr;
}

int TriangleMesh::lineIntersection(double a, double b, double c) { return 0; }
