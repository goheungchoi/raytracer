#include "obj/point.h"

double Point::getDistance(const Point &p) const {
    double tx = x - p.x, ty = y - p.y, tz = z - p.z;
    return std::sqrt(tx*tx + ty*ty + tz*tz);
}

double getDistance(const Point& l, const Point& r) {
    return l.getDistance(r);
}

void Point::print() const {
    std::cout << "Point: (" << x << ", " << y << ", " << z << ")" << std::endl;
}