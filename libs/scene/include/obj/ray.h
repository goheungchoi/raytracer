// ray.h
#ifndef _RAY_H_
#define _RAY_H_

#include "util/util.h"

#include "vector3D.h"

class Ray {
public:
    Point o;
    Vector3D v;
    // Default constructor
    constexpr Ray();
    // Parameterized constructors
    constexpr Ray(Point o, Vector3D v) : o(o), v(v.unit()) {};
    constexpr Ray(Point tl, Point hd) : o(tl), v(Vector3D(tl, hd).unit()) {};
    
    // Member functions
    std::optional<Point> getIntersectionWithLine(Ray l) const;
    bool isInDistance(const Point& p, const double dist) const;
    bool isOnRay(const Point& p) const;
};


#endif // _RAY_H_
