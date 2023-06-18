#include "obj/plane.h"

#include <math.h>

Plane::Plane() 
: Object({0, 0, 0}, 1.0, 1.0), 
n(Vector3D()), dir(Vector3D()), p0(Point()), w(10.0), h(10.0) {}

Plane::Plane(obj::color_t color, double opacity, double iRefraction, 
Vector3D n, Point p0, Vector3D dir, double w, double h)
: Object(color, opacity, iRefraction), n(n.unit()), p0(p0), dir(dir), w(w), h(h) 
{ init(); }

std::optional<Point> Plane::getIntersection(const Ray& r) const {
  double t = 0.0;

  int res = Plane::lineIntersection(r);
  if (res == NO_INTERSECTION) {  // No interaction
    return std::nullopt; 
  } else if (res == INF_INTERSECTION) {  // The line is on the plane
    // Find the intersection point on the edge that is closest
    // to the ray source.
    double distances[4] = {
      p00.getDistance(p01), p01.getDistance(p11),
      p10.getDistance(p00), p11.getDistance(p10)
    };

    Ray edges[4] = {
      Ray(p00, p01), Ray(p00, p10),
      Ray(p11, p01), Ray(p11, p10)
    };

    std::optional<Point> closestInteraction;
    double closestDistance = INFINITY;
    for (int i=0; i<4; i++) {
      auto intersection = edges[i].getIntersectionWithLine(r);
      if (intersection && edges[i].isInDistance((*intersection), distances[i])) {
        double d = (*intersection).getDistance(r.o);
        if (d < closestDistance) {
          closestInteraction = std::move(intersection);
          closestDistance = d;
        }
      }
    }
    // returns the point closest from the starting point of the line
    return closestInteraction;
  } else if (res == ONE_INTERSECTION) {  // One intersection
    Vector3D temp(r.o, p0);
    t = n.dot(temp) / n.dot(r.v);
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
  Point intersection(x, y, z);

  if (isOnSurface(intersection))
    return intersection;
  else
    return std::nullopt;
}

std::optional<Vector3D> Plane::getNormalVector(
  const Point& p, const Ray& r) const {
  Vector3D unitN(n);
  if (normal_mapping) {
    obj::text_coor_t tc = getTextureCoor(p);
    Vector3D m(normalMap.getBilinearInterpolation(tc));
    Vector3D newN(rotateVectorTBN(m));
    // is the ray inside the object?
    Vector3D v(r.v);
    // if surface_normal~I > inversed_surface_normal~I 
    // => ray is inside
    if ((-v).getAngle(newN) > (-v).getAngle(-newN))
      return -newN;
    else
      return newN;
  } else {
    Vector3D v(r.v);
    if ((-v).getAngle(unitN) > (-v).getAngle(-unitN))
      return -unitN;
    else
      return unitN;
  }
}

std::optional<util::arr3> Plane::getOdLambda(const Point& p) const {
  if (texture_mapping) {
    obj::text_coor_t tc = getTextureCoor(p);
    return textureMap.getBilinearInterpolation(tc);
  } else
    return util::arr3{color.dr, color.dg, color.db};
}

obj::text_coor_t Plane::getTextureCoor(const Point& p) const {
  // vector from upper left conner to the point
  Vector3D p00p(p00, p);

  // vertical coor
  Vector3D e00_01(p00, p01);
  double ver_area = (e00_01*p00p).mag();
  double ver_base = p00.getDistance(p01);
  double vertical_l = ver_area / ver_base;
  double v_ratio = 1.0 - vertical_l/ver_base;
  // horizontal coor
  Vector3D e00_10(p00, p10);
  double hor_area = (e00_10*p00p).mag();
  double hor_base = p00.getDistance(p10);
  double horizontal_l = hor_area / hor_base;
  double h_ratio = 1.0 - horizontal_l/hor_base;

  // vertical ratio error check
  if (v_ratio > 1.0) {
    if (util::almost_equal(v_ratio, 1.0, 100))
      v_ratio = 1.0;
    else {
      std::string error_msg = "texture coordinate error: ";
      p.print();
      throw std::invalid_argument(
        error_msg
        .append(std::to_string(v_ratio))
        .append(" ")
        .append(std::to_string(h_ratio))
      );
    }
  }
  // horizontal ratio error check
  if (h_ratio > 1.0) {
    if (util::almost_equal(h_ratio, 1.0, 100))
      h_ratio = 1.0;
    else {
      std::string error_msg = "texture coordinate error: ";
      p.print();
      throw std::invalid_argument(
        error_msg
        .append(std::to_string(v_ratio))
        .append(" ")
        .append(std::to_string(h_ratio))
      );
    }
  }

  return {v_ratio, h_ratio};
}

b_plane_coor Plane::getBarycentricCoor(const Point& p) const {
  Vector3D e00_10(p00, p10), e10_11(p10, p11);
  Vector3D e11_01(p11, p01), e01_00(p01, p00);
  Vector3D pp00(p00, p), pp10(p10, p);
  Vector3D pp01(p01, p), pp11(p11, p);
  double a = (e00_10*pp00).mag() / 2.0;
  double b = (e10_11*pp10).mag() / 2.0;
  double c = (e11_01*pp11).mag() / 2.0;
  double d = (e01_00*pp01).mag() / 2.0;
  double A = w * h;

  double alpha = a / A;
  double beta = b / A;
  double gamma = c / A;
  double delta = d / A;
  return {alpha, beta, gamma, delta};
}

Vector3D Plane::rotateVectorTBN(const Vector3D& m) const {
  double nx = T.x * m.x + B.x * m.y + n.x * m.z;
  double ny = T.y * m.x + B.y * m.y + n.y * m.z;
  double nz = T.z * m.x + B.z * m.y + n.z * m.z;
  return Vector3D(nx, ny, nz);
}
