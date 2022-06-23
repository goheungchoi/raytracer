#ifndef _SPHERE_
#define _SPHERE_

#include "object.h"

class Sphere : virtual public Object
{
public:
    Point origin;
    double radius;

    Sphere();
    Sphere(std::string name, color clr, double opacity, double index_of_refraction, Point origin, double radius);
    virtual ~Sphere() {};
    Point* getIntersection(const Line& l);
    Vector3D* getNormalVector(const Point& p, const Line& l) override;
    Point* getOdLambda(const Point& p);
    bool isOnSurface(const Point& p);
    text_coor getTextureCoor(const Point& p);
    Vector3D rotateVectorTBN(Vector3D& m, Vector3D& N);
private:
    int lineIntersection(double a, double b, double c);
};

#endif