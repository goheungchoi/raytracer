#ifndef _LINE_
#define _LINE_

#include "vector3D.h"

class Line
{
public:
    Point o;
    Vector3D v;
    Line(Point tl, Point hd);
    Line(Point o, Vector3D v);
    Point* getIntersectionWithLine(Line l)
    {
        Vector3D g(o, l.o);
        Vector3D H = l.v * g;
        Vector3D K = l.v * v;
        bool same_dir = false;
        if (H.x * K.x >= 0 &&
            H.y * K.y >= 0 &&
            H.z * K.z >= 0)
            same_dir = true;

        double h = H.getMag();
        double k = K.getMag();
        if (h == 0 || k == 0)
            return nullptr;
        Vector3D temp(v * (h / k));
        if (same_dir)
            return new Point(o + temp.toPoint());
        else
            return new Point(o - temp.toPoint());
    }
    bool isInRange(const Point& p, const double& dist)
    {
        if (!isOnRay(p))
            return false;
        return o.getDistance(p) <= dist;
    }
    bool isOnRay(const Point& p)
    {
        double tx = (p.x - o.x) / v.x;
        double ty = (p.y - o.y) / v.y;
        double tz = (p.z - o.z) / v.z;
        if (tx < 0 || ty < 0 || tz < 0)
            return false;
        return equals(tx, ty) && equals(tx, tz);
    }
private:
    bool equals(double a, double b) {
        double e = 0.000001;
        return fabs(a - b) < e && -fabs(a-b) < e;
    };
};


#endif