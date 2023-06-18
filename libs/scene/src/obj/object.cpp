#include "obj/object.h"

long unsigned int Object::total = 0;

Object::Object(obj::color_t color, double opacity, double iRefraction)
: id(total++), color(color), opacity(opacity), iRefraction(iRefraction) 
{}

util::arr3 obj::map_t::mapAt(int x, int y) const {
  // TODO: Test this.
  // if (0 <= x-w && x-w < 1)
  //   x -= 1;
  // if (0 <= y-h && y-h < 1)
  //   y -= 1;

  // Prevent overflow inputs.
  if (x >= w) x = w-1;
  else if (x < 0) x = 0;
  if (y >= h) y = h-1;
  else if (y < 0) y = 0;
  return map.at(y*w + x);
}

util::arr3 obj::map_t::getBilinearInterpolation(const obj::text_coor_t& c) const {
  double x = c.u * (w - 1);
  double y = c.v * (h - 1);
  int i = (int)x;
  int j = (int)y;
  double a = x - i;
  double b = y - j;
  return mapAt(i, j) * (1-a) * (1-b) +
         mapAt(i+1, j) * (a) * (1-b) +
         mapAt(i, j+1) * (1-a) * (b) +
         mapAt(i+1, j+1) * (a) * (b);
}

