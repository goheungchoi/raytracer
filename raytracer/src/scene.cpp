#include "scene.h"
#include <cmath>
Scene::Scene () {}

Scene::Scene(Point view_o, Vector3D view_dir, Vector3D up_dir, 
          double vfov, double image_height, double image_width,
          double d)
{
    this->view_o = view_o;
    this->view_dir = view_dir;
    this->up_dir = up_dir;

    this->vfov = vfov;
    this->image_height = image_height;
    this->image_width = image_width;
    this->d = d;

    u = (view_dir * up_dir).getUnit();
    v = (u * view_dir).getUnit();
    n = view_dir.getUnit();

    aspect_ratio = image_width / image_height;
    win_height = 2 * d * tan((vfov / 2.0) * PI / 180.0);
    win_width = win_height * aspect_ratio;

    double half_h = win_height / 2;
    double half_w = win_width / 2;
//     ul = view_origin + d×n – (w/2)×u + (h/2)×v
//     ur = view_origin + d×n + (w/2)×u + (h/2)×v
//     ll = view_origin + d×n – (w/2)×u – (h/2)×v
//     lr = view_origin + d×n + (w/2)×u – (h/2)×v
    ul = view_o + (n.scale(d) - u.scale(half_w) + v.scale(half_h)).toPoint();
    ur = view_o + (n.scale(d) + u.scale(half_w) + v.scale(half_h)).toPoint();
    ll = view_o + (n.scale(d) - u.scale(half_w) - v.scale(half_h)).toPoint();
    lr = view_o + (n.scale(d) + u.scale(half_w) - v.scale(half_h)).toPoint();

    delta_c_h = (ur - ul) / (2 * image_width);
    delta_c_v = (ll - ul) / (2 * image_height);

    delta_h = (ur - ul) / (image_width);
    delta_v = (ll - ul) / (image_height);
}

Scene::~Scene()
{
    for(int i = 0; i < obj.size(); i++)
        delete obj.at(i);
    for(int i = 0; i < lights.size(); i++)
        delete lights.at(i);
}

Point Scene::getPixelAt(int i, int j)
{
    return ul + delta_h * i + delta_v * j + delta_c_h + delta_c_v;
}

Point* Scene::getRayIntersection(const Line& ray, int &index)
{
    Point *closest_p = obj.at(0)->getIntersection(ray);
    index = 0;
    int temp = index;
    for (int i = 1; i < obj.size(); i++)
    {
        Point *p = obj.at(i)->getIntersection(ray);
        if (p == nullptr) {} //if there is no intersection
        //closest_p is not initialized or it is ray.o
        else if (closest_p == nullptr)  
        {
            closest_p = p;
            index = i;
        }   // new point is closer than the closest_p
        else if (p->getDistance(ray.o) < closest_p->getDistance(ray.o))
        {
            free(closest_p);
            closest_p = p;
            index = i;
        }
        else {free(p);}
    }
    return closest_p;
}

Point Scene::getDepthCueing(Point& I, const Point& p)
{
    if (!depth_cueing_on)
        return I;
    double distance = view_o.getDistance(p);
    double a_dc = 0.0;
    if (distance <= depth_cue.dist_near)
        a_dc = depth_cue.a_max;
    else if ( distance <= depth_cue.dist_far )
        a_dc = depth_cue.a_min 
                + (depth_cue.a_max - depth_cue.a_min) 
                * ((depth_cue.dist_far - distance) 
                    / (depth_cue.dist_far - depth_cue.dist_near));
    else
        a_dc = depth_cue.a_min;

    Point result = I * a_dc + depth_cue.I_dc * (1 - a_dc);
    return result;
}