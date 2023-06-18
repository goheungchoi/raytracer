#include "obj/triangle.h"

Triangle::Triangle() : Object({0, 0, 0}, 1.0, 1.0) {}

Triangle::Triangle(obj::color_t color, double opacity, double iRefraction, 
  Point p0, Point p1, Point p2)
: Object(color, opacity, iRefraction), p0(p0), p1(p1), p2(p2) {
  e1 = Vector3D(p0, p1);
  e2 = Vector3D(p0, p2);
  N = (e1*e2).unit();
  A = (e1*e2).mag()/2.0;
}

void Triangle::getTextureVectors() {
  if (!texture_mapping || !normal_mapping) return;

  double du1 = c1.u - c0.u;
  double dv1 = c1.v - c0.v;
  double du2 = c2.u - c1.u;
  double dv2 = c2.v - c1.v;

  double temp = du1*dv2 - dv1*du2;

  Point edge1 = p1 - p0;
  Point edge2 = p2 - p1;

  Point tempT = (edge1*dv2 - edge2*dv1) / temp;
  Point tempB = (edge2*du1 - edge1*du2) / temp;

  B = Vector3D(tempB.x, tempB.y, tempB.z);
  T = Vector3D(tempT.x, tempT.y, tempT.z);
}

std::optional<Point> Triangle::getIntersection(const Ray& r) const {
  double t = 0.0;
  int res = Triangle::lineIntersection(r);
  if (res == NO_INTERSECTION) {  // No intersection
    return std::nullopt;
  } else if (res == INF_INTERSECTION) {  // line is on the plane
    Vector3D e3(p1, p2);
    double distances[3] = { p0.getDistance(p1), p0.getDistance(p2), p1.getDistance(p2) };
    Ray edges[3] = { Ray(p0, e1), Ray(p0, e2), Ray(p1, e3) };

    std::optional<Point> closestIntersection;
    double closestDistance = INFINITY;
    for (int i=0; i<3; i++) {
      auto intersection = edges[i].getIntersectionWithLine(r);
      if (intersection && edges[i].isInDistance((*intersection), distances[i])) {
        double d = (*intersection).getDistance(r.o);
        if (d < closestDistance) {
          closestIntersection = std::move(intersection);
          closestDistance = d;
        }
      }
    }
    // returns the point closest from the starting point of the line
    return closestIntersection;
  } else if (res == ONE_INTERSECTION) {  // one intersection
    Vector3D temp(r.o, p0);
    t = N.dot(temp) / N.dot(r.v);
  } else {  // This should never happen
    throw std::invalid_argument("error: Plane::getIntersection: something went wrong:(");
  }
  // If the starting point of the line lays on the plane
  // or the plane is behind the starting point of the line.
  if (t < 0 || util::almost_equal(t, 0.0, 100)) 
    return std::nullopt;
  // calculate x y z coordinates
  double x = r.o.x + r.v.x*t;
  double y = r.o.y + r.v.y*t;
  double z = r.o.z + r.v.z*t;

  Point p(x, y, z);
  // if the point is on the triangle
  if (isInTriangle(p))
    return p;
  else
    return std::nullopt;
}

std::optional<Vector3D> Triangle::getNormalVector(const Point& p) const {
  // if the point is in the triangle
  if (isInTriangle(p)) {
    if (smooth_shading) {  // smooth shading
      util::arr3 bary_coor = getBarycentricCoor(p);
      double alpha = bary_coor.x;
      double beta = bary_coor.y;
      double gamma = bary_coor.z;
      double sum = alpha + beta + gamma;
      Vector3D tempA = vn0*alpha;
      Vector3D tempB = vn1*beta;
      Vector3D tempC = vn2*gamma;
      Vector3D temp = tempA + tempB + tempC;
      return temp.unit();
    } else {  // flat shading
      return N.unit();
    }
  } else
    return std::nullopt;
}

std::optional<Vector3D> Triangle::getNormalVector(const Point& p, const Ray& r) const {
  auto normal = getNormalVector(p);
  if (normal) {
    Vector3D unitN(std::move(*normal));
    if (normal_mapping) {
      obj::text_coor_t tc = getTextureCoor(p);
      Vector3D m(normalMap.getBilinearInterpolation(tc));
      Vector3D n(rotateVectorTBN(m));
      Vector3D v(r.v);
      // if surface_normal~I > inversed_surface_normal~I 
      // => ray is on the opposite side
      if ((-v).getAngle(n) > (-v).getAngle(-n))
        return -n;
      else
        return n;
    } else {
      Vector3D v(r.v);
      if ((-v).getAngle(unitN) > (-v).getAngle(-unitN))
        return -unitN;
      else
        return unitN;
    }
  }
  return normal;
}

std::optional<util::arr3> Triangle::getOdLambda(const Point& p) const {
  return util::arr3{color.dr, color.dg, color.db};
}

Vector3D Triangle::rotateVectorTBN(const Vector3D& m) const {
  double nx = T.x * m.x + B.x * m.y + N.x * m.z;
  double ny = T.y * m.x + B.y * m.y + N.y * m.z;
  double nz = T.z * m.x + B.z * m.y + N.z * m.z;
  return Vector3D(nx, ny, nz);
}

obj::text_coor_t Triangle::getTextureCoor(const Point& p) const {
  util::arr3 coor = getBarycentricCoor(p);
  return c0*coor.x + c1*coor.y + c2*coor.z;
}

util::arr3 Triangle::getBarycentricCoor(const Point& p) const {
  Vector3D e3(p1, p);
  Vector3D e4(p2, p);
  double a = (e3*e4).mag()/2.0;
  double b = (e4*e2).mag()/2.0;
  double c = (e1*e3).mag()/2.0;
  double alpha = a / A;
  double beta = b / A;
  double gamma = c / A;
  return {alpha, beta, gamma};
}
