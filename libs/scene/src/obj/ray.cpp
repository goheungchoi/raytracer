// ray.cpp
#include "obj/ray.h"

std::optional<Point> Ray::getIntersectionWithLine(Ray l) const {
    Vector3D g(o, l.o);
    Vector3D H = l.v * g;
    Vector3D K = l.v * v;
    bool same_dir = false;
    if (H.x * K.x >= 0 &&
        H.y * K.y >= 0 &&
        H.z * K.z >= 0)
        same_dir = true;

    double h = H.mag();
    double k = K.mag();
    if (h == 0 || k == 0)
        return std::nullopt;
    Vector3D temp(v * (h / k));
    if (same_dir)
        return Point(o + temp.toPoint());
    else
        return Point(o - temp.toPoint());
}

bool Ray::isInDistance(const Point& p, const double dist) const {
    if (!isOnRay(p))
        return false;
    return o.getDistance(p) <= dist;
}

bool Ray::isOnRay(const Point& p) const {
    double tx = (p.x - o.x) / v.x;
    double ty = (p.y - o.y) / v.y;
    double tz = (p.z - o.z) / v.z;
    if (tx < 0 || ty < 0 || tz < 0)
        return false;
    return util::almost_equal(tx, ty, 100) 
        && util::almost_equal(tx, tz, 100);
}