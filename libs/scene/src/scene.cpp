#include "scene.h"

Scene::Scene(Point view_o, Vector3D view_dir, Vector3D up_dir, 
  double vfov, double image_height, double image_width, double d)
: view_o(view_o), view_dir(view_dir), up_dir(up_dir),
vfov(vfov), image_height(image_height), image_width(image_width), d(d) {

  u = (view_dir*up_dir).unit();
  v = (u*view_dir).unit();
  n = view_dir.unit();

  aspect_ratio = image_width/image_height;
  win_height = 2.0*d*tan((vfov / 2.0) * util::PI / 180.0);
  win_width = win_height * aspect_ratio;

  double half_h = win_height / 2.0;
  double half_w = win_width / 2.0;
//  ul = view_origin + d×n – (w/2)×u + (h/2)×v
//  ur = view_origin + d×n + (w/2)×u + (h/2)×v
//  ll = view_origin + d×n – (w/2)×u – (h/2)×v
//  lr = view_origin + d×n + (w/2)×u – (h/2)×v
  ul = view_o + (n.scale(d) - u.scale(half_w) + v.scale(half_h)).toPoint();
  ur = view_o + (n.scale(d) + u.scale(half_w) + v.scale(half_h)).toPoint();
  ll = view_o + (n.scale(d) - u.scale(half_w) - v.scale(half_h)).toPoint();
  lr = view_o + (n.scale(d) + u.scale(half_w) - v.scale(half_h)).toPoint();

  delta_c_h = (ur-ul) / (2.0*image_width);
  delta_c_v = (ll-ul) / (2.0*image_height);

  delta_h = (ur-ul) / (image_width);
  delta_v = (ll-ul) / (image_height);
}

std::optional<Point> Scene::getRayIntersection(const Ray& ray, int& index) {
  int i = 0;
  std::optional<Point> closestPoint;
  double closestDistance = INFINITY;
  for (const auto& obj : objects) {
    if (auto p = (*obj).getIntersection(ray)){
      double distance = (*p).getDistance(ray.o);
      if (!closestPoint || distance < closestDistance) {
        closestPoint = p;
        closestDistance = distance;
        index = i;
      }
    }
    i++;
  }

  return closestPoint;
}

I Scene::getDepthCueing(const I& i, const Point& p)
{
    if (!depth_cueing_on)
        return i;
    double distance = view_o.getDistance(p);
    double a_dc = 0.0;
    if (distance <= depth_cue.dist_near)
        a_dc = depth_cue.a_max;
    else if (distance <= depth_cue.dist_far)
        a_dc = depth_cue.a_min 
              + (depth_cue.a_max - depth_cue.a_min) 
              * ((depth_cue.dist_far - distance) 
                  / (depth_cue.dist_far - depth_cue.dist_near));
    else
        a_dc = depth_cue.a_min;

    return i*a_dc + depth_cue.I_dc*(1.0-a_dc);
}

void Scene::addObjectUniquePtr(std::unique_ptr<Object>&& pObj) {
  objects.push_back(std::move(pObj));
}

void Scene::addLightUniquePtr(std::unique_ptr<Light>&& pLight) {
  lights.push_back(std::move(pLight));
}
