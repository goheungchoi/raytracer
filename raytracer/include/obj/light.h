#ifndef _LIGHT_
#define _LIGHT_

#include <string>
#include <iostream>
#include <cmath>
#include "line.h"
#include "object.h"


typedef struct I {
    double Ir = 1.0;
    double Ig = 1.0;
    double Ib = 1.0;

    I (void) {
        Ir = 1.0;
        Ig = 1.0;
        Ib = 1.0;
    }

    I (double Ir, double Ig, double Ib) {
        this->Ir = Ir;
        this->Ig = Ig;
        this->Ib = Ib;
    }
    I operator+(const I& anotherI)
    {
        I temp; 
        temp.Ir = this->Ir + anotherI.Ir;
        temp.Ig = this->Ig + anotherI.Ig;
        temp.Ib = this->Ib + anotherI.Ib;
        return I(temp.Ir, temp.Ig, temp.Ib);
    }
    I operator*(const I& anotherI)
    {
        I temp; 
        temp.Ir = this->Ir * anotherI.Ir;
        temp.Ig = this->Ig * anotherI.Ig;
        temp.Ib = this->Ib * anotherI.Ib;
        return I(temp.Ir, temp.Ig, temp.Ib);
    }
    I operator*(const double& num)
    {
        I temp; 
        temp.Ir = this->Ir * num;
        temp.Ig = this->Ig * num;
        temp.Ib = this->Ib * num;
        return I(temp.Ir, temp.Ig, temp.Ib);
    }
    I operator/(const double& num)
    {
        I temp; 
        temp.Ir = this->Ir / num;
        temp.Ig = this->Ig / num;
        temp.Ib = this->Ib / num;
        return I(temp.Ir, temp.Ig, temp.Ib);
    }

    void scale() {
        if (this->Ir > 1)
            this->Ir = 1;

        if (this->Ig > 1)
            this->Ig = 1;

        if (this->Ib > 1)
            this->Ib = 1;

    }
} I;

class Light
{
public:
    I intensity;
    Light();
    Light(I intensity);
    virtual ~Light() {};
    virtual double attenuationFunc(const Point& obj_p);
};

class DirectionalLight : virtual public Light
{
public:
    Vector3D dir;
    DirectionalLight() : Light() {};
    DirectionalLight(I intensity, Vector3D dir) : Light(intensity)
    {this->dir = dir.getUnit();};
};

class PointLight : virtual public Light
{
public:
    Point p;
    double c1 = 0.0, c2 = 0.0, c3 = 0.0;
    double radius = 0.0;
    PointLight() : Light() {};
    PointLight(I intensity, Point p, double radius) : Light(intensity)
    {this->radius = radius; this->p = p;};
    void setAttenuation(double c1, double c2, double c3)
    {
        // if c1, c2, c3 = 0, don't assign
        if (Object::equals(0.0, c1) 
            && Object::equals(0.0, c2) 
            && Object::equals(0.0, c3)) {} 
        else
        {
            this->c1 = c1;
            this->c2 = c2;
            this->c3 = c3;
        }
    };
    double attenuationFunc(const Point& obj_p);
    double getTheta(const Point* p1);
};

class HighLight : virtual public Light
{
public:
    Point p;
    Vector3D dir;
    double cutoff;
    HighLight() : Light() {};
    HighLight(I intensity, Point p, Vector3D dir, double cutoff);
    bool isInHighLight(const Point& obj_p);
    double attenuationFunc(const Point& obj_p);
    double getTheta(const Point* p1);
};

#endif