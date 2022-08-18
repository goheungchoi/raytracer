#include "light.h"

Light::Light(I intensity)
{
    if(intensity.Ib >= 0 && intensity.Ib <= 1)
        this->intensity.Ib = intensity.Ib;
    if(intensity.Ig >= 0 && intensity.Ig <= 1)
        this->intensity.Ig = intensity.Ig;
    if(intensity.Ir >= 0 && intensity.Ir <= 1)
        this->intensity.Ir = intensity.Ir;
}

double Light::attenuationFunc(const Point& obj_p)
{
    return 1.0;
}

double PointLight::attenuationFunc(const Point& obj_p)
{
    if (Object::equals(0.0, c1) 
        && Object::equals(0.0, c2) 
        && Object::equals(0.0, c3))
        return 1.0;
    double distance = p.getDistance(obj_p);
    return 1.0 / (c1 + c2 * distance + c3 * distance * distance);
}

double PointLight::getTheta(const Point* p1)
{
    double dist = p.getDistance(*p1);
    return atan(radius / dist);
}

HighLight::HighLight(I intensity, Point p, Vector3D dir, double cutoff)
: Light(intensity)
{
    this->p = p; 
    this->dir = dir.getUnit(); 
    if (cutoff <= 90 && cutoff > 0)
        this->cutoff = cutoff;
}
/**
 * @brief 
 * 
 * @param obj_p point to be checked whether it is in cutoff angle
 * @return true 
 * @return false 
 */
bool HighLight::isInHighLight(const Point& obj_p)
{
    Vector3D v_o(p, obj_p);
    double rad = acos(dir.dotProd(v_o.getUnit()));
    double degree = rad * 180.0 / 3.14159265;
    return  degree <= cutoff;
}

double HighLight::attenuationFunc(const Point& obj_p)
{
    double amin = 0.5;
    double amax = 1.0;
    Vector3D v_o(p, obj_p);
    double att = 1 - (acos(dir.dotProd(v_o.getUnit())) * 180.0 / 3.14159265) / cutoff;
    double result = amin + (amax - amin) * att;
    return result;
}

double HighLight::getTheta(const Point* p1)
{
    double dist = p.getDistance(*p1);
    return atan(10 / dist);
}