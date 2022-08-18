#ifndef _RAY_CASTING_
#define _RAY_CASTING_

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <stdlib.h>
#include <math.h>

#include "scene.h"
#include "SceneFactory.h"


#define _GLIBCXX_USE_CXX11_ABI 1

I ray_cast(int i, int j);
I shade_ray(Line& ray, Object* o,const Point* p, double& traveled_distance, std::vector<int> trace);
double cast_shadow(Light *l, Vector3D& L, const Point* p, int flag);

Scene* scene;
const int RAND_RANGE = 11;
const int ANTI_ALIASING_TIMES = 4;
const int SHADOW_RAY_CASTING_TIMES = 1;
const double MAX_DISTANCE = 50;
//const double SHODOW_OFFSET_THETA = 10;

#endif