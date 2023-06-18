/**
 * @file simpleppm.cpp
 * @author Goheung Choi
 * @brief this program creates a picture in a ppm file
 * @version 1.1.0
 * @date 01/25/2022
 */
#include "raytracer.h"

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("usage: build/bin/raytracer <file_name>\n");
    exit(-1);
  }

  // Get file name
  std::string filename;
  std::stringstream ss;
  ss << argv[1];
  ss >> filename;
  // Create a scene factory with the file name
  SceneFactory scene_fac = SceneFactory(filename);
  // Create a scene
  scene = scene_fac.CreateScene();

  /// Dynamic allocation: 
  ///  color array with size of w_pixels * h_pixels
  I **pixels = new I *[(int)scene->image_height];
  for (int i = 0; i < (int)scene->image_height; i++)
    pixels[i] = new I[(int)scene->image_width];

  /// Main loop:
  ///  cast rays and store the results
  for (int y = 0; y < (int)scene->image_height; y++) {
    for (int x = 0; x < (int)scene->image_width; x++) {
      I p = ray_cast(x, y);
      p.scale();
      pixels[y][x] = p;
    }
  }

  /// Store the pixels
  for (int y = 0; y < (int)scene->image_height; y++) {
    for (int x = 0; x < (int)scene->image_width; x++) {
      I p = pixels[y][x];
      scene_fac.CreateImagePixel(
          std::to_string((int)(p.r * 255.999) > 255 ?
            255 : (int)(p.r * 255.999)) +
          " " +
          std::to_string((int)(p.g * 255.999) > 255 ? 
            255 : (int)(p.g * 255.999)) +
          " " +
          std::to_string((int)(p.b * 255.999) > 255 ? 
            255 : (int)(p.b * 255.999)) +
          " ");
    }
  }

  // free memory
  for (int i = 0; i < (int)scene->image_height; i++)
    delete[] pixels[i];
  delete[] pixels;
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
  // Set up an array which contains anti-aliased points
  for (int t = 0; t < ANTI_ALIASING_TIMES; t++) {
    // //prevent from dividing number by 0
    // double ran1 = (rand() % RAND_RANGE - RAND_RANGE / 2);
    // ran1 = (ran1 == 0)? 1.0 : 1.0 / ran1;
    // double ran2 = (rand() % RAND_RANGE - RAND_RANGE / 2);
    // ran2 = (ran2 == 0)? 1.0 : 1.0 / ran2;
    // //get randome offsets
    // calnum[t] = delta_h_fourth * ran1 + delta_v_fourth * ran2;
    calnum[t] = delta_h_fourth*pow(-1, t/2) + delta_v_fourth*pow(-1, t);
  }

  I total_color{0.0, 0.0, 0.0};
  for (int t = 0; t < ANTI_ALIASING_TIMES; t++) {
    // initialize object tracing
    std::vector<int> trace;
    trace.clear();
    trace.push_back(-1);
    double traveled_distance = 0;
    // create ray
    Ray ray(scene->view_o, scene->getPixelAt(i, j)+calnum[t]);
    // check whether there is an intersection
    // and get the closest intersection point
    int index = 0;
    // get the color of the object of the closest intersection
    // if not, use the default background color
    if (auto closestPoint = scene->getRayIntersection(ray, index)) {
      // record the current object
      trace.push_back(index);
      total_color += shade_ray(ray, (*scene).objects.at(index).get(), 
        (*closestPoint), traveled_distance, trace);
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
I shade_ray(const Ray& ray, const Object* o, const Point& p, 
double& traveled_distance, std::vector<int> trace) {
  /** calculate distance **/
  traveled_distance += ray.o.getDistance(p);
  // traveled_distance++;
  /******************************************/
  Vector3D V(p, scene->view_o);
  I total_I{0.0, 0.0, 0.0};
  // Set the normal vector
  auto normal = o->getNormalVector(p, ray);
  if (!normal) { // error check
    std::cerr << "invalid normal vector" << std::endl;
    exit(-1);
  }
  Vector3D N(std::move(*normal));
  // Set Od
  auto OdLambda = o->getOdLambda(p);
  if (!OdLambda) { // error check
    std::cerr << "invalid Od" << std::endl;
    exit(-1);
  }
  I Od(*OdLambda);
  // Set Os
  I Os{o->color.sr, o->color.sg, o->color.sb};

  I I_ambient = Od*o->color.ka;

  Light *l;
  for (int i = 0; i < scene->lights.size(); i++) {
    // used Point class in order to calculate 3-tuple at once
    l = scene->lights.at(i).get();
    Vector3D L;
    Vector3D H;
    double att;  // attenuation
    double S = 1.0;// shadow flag
    // check the type of light
    switch (l->type) {
      case (LightType::DIRECTIONAL): {
        DirectionalLight* dl = dynamic_cast<DirectionalLight*>(l);
        L = -dl->dir.unit();
        att = 1.0;
        S = cast_shadow(l, L, p, 0);
        break;
      }
      case (LightType::POINT): {
        PointLight* pl = dynamic_cast<PointLight*>(l);
        L = Vector3D(p, pl->o).unit();
        att = pl->attenuationFunc(p);
        S = cast_shadow(l, L, p, 1);
        break;
      }
      case (LightType::HIGHLIGHT): {
        Highlight* hl = dynamic_cast<Highlight*>(l);
        if (hl->isInHighlight(p)) {
          L = Vector3D(p, hl->o).unit();
          att = hl->attenuationFunc(p);
          S = cast_shadow(l, L, p, 2);
          if (util::almost_equal(S, 0.0, 100)) {
            att = 0.0;
          }
        } else {
          L = Vector3D();
          att = 0.0;
        }
        break;
      }
      default:
        break;
    }

    H = ((L + V) / (L + V).mag());
    // N*L
    double N_dot_L = N.dot(L);
    // N_dot_L should not be negative
    if (N_dot_L < 0) N_dot_L = 0;
    // N*H
    double N_dot_H = N.dot(H);
    // N_dot_L should not be negative
    if (N_dot_H < 0) N_dot_H = 0;

    double N_dot_H_sq_n = pow(N_dot_H, o->color.n);
    I I_diffuse = Od * o->color.kd * N_dot_L;
    I I_specular = Os * o->color.ks * N_dot_H_sq_n;
    I IL{l->intensity.r, l->intensity.g, l->intensity.b};
    // total_I += I
    // apply shadows
    total_I = total_I + (IL*(I_diffuse + I_specular)*att*S);
  }

  I I_lambda = I_ambient + total_I;

  /******** set R_lambda ********/
  I R_lambda{0.0, 0.0, 0.0};
  /******** get Fr ********/
  int curr_index = trace.size() - 1;
  int curr_obj_index = trace.at(curr_index);
  double index_of_refraction_t = 1.0;
  if (curr_obj_index > -1)
    index_of_refraction_t = scene->objects.at(curr_obj_index)->iRefraction;
  int prev_index = trace.size() - 2;
  int prev_obj_index = trace.at(prev_index);
  double index_of_refraction_i = 1.0;
  if (prev_obj_index > -1)
    index_of_refraction_i = scene->objects.at(prev_obj_index)->iRefraction;

  double F0_00 = index_of_refraction_t - index_of_refraction_i;
  double F0_01 = index_of_refraction_t + index_of_refraction_i;
  double F0_1 = F0_00 / F0_01;
  double F0 = F0_1 * F0_1;
  Vector3D incoming_vector(-ray.v);
  double cos_incoming_theta = N.dot(incoming_vector);
  double Fr_0 = pow(1.0 - cos_incoming_theta, 5);
  double Fr = F0 + (1.0 - F0) * Fr_0;
  // Fr = pow(Fr, traveled_distance);
  /* specular reflection */
  if (traveled_distance < MAX_DISTANCE && !util::almost_equal(Fr, 0.0, 100)) {
    double new_dist = traveled_distance;
    Vector3D R((N*(N.dot(incoming_vector)))*2.0 - incoming_vector);
    Ray new_ray(p, R);
    // check whether there is an intersection
    // and get the closest intersection point
    int index = 0;
    // get the color of the reflection
    // if not, no reflection.
    if (auto closestPoint = scene->getRayIntersection(new_ray, index)) {
      trace.push_back(-1);
      trace.push_back(index);
      /*** recursive loop ***/
      R_lambda = shade_ray(new_ray, scene->objects.at(index).get(), *closestPoint, new_dist, trace);
      trace.pop_back();
      trace.pop_back();
    }
  }
  // Fr * R_lambda
  I I_R = R_lambda * o->color.ks;
  /***********************/
  /* refraction */
  double angle_of_i = incoming_vector.getAngle(N) / 180.0 * util::PI;
  double temp_r = index_of_refraction_t / index_of_refraction_i;
  double critical_angle = asin(temp_r);
  I T_lambda{0.0, 0.0, 0.0};

  bool internal_reflection = sin(angle_of_i) > temp_r && angle_of_i >= critical_angle;
  if (!internal_reflection && !util::almost_equal(o->opacity, 1.0, 100)) {
    if (traveled_distance < MAX_DISTANCE) {
      double new_dist = traveled_distance;

      /** get T vector**/
      double ratio = index_of_refraction_i/index_of_refraction_t;
      double T_000 = ratio*ratio;
      double T_001 = 1.0 - cos_incoming_theta*cos_incoming_theta;
      double T_00 = 1.0 - T_000*T_001;
      double T_01 = sqrt(T_00);
      Vector3D T_0 = (-N)*T_01;
      Vector3D T_100 = N*cos_incoming_theta;
      Vector3D T_10 = T_100-incoming_vector;
      Vector3D T_1 = T_10*ratio;
      Vector3D T = T_0 + T_1;

      Ray new_ray(p, T);
      // check whether there is an intersection
      // and get the closest intersection point
      int index = 0;
      // get the color of the reflection
      // if not, no reflection.
      if (auto closestPoint = scene->getRayIntersection(new_ray, index)) {
        if (trace.back() == index) {
          trace.push_back(index);
          trace.push_back(-1);
        } else {
          trace.push_back(-1);
          trace.push_back(index);
        }
        /*** recursive loop ***/
        T_lambda = shade_ray(new_ray, scene->objects.at(index).get(), *closestPoint, new_dist, trace);
        trace.pop_back();
        trace.pop_back();
      }
    }
    T_lambda = T_lambda * (1.0 - Fr) * (1 - o->opacity);
  }
  I I_T = T_lambda;
  // depth cueing
  I I_lambda_prime = scene->getDepthCueing(I_lambda, p);
  I intensity{I_lambda_prime.r, I_lambda_prime.g, I_lambda_prime.b};
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
double cast_shadow(Light* l, const Vector3D& L, const Point& p, int flag) {
  double S = 1.0;  // no shadow

  switch (l->type) {
    case (LightType::DIRECTIONAL): {
      Ray ray(p, L);
      int index = 0;
      if (auto closestPoint = scene->getRayIntersection(ray, index))
        return S;  // no shadow S = 1
      else {
        S *= (1.0 - scene->objects.at(index)->opacity);
      }
      break;
    }
    case (LightType::POINT): {
      PointLight *pl = dynamic_cast<PointLight *>(l);
      double S_sum = 0.0;
      for (int t = 0; t < SHADOW_RAY_CASTING_TIMES; t++) {
        /*get soft shadow*/
        int index = 0;
        double shadow_offset_theta = pl->getTheta(p);
        // prevent from dividing number by 0
        double ran1 = (rand()%RAND_RANGE - RAND_RANGE/2.0);
        ran1 = ran1 == 0 ? 1.0 : 1.0 / ran1;
        double ran2 = (rand()%RAND_RANGE - RAND_RANGE/2.0);
        ran2 = ran2 == 0 ? 1.0 : 1.0 / ran2;
        double ran3 = (rand()%RAND_RANGE - RAND_RANGE/2.0);
        ran3 = ran3 == 0 ? 1.0 : 1.0 / ran3;
        // get random offset angles
        double ran_x_theta = shadow_offset_theta * ran1;
        double ran_y_theta = shadow_offset_theta * ran2;
        double ran_z_theta = shadow_offset_theta * ran3;
        Vector3D random_L = L.rotate(ran_x_theta, ran_y_theta, ran_z_theta);
        // generate random ray in a boundary
        Ray random_ray(p, random_L);

        if (auto closestPoint = scene->getRayIntersection(random_ray, index)) {
          double dist = (pl->o).getDistance(p);
          double dist2 = (pl->o).getDistance(*closestPoint);
          if (0 < dist2 && dist2 < dist && (*closestPoint != p))
            S_sum += (1.0 - scene->objects.at(index)->opacity);
          else
            S_sum += 1.0;
        } else
          S_sum += 1.0;
      }
      // calculate average S
      if (!util::almost_equal(S_sum, 0.0, 100))
        S = S_sum / SHADOW_RAY_CASTING_TIMES;
      else
        S = 0.0;
      break;
    }
    case (LightType::HIGHLIGHT): {
      Highlight *hl = dynamic_cast<Highlight *>(l);
      double S_sum = 0.0;
      for (int t = 0; t < SHADOW_RAY_CASTING_TIMES; t++) {
        /*get soft shadow*/
        int index = 0;
        double shadow_offset_theta = hl->getTheta(p);
        // prevent from dividing number by 0
        double ran1 = (rand()%RAND_RANGE - RAND_RANGE/2.0);
        ran1 = ran1 == 0 ? 1.0 : 1.0 / ran1;
        double ran2 = (rand()%RAND_RANGE - RAND_RANGE/2.0);
        ran2 = ran2 == 0 ? 1.0 : 1.0 / ran2;
        double ran3 = (rand()%RAND_RANGE - RAND_RANGE/2.0);
        ran3 = ran3 == 0 ? 1.0 : 1.0 / ran3;
        // get random offset angles
        double ran_x_theta = shadow_offset_theta * ran1;
        double ran_y_theta = shadow_offset_theta * ran2;
        double ran_z_theta = shadow_offset_theta * ran3;
        Vector3D random_L = L.rotate(ran_x_theta, ran_y_theta, ran_z_theta);
        // generate random ray in a boundary
        Ray random_ray(p, random_L);

        if (auto closestPoint = scene->getRayIntersection(random_ray, index)) {
          double dist = (hl->o).getDistance(p);
          double dist2 = (hl->o).getDistance(*closestPoint);
          if (0 < dist2 && dist2 < dist && (*closestPoint != p))
            S_sum += (1.0 - scene->objects.at(index)->opacity);
          else
            S_sum += 1.0;
        } else
          S_sum += 1.0;
      }
      // calculate average S
      if (!util::almost_equal(S_sum, 0.0, 100))
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
      break;
    }
    default:
      break;
  }

  return S;
}
