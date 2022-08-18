#ifndef _FILE_READER_
#define _FILE_READER_

#include <math.h>
#include <stdlib.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "cylinder.h"
#include "light.h"
#include "object.h"
#include "plane.h"
#include "point.h"
#include "sphere.h"
#include "triangle.h"
#include "triangle_mesh.h"
#include "vector3D.h"
#include "scene.h"
#include "file_stream.h"

class SceneFactory {
 public:
  /**
  * @brief Reads the file and constructs a new scene factory
  * @param filename
  */
  SceneFactory(std::string filename);
  Scene* CreateScene();
  int CreateImagePixel(std::string pix);

 private:
  FileStream* stream;
  Scene* scene;
  void get_object_from_stream();
  void get_light_from_stream();
  void get_triangle_mesh_from_stream();
  void get_ratio_of_intensity_of_each_light();
  void get_depth_cueing_from_stream();
  void convert_RGB_to_Intensity(const RGB& img, Point& px);
};

#endif