#ifndef _SCENE_H_
#define _SCENE_H_

#include "util/util.h"

#include "obj/vector3D.h"
#include "obj/point.h"
#include "obj/ray.h"
#include "obj/light.h"
#include "obj/object.h"
#include "obj/plane.h"
#include "obj/sphere.h"
#include "obj/cylinder.h"
#include "obj/triangle.h"
#include "obj/triangle_mesh.h"


typedef struct dc {
  I I_dc{0.0, 0.0, 0.0};
  double dist_near = 10.0;
  double dist_far = 50.0;
  double a_max = 1.0;
  double a_min = 1.0;
} dc;

class Scene {
public:
  // Member variables
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
  std::vector<std::unique_ptr<Object>> objects;
  I bkgColor;
  std::vector<std::unique_ptr<Light>> lights;
  dc depth_cue;
  bool depth_cueing_on = false;

  // Default constructor
  Scene() = default;
  // Parameterized constructors
  Scene(Point cam_pos, Vector3D view_dir, Vector3D up_dir, 
    double vfov, double image_width, double image_height, double d);
  // Member functions
  std::optional<Point> getRayIntersection(const Ray& ray, int& index);
  I getDepthCueing(const I& I, const Point& p);
  void addObjectUniquePtr(std::unique_ptr<Object>&&);
  void addLightUniquePtr(std::unique_ptr<Light>&&);
  // Inline member functions
  inline constexpr Point getPixelAt(int i, int j) const {
    return ul + delta_h*i + delta_v*j + delta_c_h + delta_c_v;
  };
};

#endif  //_SCENE_H_
