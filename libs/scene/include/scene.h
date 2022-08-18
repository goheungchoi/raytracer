#ifndef _SCENE_
#define _SCENE_

#include <iostream>
#include <vector>

#include "light.h"
#include "line.h"
#include "object.h"
#include "point.h"
#include "vector3D.h"

typedef struct dc {
  Point I_dc = Point(0.0, 0.0, 0.0);
  double dist_near = 10.0;
  double dist_far = 50.0;
  double a_max = 1.0;
  double a_min = 1.0;
} dc;

class Scene {
 public:
  Point view_o;
  Vector3D view_dir, v, u, n;
  Vector3D up_dir;
  double vfov = 0;
  double hfov = 0;
  double aspect_ratio = 0;
  double d = 0;
  double image_height = 0;
  double image_width = 0;
  double win_height = 0;
  double win_width = 0;
  Point ul, ur, ll, lr;
  Point delta_c_h, delta_c_v, delta_h, delta_v;
  std::vector<Object*> obj;
  I bkgColor;
  std::vector<Light*> lights;
  dc depth_cue;
  bool depth_cueing_on = false;

  Scene();
  Scene(Point cam_pos, Vector3D view_dir, Vector3D up_dir, double vfov,
        double image_width, double image_height, double d);
  ~Scene();
  Point getPixelAt(int i, int j);
  Point* getRayIntersection(const Line& ray, int& index);
  Point getDepthCueing(Point& I, const Point& p);
};

#endif