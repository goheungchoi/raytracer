#ifndef _POINT_
#define _POINT_


#include <iostream>
#include <cmath>

class Point
{
public:
    double x;
    double y;
    double z;
    Point();
    Point(double x, double y, double z);
    Point(const Point &p);
    Point operator+(const Point &p);
    Point operator-(const Point &p);
    Point operator*(const Point &p);
    Point operator*(const double &n);
    Point operator/(const Point &p);
    Point operator/(const double &n);
    double getDistance(const Point &p);
    void print() const;

    friend bool operator==(const Point& l, const Point& r)
    {
        double e = 0.01;
        bool x = fabs(l.x - r.x) < e && -fabs(l.x-r.x) < e;
        bool y = fabs(l.y - r.y) < e && -fabs(l.y-r.y) < e;
        bool z = fabs(l.z - r.z) < e && -fabs(l.z-r.z) < e;
        
        return x && y && z; 
    }

    friend bool operator!=(const Point& l, const Point& r)
    {
        return !(l == r);
    }
};

#endif