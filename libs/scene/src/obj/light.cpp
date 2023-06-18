#include "obj/light.h"

double Light::attenuationFunc(const Point &p) const { return 1.0; }

double PointLight::attenuationFunc(const Point &p) const {
  if (util::almost_equal(c1, 0.0, 100) 
    && util::almost_equal(c2, 0.0, 100) 
    && util::almost_equal(c3, 0.0, 100))
    return 1.0;
  double d = o.getDistance(p);
  return 1.0 / (c1 + c2*d + c3*d*d);
}

Highlight::Highlight(I intensity, Point p, double radius, Vector3D dir, double cutoff)
: Light(intensity), o(o), radius(radius), dir(dir.unit()) {
  type = LightType::HIGHLIGHT;
  if (cutoff <= 90.0 && cutoff > 0.0)
    this->cutoff = cutoff;
}

double Highlight::attenuationFunc(const Point &p) const
{
  double amin = 0.5;
  double amax = 1.0;
  Vector3D v_o(o, p);
  double att = 1.0 - (acos(dir.dot(v_o.unit()))*180.0/util::PI)/cutoff;
  return amin + (amax - amin)*att;
}
