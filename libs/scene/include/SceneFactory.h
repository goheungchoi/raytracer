#ifndef _SCENE_FACTORY_H_
#define _SCENE_FACTORY_H_

#include <math.h>
#include <stdlib.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

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
  ~SceneFactory() {
    delete stream;
    delete scene;
  }

 private:
  FileStream* stream;
  Scene* scene;
  void get_object_from_stream();
  void get_light_from_stream();
  void get_ratio_of_intensity_of_each_light();
  void get_depth_cueing_from_stream();
  void convert_RGB_to_Intensity(const RGB& img, util::arr3& px);
};

#endif  // _SCENE_FACTORY_H_
