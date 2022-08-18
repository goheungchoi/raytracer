#include "obj/line.h"

Line::Line(Point tl, Point hd)
{
    this->o = tl;
    this->v = Vector3D(tl, hd).getUnit();
}

Line::Line(Point o, Vector3D v)
{
    this->o = o;
    this->v = v.getUnit();
}
