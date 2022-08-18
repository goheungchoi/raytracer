#include "obj/plane.h"

#include <math.h>

Plane::Plane() : Object("", color{0, 0, 0}, 1.0, 1.0) {
  this->n = Vector3D();
  this->p0 = Point();
  w = 10.0;
  h = 10.0;
}

Plane::Plane(std::string name, color clr, double opacity,
             double index_of_refraction, Vector3D n, Point p0, Vector3D dir,
             double w, double h)
    : Object(name, clr, opacity, index_of_refraction) {
  this->n = n.getUnit();
  this->p0 = p0;
  this->dir = dir;
  this->w = w;
  this->h = h;
  init();
}


void Plane::init() {
  Vector3D standard_vec(1, 0, 0);
  double theta = dir.getAngle(standard_vec);
  if (dir.y < 0) theta = -theta;
  T = Vector3D(-n).rotate(0, 90, theta).getUnit();
  B = (n * T).getUnit();

  Point scaler_h((T * (w / 2)).toPoint());
  Point scaler_v((B * (h / 2)).toPoint());
  p00 = p0 - scaler_h + scaler_v;
  p10 = p0 + scaler_h + scaler_v;
  p01 = p0 - scaler_h - scaler_v;
  p11 = p0 + scaler_h - scaler_v;

  c00 = text_coor{0, 0};
  c01 = text_coor{0, 1};
  c10 = text_coor{1, 0};
  c11 = text_coor{1, 1};
}

Point* Plane::getIntersection(const Line& l) {
  double t = 0.0;

  int isIntersection = Plane::lineIntersection(l);
  if (isIntersection > 0) {     // there exist intersection
    if (isIntersection == 1) {  // line is on the plane
      double dist00_01 = p00.getDistance(p01);
      double dist01_11 = p01.getDistance(p11);
      double dist00_10 = p10.getDistance(p00);
      double dist10_11 = p11.getDistance(p10);
      Line edge00_01(p00, p01);
      Line edge00_10(p00, p10);
      Line edge01_11(p11, p01);
      Line edge10_11(p11, p10);
      Point* intersection1 = edge00_01.getIntersectionWithLine(l);
      Point* intersection2 = edge00_10.getIntersectionWithLine(l);
      Point* intersection3 = edge01_11.getIntersectionWithLine(l);
      Point* intersection4 = edge10_11.getIntersectionWithLine(l);

      Point* intersection = nullptr;
      double shortest = INFINITY;
      double d1 = INFINITY;
      if (intersection1 != nullptr) {
        d1 = intersection1->getDistance(l.o);
        if (d1 < shortest) {
          if (edge00_01.isInRange(*intersection1, dist00_01)) {
            shortest = d1;
            intersection = intersection1;
          }
        }
      }
      double d2 = INFINITY;
      if (intersection2 != nullptr) {
        d2 = intersection2->getDistance(l.o);
        if (d2 < shortest) {
          if (edge00_10.isInRange(*intersection1, dist00_10)) {
            shortest = d2;
            intersection = intersection2;
          }
        }
      }
      double d3 = INFINITY;
      if (intersection3 != nullptr) {
        d3 = intersection3->getDistance(l.o);
        if (d3 < shortest) {
          if (edge01_11.isInRange(*intersection3, dist01_11)) {
            shortest = d3;
            intersection = intersection3;
          }
        }
      }
      double d4 = INFINITY;
      if (intersection4 != nullptr) {
        d4 = intersection4->getDistance(l.o);
        if (d4 < shortest) {
          if (edge10_11.isInRange(*intersection4, dist10_11)) {
            shortest = d4;
            intersection = intersection4;
          }
        }
      }
      // returns the point closest from the starting point of the line
      return intersection;
    } else {  // one intersection
      Vector3D temp(l.o, p0);
      t = n.dotProd(temp) / n.dotProd(l.v);
      if (t <= 0) return nullptr;
    }
  } else {  // no intersection
    return nullptr;
  }
  // if the starting point of the line lays on the plane
  if (Object::equals(t, 0.0)) return nullptr;
  // calculate x y z coordinates
  double x = l.o.x + l.v.x * t;
  double y = l.o.y + l.v.y * t;
  double z = l.o.z + l.v.z * t;
  Point intersection(x, y, z);

  if (isOnSurface(intersection))
    return new Point(x, y, z);
  else
    return nullptr;
}

int Plane::lineIntersection(const Line& l) {
  double d = n.dotProd(l.v);
  // line and plane are parallel
  if (Object::equals(d, 0.0)) {
    Vector3D temp(l.o, p0);
    if (Object::equals(n.dotProd(temp), 0.0))
      return 1;  // line is on the plane
    else
      return 0;
  }
  // one intersection
  else
    return 2;
}

Vector3D* Plane::getNormalVector(const Point& p, const Line& l) {
  Vector3D unit_n = n;
  if (normal_mapping) {
    text_coor tc = getTextureCoor(p);
    Point vec = normal_map.getBilinearInterpolation(tc);
    Vector3D m(vec.x, vec.y, vec.z);
    Vector3D normal_vector(rotateVectorTBN(m));
    // is the ray inside the object?
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

Point* Plane::getOdLambda(const Point& p) {
  if (texture_mapping) {
    text_coor tc = getTextureCoor(p);
    Point vec = texture_map.getBilinearInterpolation(tc);
    return new Point(vec);
  } else
    return new Point(clr.dr, clr.dg, clr.db);
}

text_coor Plane::getTextureCoor(const Point& p) {
  // vector from upper left conner to the point
  Vector3D p00p(p00, p);

  // vertical coor
  Vector3D e00_01(p00, p01);
  double ver_area = (e00_01 * p00p).getMag();
  double ver_base = p00.getDistance(p01);
  double vertical_l = ver_area / ver_base;
  double v_ratio = vertical_l / ver_base;
  v_ratio = 1 - v_ratio;
  // horizontal coor
  Vector3D e00_10(p00, p10);
  double hor_area = (e00_10 * p00p).getMag();
  double hor_base = p00.getDistance(p01);
  double horizontal_l = hor_area / hor_base;
  double h_ratio = horizontal_l / hor_base;
  h_ratio = 1 - h_ratio;

  if (v_ratio > 1) {
    if (Object::equals(v_ratio, 1.0))
      v_ratio = 1;
    else {
      std::string error_msg = "texture coordinate error: ";
      p.print();
      throw std::invalid_argument(error_msg.append(std::to_string(v_ratio))
                                      .append(" ")
                                      .append(std::to_string(h_ratio)));
    }
  }
  if (h_ratio > 1) {
    if (Object::equals(h_ratio, 1.0))
      h_ratio = 1;
    else {
      std::string error_msg = "texture coordinate error: ";
      p.print();
      throw std::invalid_argument(error_msg.append(std::to_string(v_ratio))
                                      .append(" ")
                                      .append(std::to_string(h_ratio)));
    }
  }

  text_coor tc{v_ratio, h_ratio};
  return tc;
}

bool Plane::isOnSurface(const Point& p) {
  double result = n.dotProd(Vector3D(p0, p));
  if (Object::equals(result, 0.0)) {
    b_plane_coor barycentric_coor = getBarycentricCoor(p);
    return isBarycentricCoorOnPlane(
        barycentric_coor.alpha, barycentric_coor.beta, barycentric_coor.gamma,
        barycentric_coor.delta);
  } else
    return false;
}

b_plane_coor Plane::getBarycentricCoor(const Point& p) {
  Vector3D e00_10(p00, p10);
  Vector3D e10_11(p10, p11);
  Vector3D e11_01(p11, p01);
  Vector3D e01_00(p01, p00);
  Vector3D pp00(p00, p);
  Vector3D pp10(p10, p);
  Vector3D pp01(p01, p);
  Vector3D pp11(p11, p);
  double a = (e00_10 * pp00).getMag() / 2.0;
  double b = (e10_11 * pp10).getMag() / 2.0;
  double c = (e11_01 * pp11).getMag() / 2.0;
  double d = (e01_00 * pp01).getMag() / 2.0;
  double A = w * h;

  double alpha = a / A;
  double beta = b / A;
  double gamma = c / A;
  double delta = d / A;
  return b_plane_coor{alpha, beta, gamma, delta};
}

bool Plane::isBarycentricCoorOnPlane(double alpha, double beta, double gamma,
                                     double delta) {
  double sum = alpha + beta + gamma + delta;
  return Object::equals(sum, 1.0) && 0 <= alpha && alpha <= 1 && 0 <= beta &&
         beta <= 1 && 0 <= gamma && gamma <= 1 && 0 <= delta && delta <= 1;
}

Vector3D Plane::rotateVectorTBN(const Vector3D& m) {
  double nx = T.x * m.x + B.x * m.y + n.x * m.z;
  double ny = T.y * m.x + B.y * m.y + n.y * m.z;
  double nz = T.z * m.x + B.z * m.y + n.z * m.z;
  return Vector3D(nx, ny, nz);
}