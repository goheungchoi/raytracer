#include "SceneFactory.h"


SceneFactory::SceneFactory(std::string filename) {
  stream = new FileStream(filename);
}

Scene* SceneFactory::CreateScene() {
  scene = new Scene(
      Point(stream->eye[0], stream->eye[1], stream->eye[2]),
      Vector3D(stream->view_dir[0], stream->view_dir[1], stream->view_dir[2]),
      Vector3D(stream->up_dir[0], stream->up_dir[1], stream->up_dir[2]),
      *stream->vfov, stream->imsize[0], stream->imsize[1], 5);
  // set background color
  I bkgcolor;
  bkgcolor.Ir = stream->bkg_color[0];
  bkgcolor.Ig = stream->bkg_color[1];
  bkgcolor.Ib = stream->bkg_color[2];
  scene->bkgColor = bkgcolor;
  // set objects
  get_object_from_stream();
  // set lights
  get_light_from_stream();
  // set triangles
  get_triangle_mesh_from_stream();
  // set depth cueing
  if (stream->depthcueing_on) get_depth_cueing_from_stream();
  return scene;
}

int SceneFactory::CreateImagePixel(std::string pix) {
    stream->writeFile(pix);
    return 1;
}

void SceneFactory::get_object_from_stream() {
  // get objects
  object o = stream->getObject();
  for (int i = 0; o.type.compare("") != 0; i++) {
    if (o.type.compare("sphere") == 0) {
      std::cout << "reading object: " << o.type << std::endl;
      std::cout << "#: " << i << std::endl;
      Object* newObj =
          new Sphere(std::to_string(i),  // name
                                         // color of the object
                     color{(float)o.sph.color[0], (float)o.sph.color[1],
                           (float)o.sph.color[2], (float)o.sph.color[3],
                           (float)o.sph.color[4], (float)o.sph.color[5],
                           (float)o.sph.color[6], (float)o.sph.color[7],
                           (float)o.sph.color[8], (int)o.sph.color[9]},
                     o.sph.color[10], o.sph.color[11],
                     Point(o.sph.origin[0], o.sph.origin[1], o.sph.origin[2]),
                     o.sph.radius);

      // texture_mapping
      newObj->texture_mapping = o.sph.texture_mapping;
      if (newObj->texture_mapping) {
        ImageRGB image = o.sph.texture;
        newObj->texture_map.w = image.w;
        newObj->texture_map.h = image.h;
        for (RGB rgb : image.data) {
          Point p;
          convert_RGB_to_Intensity(rgb, p);
          newObj->texture_map.map.push_back(p);
        }
      }
      // normal mapping
      newObj->normal_mapping = o.sph.normal_mapping;
      if (newObj->normal_mapping) {
        ImageRGB image = o.sph.normal_map;
        newObj->normal_map.w = image.w;
        newObj->normal_map.h = image.h;
        for (RGB rgb : image.data) {
          Point p;
          if (128 <= rgb.b && rgb.b <= 255)
            convert_RGB_to_Intensity(rgb, p);
          else
            throw std::invalid_argument("invalid normal map");
          newObj->normal_map.map.push_back(p);
        }
      }
      std::cout << "object created." << std::endl;
      // store the object in the scene
      scene->obj.push_back(newObj);
      std::cout << "object stored." << std::endl;
    } else if (o.type.compare("cylinder") == 0) {
      std::cout << "reading object: " << o.type << std::endl;
      std::cout << "#: " << i << std::endl;
      Object* newObj =
          new Cylinder(std::to_string(i),
                       color{(float)o.cyl.color[0], (float)o.cyl.color[1],
                             (float)o.cyl.color[2], (float)o.cyl.color[3],
                             (float)o.cyl.color[4], (float)o.cyl.color[5],
                             (float)o.cyl.color[6], (float)o.cyl.color[7],
                             (float)o.cyl.color[8], (int)o.cyl.color[9]},
                       o.cyl.color[10], o.cyl.color[11],
                       Point(o.cyl.origin[0], o.cyl.origin[1], o.cyl.origin[2]),
                       Vector3D(o.cyl.dir[0], o.cyl.dir[1], o.cyl.dir[2]),
                       o.cyl.radius, o.cyl.length);
      // texture_mapping
      newObj->texture_mapping = o.cyl.texture_mapping;
      if (newObj->texture_mapping) {
        ImageRGB image = o.cyl.texture;
        newObj->texture_map.w = image.w;
        newObj->texture_map.h = image.h;
        for (RGB rgb : image.data) {
          Point p;
          convert_RGB_to_Intensity(rgb, p);
          newObj->texture_map.map.push_back(p);
        }
      }
      // normal mapping
      newObj->normal_mapping = o.cyl.normal_mapping;
      if (newObj->normal_mapping) {
        ImageRGB image = o.cyl.normal_map;
        newObj->normal_map.w = image.w;
        newObj->normal_map.h = image.h;
        for (RGB rgb : image.data) {
          Point p;
          if (128 <= rgb.b && rgb.b <= 255)
            convert_RGB_to_Intensity(rgb, p);
          else
            throw std::invalid_argument("invalid normal map");
          newObj->normal_map.map.push_back(p);
        }
      }
      std::cout << "object created." << std::endl;
      // store the object in the scene
      scene->obj.push_back(newObj);
      std::cout << "object stored." << std::endl;
    } else if (o.type.compare("plane") == 0) {
      std::cout << "reading object: " << o.type << std::endl;
      std::cout << "#: " << i << std::endl;
      Object* newObj = new Plane(
          std::to_string(i),  // name
                              // color of the object
          color{(float)o.pln.color[0], (float)o.pln.color[1],
                (float)o.pln.color[2], (float)o.pln.color[3],
                (float)o.pln.color[4], (float)o.pln.color[5],
                (float)o.pln.color[6], (float)o.pln.color[7],
                (float)o.pln.color[8], (int)o.pln.color[9]},
          o.pln.color[10], o.pln.color[11],
          Vector3D(o.pln.n[0], o.pln.n[1], o.pln.n[2]),
          Point(o.pln.p0[0], o.pln.p0[1], o.pln.p0[2]),
          Vector3D(o.pln.dir[0], o.pln.dir[1], 0.0), o.pln.w, o.pln.h);
      // texture_mapping
      newObj->texture_mapping = o.pln.texture_mapping;
      if (newObj->texture_mapping) {
        ImageRGB image = o.pln.texture;
        newObj->texture_map.w = image.w;
        newObj->texture_map.h = image.h;
        for (RGB rgb : image.data) {
          Point p;
          convert_RGB_to_Intensity(rgb, p);
          newObj->texture_map.map.push_back(p);
        }
      }
      // normal mapping
      newObj->normal_mapping = o.pln.normal_mapping;
      if (newObj->normal_mapping) {
        ImageRGB image = o.pln.normal_map;
        newObj->normal_map.w = image.w;
        newObj->normal_map.h = image.h;
        for (RGB rgb : image.data) {
          Point p;
          if (128 <= rgb.b && rgb.b <= 255)
            convert_RGB_to_Intensity(rgb, p);
          else
            throw std::invalid_argument("invalid normal map");
          newObj->normal_map.map.push_back(p);
        }
      }
      std::cout << "object created." << std::endl;
      // store the object in the scene
      scene->obj.push_back(newObj);
      std::cout << "object stored." << std::endl;
    } else
      throw std::invalid_argument("ERROR: wrong type of object");
    // get object and check whether it is a null obj, name is ""
    o = stream->getObject();
    std::cout << "next object type: " << o.type << std::endl;
  }
}

void SceneFactory::get_light_from_stream() {
  // get lights
  light l = stream->getLight();
  // initialized value of l.s is -1
  // -> if l is not initialized, terminate
  for (int i = 0; l.s != -1; i++) {
    // get point, pixel_color, attenuation values
    std::cout << "reading light: " << l.s << std::endl;
    std::cout << "#: " << i << std::endl;
    // get directional light
    if (l.s == 0) {
      // get point, pixel_color, attenuation values
      Vector3D v(l.arr[0], l.arr[1], l.arr[2]);
      I i;
      i.Ir = l.arr[3];
      i.Ig = l.arr[4], i.Ib = l.arr[5];
      // create light
      Light* newLight = new DirectionalLight(i, v);
      std::cout << "light created." << std::endl;
      // store the light in the scene
      scene->lights.push_back(newLight);
      std::cout << "light stored." << std::endl;
    }
    // get point light
    else if (l.s == 1) {
      // get point, pixel_color, attenuation values
      Point p(l.arr[0], l.arr[1], l.arr[2]);
      I i;
      i.Ir = l.arr[3];
      i.Ig = l.arr[4], i.Ib = l.arr[5];
      double c1 = l.arr[6], c2 = l.arr[7], c3 = l.arr[8];
      double radius = l.radius;
      // create light
      Light* newLight = new PointLight(i, p, radius);
      // set attenuation values
      if (dynamic_cast<PointLight*>(newLight) == nullptr)
        std::cout << "ERROR: type casting failed. - 1" << std::endl;
      dynamic_cast<PointLight*>(newLight)->setAttenuation(c1, c2, c3);
      std::cout << "light created." << std::endl;
      // store the light in the scene
      scene->lights.push_back(newLight);
      std::cout << "light stored." << std::endl;
    }
    // get highlight
    else if (l.s == 2) {
      // get point, pixel_color, vector, cutoff
      Point p(l.arr[0], l.arr[1], l.arr[2]);
      I i;
      i.Ir = l.arr[3];
      i.Ig = l.arr[4], i.Ib = l.arr[5];
      Vector3D v(l.arr[6], l.arr[7], l.arr[8]);
      double cutoff = l.angle;
      // create light
      Light* newLight = new HighLight(i, p, v, cutoff);
      std::cout << "light created." << std::endl;
      // store the light in the scene
      scene->lights.push_back(newLight);
      std::cout << "light stored." << std::endl;
    } else
      // if l.s is not 0 or 1 or 2
      throw std::invalid_argument("ERROR: wrong type of light");
    // get next light and check whether it is a null light or not
    l = stream->getLight();
    std::cout << "next light type: " << l.s << std::endl;
  }
  get_ratio_of_intensity_of_each_light();
}

void SceneFactory::get_triangle_mesh_from_stream() {
  for (triangle_mesh& tri_mesh : stream->tri_meshes) {
    // color
    color clr{(float)tri_mesh.color[0], (float)tri_mesh.color[1],
              (float)tri_mesh.color[2], (float)tri_mesh.color[3],
              (float)tri_mesh.color[4], (float)tri_mesh.color[5],
              (float)tri_mesh.color[6], (float)tri_mesh.color[7],
              (float)tri_mesh.color[8], (int)tri_mesh.color[9]};
    Object* mesh =
        new TriangleMesh("", clr, tri_mesh.color[10], tri_mesh.color[11]);
    // texture_mapping
    mesh->texture_mapping = tri_mesh.texture_mapping;
    if (mesh->texture_mapping) {
      ImageRGB image = tri_mesh.texture;
      mesh->texture_map.w = image.w;
      mesh->texture_map.h = image.h;
      for (RGB rgb : image.data) {
        Point p;
        convert_RGB_to_Intensity(rgb, p);
        mesh->texture_map.map.push_back(p);
      }
    }
    // normal mapping
    mesh->normal_mapping = tri_mesh.normal_mapping;
    if (mesh->normal_mapping) {
      ImageRGB image = tri_mesh.normal_map;
      mesh->normal_map.w = image.w;
      mesh->normal_map.h = image.h;
      for (RGB rgb : image.data) {
        Point p;
        if (128 <= rgb.b && rgb.b <= 255)
          convert_RGB_to_Intensity(rgb, p);
        else
          throw std::invalid_argument("invalid normal map");
        mesh->normal_map.map.push_back(p);
      }
    }

    for (face& f : tri_mesh.triangles) {
      Point p0(f.v0.x, f.v0.y, f.v0.z);
      Point p1(f.v1.x, f.v1.y, f.v1.z);
      Point p2(f.v2.x, f.v2.y, f.v2.z);
      Triangle tri("", mesh->clr, mesh->opacity, mesh->index_of_refraction, p0,
                   p1, p2);
      tri.smooth_shading = f.smooth_shading;
      if (f.texture_mapping && tri_mesh.texture_mapping) {
        tri.texture_mapping = f.texture_mapping;
        text_coor c0{f.vt0.u, f.vt0.v};
        text_coor c1{f.vt1.u, f.vt1.v};
        text_coor c2{f.vt2.u, f.vt2.v};
        tri.c0 = c0;
        tri.c1 = c1;
        tri.c2 = c2;
      }
      if (f.normal_mapping && tri_mesh.normal_mapping) {
        tri.normal_mapping = f.normal_mapping;
        tri.getTextureVectors();
      }
      if (f.smooth_shading) {
        tri.normal_mapping = f.normal_mapping;
        Vector3D vn0{f.vn0.x, f.vn0.y, f.vn0.z};
        Vector3D vn1{f.vn1.x, f.vn1.y, f.vn1.z};
        Vector3D vn2{f.vn2.x, f.vn2.y, f.vn2.z};
        tri.vn0 = vn0;
        tri.vn1 = vn1;
        tri.vn2 = vn2;
      }

      TriangleMesh* tri_mesh = dynamic_cast<TriangleMesh*>(mesh);
      tri_mesh->triangles->push_back(new Triangle(tri));
    }

    scene->obj.push_back(mesh);
  }
}

void SceneFactory::get_ratio_of_intensity_of_each_light() {
  int num_of_lights = scene->lights.size();
  Point total_ILs(0.0, 0.0, 0.0);
  for (int i = 0; i < num_of_lights; i++) {
    total_ILs = total_ILs + Point(scene->lights.at(i)->intensity.Ir,
                                  scene->lights.at(i)->intensity.Ig,
                                  scene->lights.at(i)->intensity.Ib);
  }

  if (total_ILs.x > 1.0 || total_ILs.y > 1.0 || total_ILs.z > 1.0) {
    for (int i = 0; i < num_of_lights; i++) {
      Light* l = scene->lights.at(i);
      l->intensity.Ir /= total_ILs.x;
      l->intensity.Ig /= total_ILs.y;
      l->intensity.Ib /= total_ILs.z;
    }
  }
}

void SceneFactory::get_depth_cueing_from_stream() {
  // turn on depth cueing
  scene->depth_cueing_on = true;
  // set color of intensity
  Point intensity(stream->depthcueing[0], stream->depthcueing[1],
                  stream->depthcueing[2]);
  scene->depth_cue.I_dc = intensity;
  // set a_max and a_min
  if (stream->depthcueing[3] > stream->depthcueing[4]) {
    scene->depth_cue.a_max = stream->depthcueing[3];
    scene->depth_cue.a_min = stream->depthcueing[4];
  }
  // set dist_far and dist_near
  if (stream->depthcueing[5] > stream->depthcueing[6]) {
    scene->depth_cue.dist_far = stream->depthcueing[5];
    scene->depth_cue.dist_near = stream->depthcueing[6];
  }
}

void SceneFactory::convert_RGB_to_Intensity(const RGB& img, Point& px) {
  px.x = ((double)img.r) / 255.0;
  px.y = ((double)img.g) / 255.0;
  px.z = ((double)img.b) / 255.0;
}