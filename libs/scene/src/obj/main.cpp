#include "util/util.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <stdlib.h>
#include <math.h>
#include "obj/object.h"
#include "obj/point.h"
#include "obj/sphere.h"
#include "obj/plane.h"
#include "obj/vector3D.h"

typedef struct I {
    double Ir = 1.0;
    double Ig = 1.0;
    double Ib = 1.0;
    I operator+(const I& anotherI)
    {
        I temp; 
        temp.Ir = this->Ir + anotherI.Ir;
        temp.Ig = this->Ig + anotherI.Ig;
        temp.Ib = this->Ib + anotherI.Ib;
        return temp;
    }
    I operator/(const double& num)
    {
        I temp; 
        temp.Ir = this->Ir / num;
        temp.Ig = this->Ig / num;
        temp.Ib = this->Ib / num;
        return temp;
    }
} I;

int main() {

    Point origin(-4, 0, -10);
    Sphere sphere({0.5, 1, 0,1,0,0,0.2,0.4,0.2,20}, 
        1.0, 1.0, origin, 2);

    Point none(-4.26279, 1.94516, -9.61619);
    std::cout << sphere.isOnSurface(none) << std::endl;

    Point start(0,0,10);
    Vector3D v(3, 0, -4);
    Ray r(start, v);
    Point p(3, 0, 6);

    auto n = sphere.getNormalVector(p, r);
    std::cout << n->x << " " << n->y << " " << n->z << std::endl;


    Vector3D V(p, start);
    Point total_I(0.0, 0.0, 0.0);
    auto N = sphere.getNormalVector(p, r);
    
    Point Od(sphere.color.dr, sphere.color.dg, sphere.color.db);
    Od.print();
    Point Os(sphere.color.sr, sphere.color.sg, sphere.color.sb);
    Os.print();
    
    Point I_ambient = Od * sphere.color.ka;
    I_ambient.print();

    Vector3D L(Point(3,0,6), Point(5,2,7));
    L = L.unit();
    L.print();
    Vector3D H = ((L + V) / (L + V).mag());
    H.print();

    //N*L
    double N_dot_L = N->dot(L);
    //N_dot_L should not be negative
    if (N_dot_L < 0)
        N_dot_L = 0;
    //N*H
    double N_dot_H = N->dot(H);
    //N_dot_L should not be negative
    if (N_dot_H < 0)
        N_dot_H = 0;
        
    double N_dot_H_sq_n = pow(N_dot_H, sphere.color.n);
    Point I_diffuse = Od * sphere.color.kd * N_dot_L;
    Point I_specular = Os * sphere.color.ks * N_dot_H_sq_n;
    Point IL(1, 1, 1);
    //total_I += I
    //apply shadows
    total_I = I_ambient  + IL * (I_diffuse + I_specular);
    std::cout << total_I.x * 255 << " " << total_I.y * 255 << " " << total_I.z * 255 << " " << std::endl;
    // Vector v1(3, -3, 1);
    // Vector v2(4, 9, 2);
    // Vector v3 = v1 * v2;
    // std::cout << v3.x << " " << v3.y << " " << v3.z << std::endl;
    return 0;
}

