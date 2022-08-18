#include "obj/object.h"

text_coor text_coor::operator+(const text_coor& coor) {
  double new_u = this->u + coor.u;
  double new_v = this->v + coor.v;
  return text_coor{new_u, new_v};
}
text_coor text_coor::operator-(const text_coor& coor) {
  double new_u = this->u - coor.u;
  double new_v = this->v - coor.v;
  return text_coor{new_u, new_v};
}
text_coor text_coor::operator*(const text_coor& coor) {
  double new_u = this->u * coor.u;
  double new_v = this->v * coor.v;
  return text_coor{new_u, new_v};
}
text_coor text_coor::operator/(const text_coor& coor) {
  double new_u = this->u / coor.u;
  double new_v = this->v / coor.v;
  return text_coor{new_u, new_v};
}
text_coor text_coor::operator+(const double& num) {
  double new_u = this->u + num;
  double new_v = this->v + num;
  return text_coor{new_u, new_v};
}
text_coor text_coor::operator-(const double& num) {
  double new_u = this->u - num;
  double new_v = this->v - num;
  return text_coor{new_u, new_v};
}
text_coor text_coor::operator*(const double& num) {
  double new_u = this->u * num;
  double new_v = this->v * num;
  return text_coor{new_u, new_v};
}
text_coor text_coor::operator/(const double& num) {
  double new_u = this->u / num;
  double new_v = this->v / num;
  return text_coor{new_u, new_v};
}

Point map::getImage(int i, int j) {
  if (0 <= i - w && i - w < 1) {
    i -= 1;
  }
  if (0 <= j - h && j - h < 1) {
    j -= 1;
  }
  return map.at(j * w + i);
}

Point map::getBilinearInterpolation(const text_coor& c) {
  double x = c.u * (w - 1);
  double y = c.v * (h - 1);
  int i = (int)x;
  int j = (int)y;
  double a = x - i;
  double b = y - j;
  return getImage(i, j) * (1 - a) * (1 - b) +
         getImage(i + 1, j) * (a) * (1 - b) +
         getImage(i, j + 1) * (1 - a) * (b) +
         getImage(i + 1, j + 1) * (a) * (b);
}

Object::Object() {
  this->name = "";
  this->clr = color{0, 0, 0};
}

Object::Object(std::string name, color clr, double opacity,
               double index_of_refraction) {
  this->name = name;
  this->clr = clr;
  this->opacity = opacity;
  this->index_of_refraction = index_of_refraction;
}

Object::~Object() {}