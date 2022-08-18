#include "obj/triangle.h"

Triangle::Triangle() : Object("", color{0, 0, 0}, 1.0, 1.0) {}

Triangle::Triangle(std::string name, color clr, double opacity,
                   double index_of_refraction, Point p0, Point p1, Point p2)
    : Object(name, clr, opacity, index_of_refraction) {
  this->p0 = p0;
  this->p1 = p1;
  this->p2 = p2;

  this->e1 = Vector3D(p0, p1);
  this->e2 = Vector3D(p0, p2);
  N = (e1 * e2).getUnit();
  A = (e1 * e2).getMag() / 2;
}

void Triangle::getTextureVectors() {
  if (!texture_mapping || !normal_mapping) return;

  double du1 = c1.u - c0.u;
  double dv1 = c1.v - c0.v;
  double du2 = c2.u - c1.u;
  double dv2 = c2.v - c1.v;

  double temp = du1 * dv2 - dv1 * du2;

  Point edge1 = p1 - p0;
  Point edge2 = p2 - p1;

  Point tempT = (edge1 * dv2 - edge2 * dv1) / temp;
  Point tempB = (edge2 * du1 - edge1 * du2) / temp;

  B = Vector3D(tempB.x, tempB.y, tempB.z);
  T = Vector3D(tempT.x, tempT.y, tempT.z);
}

Point* Triangle::getIntersection(const Line& l) {
  double t = 0.0;

  int isIntersection = Triangle::lineIntersection(l);
  if (isIntersection > 0) {  // there exist intersection
    if (isIntersection == 1) {  // line is on the plane
      double dist1 = p0.getDistance(p1);
      double dist2 = p0.getDistance(p2);
      double dist3 = p1.getDistance(p2);
      Vector3D e3(p1, p2);
      Line edge1(p0, e1);
      Line edge2(p0, e2);
      Line edge3(p1, e3);
      Point* intersection1 = edge1.getIntersectionWithLine(l);
      Point* intersection2 = edge2.getIntersectionWithLine(l);
      Point* intersection3 = edge3.getIntersectionWithLine(l);

      Point* intersection = nullptr;
      double shortest = INFINITY;
      double d1 = INFINITY;
      if (intersection1 != nullptr) {
        d1 = intersection1->getDistance(l.o);
        if (d1 < shortest) {
          if (edge1.isInRange(*intersection1, dist1)) {
            shortest = d1;
            intersection = intersection1;
          }
        }
      }
      double d2 = INFINITY;
      if (intersection2 != nullptr) {
        d2 = intersection2->getDistance(l.o);
        if (d2 < shortest) {
          if (edge2.isInRange(*intersection2, dist2)) {
            shortest = d2;
            intersection = intersection2;
          }
        }
      }
      double d3 = INFINITY;
      if (intersection3 != nullptr) {
        d3 = intersection3->getDistance(l.o);
        if (d3 < shortest) {
          if (edge3.isInRange(*intersection3, dist3)) {
            shortest = d3;
            intersection = intersection3;
          }
        }
      }

      return intersection;
    } else {  // one intersection
      Vector3D temp(l.o, p0);
      t = N.dotProd(temp) / N.dotProd(l.v);
      if (t <= 0) return nullptr;
    }
  } else {  // no intersection
    return nullptr;
  }
  // if l.o lays on the plane
  if (Object::equals(t, 0.0)) return nullptr;
  // calculate x y z coordinates
  double x = l.o.x + l.v.x * t;
  double y = l.o.y + l.v.y * t;
  double z = l.o.z + l.v.z * t;

  Point p(x, y, z);
  // if the point is on the triangle
  if (isInTriangle(p)) {
    return new Point(x, y, z);
  } else
    return nullptr;
}

int Triangle::lineIntersection(const Line& l) {
  double d = N.dotProd(l.v);
  // line and plane are parallel
  if (Object::equals(d, 0.0)) {
    Vector3D temp(l.o, p0);
    if (Object::equals(N.dotProd(temp), 0.0))
      return 1;  // line is on the plane
    else
      return 0;
  }
  // one intersection
  else
    return 2;
}

Vector3D* Triangle::getNormalVector(const Point& p) {
  // if the point is in the triangle
  if (isInTriangle(p)) {
    if (smooth_shading) {  // smooth shading
      Point bary_coor = getBarycentricCoor(p);
      double alpha = bary_coor.x;
      double beta = bary_coor.y;
      double gamma = bary_coor.z;
      double sum = alpha + beta + gamma;
      Vector3D tempA = vn0 * alpha;
      Vector3D tempB = vn1 * beta;
      Vector3D tempC = vn2 * gamma;
      Vector3D temp = tempA + tempB + tempC;
      Vector3D v = temp.getUnit();
      return new Vector3D(v.x, v.y, v.z);
    } else {  // flat shading
      return new Vector3D(N.x, N.y, N.z);
    }
  } else
    return nullptr;
}

Vector3D* Triangle::getNormalVector(const Point& p, const Line& l) {
  Vector3D* normal_vector = getNormalVector(p);
  if (normal_vector != nullptr) {
    Vector3D unit_n(*normal_vector);
    delete normal_vector;
    if (normal_mapping) {
      text_coor tc = getTextureCoor(p);
      Point vec = normal_map.getBilinearInterpolation(tc);
      Vector3D m(vec.x, vec.y, vec.z);
      Vector3D normal_vector(rotateVectorTBN(m));
      // is the ray inside the sphere?
      Vector3D v(l.v);
      // if surface_normal~I > inversed_surface_normal~I => ray is inside
      if ((-v).getAngle(normal_vector) > (-v).getAngle(-normal_vector))
        return new Vector3D(-normal_vector);
      else
        return new Vector3D(normal_vector);
    } else {
      Vector3D v(l.v);
      if ((-v).getAngle(unit_n) > (-v).getAngle(-unit_n))
        return new Vector3D(-unit_n);
      else
        return new Vector3D(unit_n);
    }
  }
  return normal_vector;
}

Point* Triangle::getOdLambda(const Point& p) {
  return new Point(clr.dr, clr.dg, clr.db);
}

Vector3D Triangle::rotateVectorTBN(const Vector3D& m) {
  double nx = T.x * m.x + B.x * m.y + N.x * m.z;
  double ny = T.y * m.x + B.y * m.y + N.y * m.z;
  double nz = T.z * m.x + B.z * m.y + N.z * m.z;
  return Vector3D(nx, ny, nz);
}

text_coor Triangle::getTextureCoor(const Point& p) {
  Point bary_coor = getBarycentricCoor(p);
  double alpha = bary_coor.x;
  double beta = bary_coor.y;
  double gamma = bary_coor.z;
  return c0 * alpha + c1 * beta + c2 * gamma;
}

Point Triangle::getBarycentricCoor(const Point& p) {
  Vector3D e3(p1, p);
  Vector3D e4(p2, p);
  double a = (e3 * e4).getMag() / 2;
  double b = (e4 * e2).getMag() / 2;
  double c = (e1 * e3).getMag() / 2;
  double alpha = a / A;
  double beta = b / A;
  double gamma = c / A;
  return Point(alpha, beta, gamma);
}

bool Triangle::isBarycentricCoorInTriangle(double alpha, double beta,
                                           double gamma) {
  double sum = alpha + beta + gamma;
  return Object::equals(sum, 1.0) && 0 <= alpha && alpha <= 1 && 0 <= beta &&
         beta <= 1 && 0 <= gamma && gamma <= 1;
}

bool Triangle::isInTriangle(const Point& p) {
  double d = N.dotProd(Vector3D(p0, p));
  if (!Object::equals(d, 0.0)) return false;
  Point bary_coor = getBarycentricCoor(p);
  double alpha = bary_coor.x;
  double beta = bary_coor.y;
  double gamma = bary_coor.z;
  return isBarycentricCoorInTriangle(alpha, beta, gamma);
}

bool Triangle::isTextureMapping() { return texture_mapping; }

bool Triangle::isNormalMapping() { return normal_mapping; }
