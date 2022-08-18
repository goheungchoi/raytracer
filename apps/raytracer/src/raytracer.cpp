/**
 * @file simpleppm.cpp
 * @author Goheung Choi
 * @brief this program creates a picture in a ppm file
 * @version 1.1.0
 * @date 01/25/2022
 */
#include "raytracer.h"

int main(int argc, char **argv) {
  // get file name
  std::string filename;
  std::stringstream ss;
  ss << argv[1];
  ss >> filename;
  // create a scene factory with the file name
  SceneFactory scene_fac = SceneFactory(filename);
  // create a scene
  scene = scene_fac.CreateScene();

  // dynamic allocation: color array with size of w_pixels * h_pixels
  I **clr = new I *[(int)scene->image_height];
  for (int i = 0; i < (int)scene->image_height; i++)
    clr[i] = new I[(int)scene->image_width];

  // store the pixels
  for (int y = 0; y < (int)scene->image_height; y++) {
    for (int x = 0; x < (int)scene->image_width; x++) {
      I p = ray_cast(x, y);
      p.scale();
      clr[y][x] = p;
    }
  }

  // print the pixels
  for (int y = 0; y < (int)scene->image_height; y++) {
    for (int x = 0; x < (int)scene->image_width; x++) {
      I p = clr[y][x];
      scene_fac.CreateImagePixel(
          std::to_string((int)(p.Ir * 255.999) > 255 ?
                                                 255 : (int)(p.Ir * 255.999)) +
          " " +
          std::to_string((int)(p.Ig * 255.999) > 255 ? 
                                                 255 : (int)(p.Ig * 255.999)) +
          " " +
          std::to_string((int)(p.Ib * 255.999) > 255 ? 
                                                 255 : (int)(p.Ib * 255.999)) +
          " ");
    }
  }

  // free memory
  delete[] clr;
  delete scene;
  return 0;
}

I ray_cast(int i, int j) {
  // I pixel_color = scene->bkgColor;
  // Line ray(scene->view_o, scene->getPixelAt(i, j));
  // int index = 0;
  // Point *closest_p = scene->getRayIntersection(ray, index);
  // if (closest_p != nullptr)
  //     pixel_color = shade_ray(scene->obj.at(index), closest_p);

  // anti-aliasing
  I pixel_color = scene->bkgColor;

  Point calnum[ANTI_ALIASING_TIMES];
  Point delta_h_fourth = scene->delta_c_h / 2.0;
  Point delta_v_fourth = scene->delta_c_v / 2.0;
  // set up an array which contains offsets
  for (int t = 0; t < ANTI_ALIASING_TIMES; t++) {
    // //prevent from dividing number by 0
    // double ran1 = (rand() % RAND_RANGE - RAND_RANGE / 2);
    // ran1 = (ran1 == 0)? 1.0 : 1.0 / ran1;
    // double ran2 = (rand() % RAND_RANGE - RAND_RANGE / 2);
    // ran2 = (ran2 == 0)? 1.0 : 1.0 / ran2;
    // //get randome offsets
    // calnum[t] = delta_h_fourth * ran1 + delta_v_fourth * ran2;
    calnum[t] = delta_h_fourth * pow(-1, t / 2) + delta_v_fourth * pow(-1, t);
  }
  I total_color;
  total_color.Ir = 0;
  total_color.Ig = 0;
  total_color.Ib = 0;

  for (int t = 0; t < ANTI_ALIASING_TIMES; t++) {
    // initialize object tracing
    std::vector<int> trace;
    trace.clear();
    trace.push_back(-1);
    double traveled_distance = 0;
    // create ray
    Line ray(scene->view_o, scene->getPixelAt(i, j) + calnum[t]);
    // check whether there is an intersection
    // and get the closest intersection point
    int index = 0;
    Point *closest_p = scene->getRayIntersection(ray, index);
    // get the color of the object of the closest intersection
    // if not, use the default background color
    if (closest_p != nullptr) {
      // record the current object
      trace.push_back(index);
      total_color =
          total_color + shade_ray(ray, scene->obj.at(index), closest_p,
                                  traveled_distance, trace);
    }
  }
  pixel_color = total_color / ANTI_ALIASING_TIMES;
  return pixel_color;
}
/**
 * generate surface effects and shadows
 * @param ray - injected ray
 * @param o - object that the ray hits
 * @param p - point on the object where the ray hits
 * @param traveled_distance - ray traced distance
 */
I shade_ray(Line &ray, Object *o, const Point *p, double &traveled_distance,
            std::vector<int> trace) {
  /** calculate distance **/
  traveled_distance += ray.o.getDistance(*p);
  // traveled_distance++;
  /************************/
  Vector3D V(*p, scene->view_o);
  Point total_I(0.0, 0.0, 0.0);
  // set normal vector
  Vector3D *normal = o->getNormalVector(*p, ray);
  if (normal == nullptr) {
    printf("invalid normal vector");
    exit(0);
  }
  Vector3D N(*normal);
  delete normal;
  // set Od
  Point *OdLambda = o->getOdLambda(*p);
  if (OdLambda == nullptr) {
    printf("invalid Od");
    exit(0);
  }
  Point Od(*OdLambda);
  delete OdLambda;
  // set Os
  Point Os(o->clr.sr, o->clr.sg, o->clr.sb);

  Point I_ambient = Od * o->clr.ka;

  Light *l;
  for (int i = 0; i < scene->lights.size(); i++) {
    // used Point class in order to calculate 3-tuple at once
    l = scene->lights.at(i);
    Vector3D L;
    Vector3D H;
    double att;  // attenuation
    double S = 1.0;
    ;  // shadow flag
    // check the type of light
    DirectionalLight *dl = dynamic_cast<DirectionalLight *>(l);
    PointLight *pl = dynamic_cast<PointLight *>(l);
    HighLight *hl = dynamic_cast<HighLight *>(l);
    if (dl != nullptr) {
      L = -dl->dir.getUnit();
      att = 1.0;
      S = cast_shadow(l, L, p, 0);
    } else if (pl != nullptr) {
      L = Vector3D(*p, pl->p).getUnit();
      att = pl->attenuationFunc(*p);
      S = cast_shadow(l, L, p, 1);
    } else if (hl != nullptr) {
      if (hl->isInHighLight(*p)) {
        L = Vector3D(*p, hl->p).getUnit();
        att = hl->attenuationFunc(*p);
        S = cast_shadow(l, L, p, 2);
        if (Object::equals(S, 0.0)) {
          att = 0.0;
        }
      } else {
        L = Vector3D();
        att = 0.0;
      }
    }

    H = ((L + V) / (L + V).getMag());
    // N*L
    double N_dot_L = N.dotProd(L);
    // N_dot_L should not be negative
    if (N_dot_L < 0) N_dot_L = 0;
    // N*H
    double N_dot_H = N.dotProd(H);
    // N_dot_L should not be negative
    if (N_dot_H < 0) N_dot_H = 0;

    double N_dot_H_sq_n = pow(N_dot_H, o->clr.n);
    Point I_diffuse = Od * o->clr.kd * N_dot_L;
    Point I_specular = Os * o->clr.ks * N_dot_H_sq_n;
    Point IL(l->intensity.Ir, l->intensity.Ig, l->intensity.Ib);
    // total_I += I
    // apply shadows
    total_I = total_I + (IL * (I_diffuse + I_specular) * att * S);
  }

  Point I_lambda = I_ambient + total_I;

  /******** set R_lambda ********/
  I R_lambda(0.0, 0.0, 0.0);
  /******** get Fr ********/
  int curr_index = trace.size() - 1;
  int curr_obj_index = trace.at(curr_index);
  double index_of_refraction_t = 1.0;
  if (curr_obj_index > -1)
    index_of_refraction_t = scene->obj.at(curr_obj_index)->index_of_refraction;
  int prev_index = trace.size() - 2;
  int prev_obj_index = trace.at(prev_index);
  double index_of_refraction_i = 1.0;
  if (prev_obj_index > -1)
    index_of_refraction_i = scene->obj.at(prev_obj_index)->index_of_refraction;

  double F0_00 = index_of_refraction_t - index_of_refraction_i;
  double F0_01 = index_of_refraction_t + index_of_refraction_i;
  double F0_1 = F0_00 / F0_01;
  double F0 = F0_1 * F0_1;
  Vector3D incoming_vector(-ray.v);
  double cos_incoming_theta = N.dotProd(incoming_vector);
  double Fr_0 = pow(1.0 - cos_incoming_theta, 5);
  double Fr = F0 + (1.0 - F0) * Fr_0;
  // Fr = pow(Fr, traveled_distance);
  /* specular reflection */
  if (traveled_distance < MAX_DISTANCE && !Object::equals(Fr, 0.0)) {
    double new_dist = traveled_distance;
    Vector3D R((N * (N.dotProd(incoming_vector))) * 2 - incoming_vector);
    Line new_ray(*p, R);
    // check whether there is an intersection
    // and get the closest intersection point
    int index = 0;
    Point *closest_p = scene->getRayIntersection(new_ray, index);
    // get the color of the reflection
    // if not, no reflection.
    if (closest_p != nullptr) {
      trace.push_back(-1);
      trace.push_back(index);
      /*** recursive loop ***/
      R_lambda =
          shade_ray(new_ray, scene->obj.at(index), closest_p, new_dist, trace);
      trace.pop_back();
      trace.pop_back();
    }
  }
  // Fr * R_lambda
  I I_R = R_lambda * o->clr.ks;
  /***********************/
  /* refraction */
  double angle_of_i = incoming_vector.getAngle(N) / 180.0 * PI;
  double temp_r = index_of_refraction_t / index_of_refraction_i;
  double critical_angle = asin(temp_r);
  I T_lambda(0.0, 0.0, 0.0);

  bool internal_reflection =
      sin(angle_of_i) > temp_r && angle_of_i >= critical_angle;
  if (!internal_reflection && !Object::equals(o->opacity, 1.0)) {
    if (traveled_distance < MAX_DISTANCE) {
      double new_dist = traveled_distance;

      /** get T vector**/
      double ratio = index_of_refraction_i / index_of_refraction_t;
      double T_000 = ratio * ratio;
      double T_001 = 1.0 - cos_incoming_theta * cos_incoming_theta;
      double T_00 = 1 - T_000 * T_001;
      double T_01 = sqrt(T_00);
      Vector3D T_0 = (-N) * T_01;
      Vector3D T_100 = N * cos_incoming_theta;
      Vector3D T_10 = T_100 - incoming_vector;
      Vector3D T_1 = T_10 * ratio;
      Vector3D T = T_0 + T_1;

      Line new_ray(*p, T);
      // check whether there is an intersection
      // and get the closest intersection point
      int index = 0;
      Point *closest_p = scene->getRayIntersection(new_ray, index);
      // get the color of the reflection
      // if not, no reflection.
      if (closest_p != nullptr) {
        if (trace.back() == index) {
          trace.push_back(index);
          trace.push_back(-1);
        } else {
          trace.push_back(-1);
          trace.push_back(index);
        }
        /*** recursive loop ***/
        T_lambda = shade_ray(new_ray, scene->obj.at(index), closest_p, new_dist,
                             trace);
        trace.pop_back();
        trace.pop_back();
      }
    }
    T_lambda = T_lambda * (1.0 - Fr) * (1 - o->opacity);
  }
  I I_T = T_lambda;
  // depth cueing
  Point I_lambda_prime = scene->getDepthCueing(I_lambda, *p);

  I intensity(I_lambda_prime.x, I_lambda_prime.y, I_lambda_prime.z);
  return intensity + I_R + T_lambda;
}

// double cast_shadow(const Point* p)
// {
//     double S = 1.0; //no shadow
//     double sum_of_flags = 0.0;
//     int num_of_shadows = 0;
//     int num_of_lights = scene->lights.size();
//     Point total_ILs(0.0, 0.0, 0.0);
//     for (Light* l : scene->lights)
//     {
//         //check the type of light
//         DirectionalLight *dl = dynamic_cast<DirectionalLight*>(l);
//         PointLight *pl = dynamic_cast<PointLight*>(l);
//         HighLight *hl = dynamic_cast<HighLight*>(l);
//         if (dl != nullptr)
//         {
//             Vector L = -dl->dir.getUnit();
//             Line ray(*p, L);
//             int index = 0;
//             Point *closest_p = scene->getRayIntersection(ray, index);
//             if (closest_p != nullptr && closest_p != p)
//             {
//                 sum_of_flags -= 1.0; //set S to be 0.0
//                 num_of_shadows++;
//             }
//         }
//         else if (pl != nullptr)
//         {
//             double S_sum = 0.0;
//             Vector l_v(pl->p, *p);
//             for (int t = 0; t < SHADOW_RAY_CASTING_TIMES; t++)
//             {
//                 //prevent from dividing number by 0
//                 int ran1 = (rand() % RAND_RANGE - RAND_RANGE / 2);
//                 ran1 = ran1 == 0? 1.0 : 1.0 / ran1;
//                 int ran2 = (rand() % RAND_RANGE - RAND_RANGE / 2);
//                 ran2 = ran2 == 0? 1.0 : 1.0 / ran2;
//                 int ran3 = (rand() % RAND_RANGE - RAND_RANGE / 2);
//                 ran3 = ran3 == 0? 1.0 : 1.0 / ran3;
//                 //get random offset angles
//                 double ran_x_theta = SHODOW_OFFSET_THETA * ran1;
//                 double ran_y_theta = SHODOW_OFFSET_THETA * ran2;
//                 double ran_z_theta = SHODOW_OFFSET_THETA * ran3;
//                 Vector random_l = l_v.rotate_x_axis(ran_x_theta)
//                                     + l_v.rotate_y_axis(ran_y_theta)
//                                     + l_v.rotate_z_axis(ran_z_theta);
//                 Line ray(pl->p, random_l);
//                 double dist = pl->p.getDistance(*p);
//                 int index = 0;
//                 Point *closest_p = scene->getRayIntersection(ray, index);
//                 double dist2 = pl->p.getDistance(*closest_p);
//                 if( 0 < dist2 && dist2 < dist && (*closest_p != *p))
//                     S_sum -= 1.0;
//             }
//             if(!Object::equals(S_sum, 0.0))
//             {
//                 sum_of_flags -= S_sum / SHADOW_RAY_CASTING_TIMES;
//                 num_of_shadows++;
//             }
//         }
//         else if (hl != nullptr)
//         {
//             if(hl->isInHighLight(*p))
//             {
//                 double S_sum = 0.0;
//                 Vector l_v(pl->p, *p);
//                 for (int t = 0; t < SHADOW_RAY_CASTING_TIMES; t++)
//                 {
//                     double ran_x_theta = SHODOW_OFFSET_THETA
//                         * (1.0 / (rand() % RAND_RANGE - RAND_RANGE / 2));
//                     double ran_y_theta = SHODOW_OFFSET_THETA
//                         * (1.0 / (rand() % RAND_RANGE - RAND_RANGE / 2));
//                     double ran_z_theta = SHODOW_OFFSET_THETA
//                         * (1.0 / (rand() % RAND_RANGE - RAND_RANGE / 2));
//                     Vector random_l = l_v.rotate_x_axis(ran_x_theta)
//                                         + l_v.rotate_y_axis(ran_y_theta)
//                                         + l_v.rotate_z_axis(ran_z_theta);
//                     Line ray(pl->p, random_l);
//                     double dist = pl->p.getDistance(*p);
//                     int index = 0;
//                     Point *closest_p = scene->getRayIntersection(ray, index);
//                     double dist2 = pl->p.getDistance(*closest_p);
//                     if( 0 < dist2 && dist2 < dist && closest_p != p)
//                         S_sum -= 1.0;
//                 }
//                 if(!Object::equals(S_sum, 0.0))
//                 {
//                     sum_of_flags -= S_sum / SHADOW_RAY_CASTING_TIMES;
//                     num_of_shadows++;
//                 }
//             }
//             else
//             {
//                 S = S;
//             }
//         }
//     }
//     S += sum_of_flags / num_of_shadows;
//     return S < 0.0? 0.0 : S;
// }
/**
 * @brief create shadow effect
 *
 * @param l pointer to the light object
 * @param L vector L
 * @param p intersection between the injected ray and the object
 * @param flag  0 - directional light
 *              1 - point light
 *              2 - point highlight
 *              3 - spherical light
 * @return double shadow flag from 0 (full shadow) to 1 (no shadow)
 */
double cast_shadow(Light *l, Vector3D &L, const Point *p, int flag) {
  double S = 1.0;  // no shadow

  if (flag == 0) {
    Line ray(*p, L);
    int index = 0;
    Point *closest_p = scene->getRayIntersection(ray, index);

    if (closest_p == nullptr)
      return S;  // no shadow S = 1
    else {
      S = S * (1 - scene->obj.at(index)->opacity);
    }
  } else if (flag == 1) {
    PointLight *pl = dynamic_cast<PointLight *>(l);
    double S_sum = 0.0;
    for (int t = 0; t < SHADOW_RAY_CASTING_TIMES; t++) {
      /*get soft shadow*/
      int index = 0;
      double shadow_offset_theta = pl->getTheta(p);
      // prevent from dividing number by 0
      double ran1 = (rand() % RAND_RANGE - RAND_RANGE / 2);
      ran1 = ran1 == 0 ? 1.0 : 1.0 / ran1;
      double ran2 = (rand() % RAND_RANGE - RAND_RANGE / 2);
      ran2 = ran2 == 0 ? 1.0 : 1.0 / ran2;
      double ran3 = (rand() % RAND_RANGE - RAND_RANGE / 2);
      ran3 = (ran3 == 0) ? 1.0 : 1.0 / ran3;
      // get random offset angles
      double ran_x_theta = shadow_offset_theta * ran1;
      double ran_y_theta = shadow_offset_theta * ran2;
      double ran_z_theta = shadow_offset_theta * ran3;
      Vector3D random_L = L.rotate(ran_x_theta, ran_y_theta, ran_z_theta);
      // generate random ray in a boundary
      Line random_ray(*p, random_L);

      Point *closest_p = scene->getRayIntersection(random_ray, index);
      if (closest_p != nullptr) {
        double dist = (pl->p).getDistance(*p);
        double dist2 = (pl->p).getDistance(*closest_p);
        if (0 < dist2 && dist2 < dist && (*closest_p != *p))
          S_sum += (1 - scene->obj.at(index)->opacity);
        else
          S_sum += 1.0;
      } else
        S_sum += 1.0;
    }
    // calculate average S
    if (!Object::equals(S_sum, 0.0))
      S = S_sum / SHADOW_RAY_CASTING_TIMES;
    else
      S = 0.0;
  } else if (flag == 2) {
    HighLight *hl = dynamic_cast<HighLight *>(l);
    double S_sum = 0.0;
    for (int t = 0; t < SHADOW_RAY_CASTING_TIMES; t++) {
      /*get soft shadow*/
      int index = 0;
      double shadow_offset_theta = hl->getTheta(p);
      // prevent from dividing number by 0
      double ran1 = (rand() % RAND_RANGE - RAND_RANGE / 2);
      ran1 = ran1 == 0 ? 1.0 : 1.0 / ran1;
      double ran2 = (rand() % RAND_RANGE - RAND_RANGE / 2);
      ran2 = ran2 == 0 ? 1.0 : 1.0 / ran2;
      double ran3 = (rand() % RAND_RANGE - RAND_RANGE / 2);
      ran3 = (ran3 == 0) ? 1.0 : 1.0 / ran3;
      // get random offset angles
      double ran_x_theta = shadow_offset_theta * ran1;
      double ran_y_theta = shadow_offset_theta * ran2;
      double ran_z_theta = shadow_offset_theta * ran3;
      Vector3D random_L = L.rotate(ran_x_theta, ran_y_theta, ran_z_theta);
      // generate random ray in a boundary
      Line random_ray(*p, random_L);

      Point *closest_p = scene->getRayIntersection(random_ray, index);
      if (closest_p != nullptr) {
        double dist = (hl->p).getDistance(*p);
        double dist2 = (hl->p).getDistance(*closest_p);
        if (0 < dist2 && dist2 < dist && (*closest_p != *p))
          S_sum += (1 - scene->obj.at(index)->opacity);
        else
          S_sum += 1.0;
      } else
        S_sum += 1.0;
    }
    // calculate average S
    if (!Object::equals(S_sum, 0.0))
      S = S_sum / SHADOW_RAY_CASTING_TIMES;
    else
      S = 0.0;
    // Line ray(*p, L);
    // int index = 0;
    // Point *closest_p = scene->getRayIntersection(ray, index);
    // if (closest_p != nullptr) {
    //     double dist = (hl->p).getDistance(*p);
    //     double dist2 = (hl->p).getDistance(*closest_p);
    //     if( 0 < dist2 && dist2 < dist && (*closest_p != *p))
    //         S = 0;
    // }
  }

  return S;
}
