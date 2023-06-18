#include "SceneFactory.h"


SceneFactory::SceneFactory(std::string filename) {
  stream = new FileStream(filename);
}

Scene* SceneFactory::CreateScene() {
  scene = new Scene(
    Point(stream->eye[0], stream->eye[1], stream->eye[2]),
    Vector3D(stream->view_dir[0], stream->view_dir[1], stream->view_dir[2]),
    Vector3D(stream->up_dir[0], stream->up_dir[1], stream->up_dir[2]),
    *stream->vfov, stream->imsize[0], stream->imsize[1], 5
  );
  // set background color
  scene->bkgColor = I{stream->bkg_color[0], stream->bkg_color[1], stream->bkg_color[2]};
  // set objects
  get_object_from_stream();
  // set lights
  get_light_from_stream();
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
  int count = 0;
  object* o = stream->getObject();
  while (o != nullptr) {
    /// Create Sphere Object
    if ((*o).type.compare("sphere") == 0) {
      std::cout << "reading object: " << (*o).type << std::endl;
      std::cout << "#: " << count << std::endl;
      sphere* s = static_cast<sphere*>(o);
      Sphere newSphere(// Color of the object
        obj::color_t{
          (float)(*o).color[0], (float)(*o).color[1],
          (float)(*o).color[2], (float)(*o).color[3],
          (float)(*o).color[4], (float)(*o).color[5],
          (float)(*o).color[6], (float)(*o).color[7],
          (float)(*o).color[8], (int)(*o).color[9]},
        // Opacity   // iRefrection
        (*o).color[10], (*o).color[11],
        // Origin
        Point(s->origin[0], s->origin[1], s->origin[2]),
        // Radius
        s->radius
      );

      // texture_mapping
      newSphere.texture_mapping = s->texture_mapping;
      if (newSphere.texture_mapping) {
        ImageRGB image = s->texture;
        newSphere.textureMap.w = image.w;
        newSphere.textureMap.h = image.h;
        for (const RGB& rgb : image.data) {
          util::arr3 arr;
          convert_RGB_to_Intensity(rgb, arr);
          newSphere.textureMap.map.push_back(arr);
        }
      }
      // normal mapping
      newSphere.normal_mapping = s->normal_mapping;
      if (newSphere.normal_mapping) {
        ImageRGB image = s->normal_map;
        newSphere.normalMap.w = image.w;
        newSphere.normalMap.h = image.h;
        for (const RGB& rgb : image.data) {
          util::arr3 arr;
          if (128 <= rgb.b && rgb.b <= 255)
            convert_RGB_to_Intensity(rgb, arr);
          else
            throw std::invalid_argument("invalid normal map");
          newSphere.normalMap.map.push_back(arr);
        }
      }

      std::cout << "object created." << std::endl;
      // store the object in the scene
      std::unique_ptr<Object> pSphere(new Sphere(std::move(newSphere)));
      (*scene).addObjectUniquePtr(std::move(pSphere));
      std::cout << "object stored." << std::endl;
    }
    /// Create Cylinder Object 
    else if ((*o).type.compare("cylinder") == 0) {
      std::cout << "reading object: " << (*o).type << std::endl;
      std::cout << "#: " << count << std::endl;
      cylinder* c = static_cast<cylinder*>(o);
      Cylinder newCylinder(
        // Color of the object
        obj::color_t{
          (float)(*o).color[0], (float)(*o).color[1],
          (float)(*o).color[2], (float)(*o).color[3],
          (float)(*o).color[4], (float)(*o).color[5],
          (float)(*o).color[6], (float)(*o).color[7],
          (float)(*o).color[8], (int)(*o).color[9]},
        // Opacity   // iRefrection
        (*o).color[10], (*o).color[11],
        // Origin
        Point(c->origin[0], c->origin[1], c->origin[2]),
        // Direction
        Vector3D(c->dir[0], c->dir[1], c->dir[2]),
        // Radius  // Length
        c->radius, c->length
      );        
      // texture_mapping
      newCylinder.texture_mapping = c->texture_mapping;
      if (newCylinder.texture_mapping) {
        ImageRGB image = c->texture;
        newCylinder.textureMap.w = image.w;
        newCylinder.textureMap.h = image.h;
        for (RGB rgb : image.data) {
          util::arr3 arr;
          convert_RGB_to_Intensity(rgb, arr);
          newCylinder.textureMap.map.push_back(arr);
        }
      }
      // normal mapping
      newCylinder.normal_mapping = c->normal_mapping;
      if (newCylinder.normal_mapping) {
        ImageRGB image = c->normal_map;
        newCylinder.normalMap.w = image.w;
        newCylinder.normalMap.h = image.h;
        for (RGB rgb : image.data) {
          util::arr3 arr;
          if (128 <= rgb.b && rgb.b <= 255)
            convert_RGB_to_Intensity(rgb, arr);
          else
            throw std::invalid_argument("invalid normal map");
          newCylinder.normalMap.map.push_back(arr);
        }
      }
      std::cout << "object created." << std::endl;
      // store the object in the scene
      std::unique_ptr<Object> pCylinder(new Cylinder(std::move(newCylinder)));
      (*scene).addObjectUniquePtr(std::move(pCylinder));
      std::cout << "object stored." << std::endl;
    }
    /// Create Plane Object
    else if ((*o).type.compare("plane") == 0) {
      std::cout << "reading object: " << (*o).type << std::endl;
      std::cout << "#: " << count << std::endl;
      plane* p = static_cast<plane*>(o);
      Plane newPlane(
        // Color of the object
        obj::color_t{
          (float)(*o).color[0], (float)(*o).color[1],
          (float)(*o).color[2], (float)(*o).color[3],
          (float)(*o).color[4], (float)(*o).color[5],
          (float)(*o).color[6], (float)(*o).color[7],
          (float)(*o).color[8], (int)(*o).color[9]},
        // Opacity   // iRefrection
        (*o).color[10], (*o).color[11],
        // Normal vector
        Vector3D(p->n[0], p->n[1], p->n[2]),
        // Origin
        Point(p->p0[0], p->p0[1], p->p0[2]),
        // Direction
        Vector3D(p->dir[0], p->dir[1], 0.0), 
        // Width and height
        p->w, p->h
      );
      // texture_mapping
      newPlane.texture_mapping = p->texture_mapping;
      if (newPlane.texture_mapping) {
        ImageRGB image = p->texture;
        newPlane.textureMap.w = image.w;
        newPlane.textureMap.h = image.h;
        for (RGB rgb : image.data) {
          util::arr3 arr;
          convert_RGB_to_Intensity(rgb, arr);
          newPlane.textureMap.map.push_back(arr);
        }
      }
      // normal mapping
      newPlane.normal_mapping = p->normal_mapping;
      if (newPlane.normal_mapping) {
        ImageRGB image = p->normal_map;
        newPlane.normalMap.w = image.w;
        newPlane.normalMap.h = image.h;
        for (RGB rgb : image.data) {
          util::arr3 arr;
          if (128 <= rgb.b && rgb.b <= 255)
            convert_RGB_to_Intensity(rgb, arr);
          else
            throw std::invalid_argument("invalid normal map");
          newPlane.normalMap.map.push_back(arr);
        }
      }
      std::cout << "object created." << std::endl;
      // store the object in the scene
      std::unique_ptr<Object> pPlane(new Plane(std::move(newPlane)));
      (*scene).addObjectUniquePtr(std::move(pPlane));
      std::cout << "object stored." << std::endl;
    }
    /// Create Triangle Mesh Object 
    else if ((*o).type.compare("mesh") == 0) {
      std::cout << "reading object: " << (*o).type << std::endl;
      std::cout << "#: " << count << std::endl;
      triangle_mesh* tri_mesh = static_cast<triangle_mesh*>(o);
      // color
      TriangleMesh newMesh(obj::color_t{
        (float)(*o).color[0], (float)(*o).color[1],
        (float)(*o).color[2], (float)(*o).color[3],
        (float)(*o).color[4], (float)(*o).color[5],
        (float)(*o).color[6], (float)(*o).color[7],
        (float)(*o).color[8], (int)(*o).color[9]},
        (*o).color[10], (*o).color[11]);
      // texture_mapping
      newMesh.texture_mapping = tri_mesh->texture_mapping;
      if (newMesh.texture_mapping) {
        ImageRGB image = tri_mesh->texture;
        newMesh.textureMap.w = image.w;
        newMesh.textureMap.h = image.h;
        for (RGB rgb : image.data) {
          util::arr3 arr;
          convert_RGB_to_Intensity(rgb, arr);
          newMesh.textureMap.map.push_back(arr);
        }
      }
      // normal mapping
      newMesh.normal_mapping = tri_mesh->normal_mapping;
      if (newMesh.normal_mapping) {
        ImageRGB image = tri_mesh->normal_map;
        newMesh.normalMap.w = image.w;
        newMesh.normalMap.h = image.h;
        for (RGB rgb : image.data) {
          util::arr3 arr;
          if (128 <= rgb.b && rgb.b <= 255)
            convert_RGB_to_Intensity(rgb, arr);
          else
            throw std::invalid_argument("invalid normal map");
          newMesh.normalMap.map.push_back(arr);
        }
      }

      for (const face& f : tri_mesh->triangles) {
        Point p0(f.v0.x, f.v0.y, f.v0.z);
        Point p1(f.v1.x, f.v1.y, f.v1.z);
        Point p2(f.v2.x, f.v2.y, f.v2.z);
        Triangle tri(newMesh.color, newMesh.opacity, newMesh.iRefraction, p0, p1, p2);
        tri.smooth_shading = f.smooth_shading;
        if (f.texture_mapping && tri_mesh->texture_mapping) {
          tri.texture_mapping = f.texture_mapping;
          obj::text_coor_t c0{f.vt0.u, f.vt0.v};
          obj::text_coor_t c1{f.vt1.u, f.vt1.v};
          obj::text_coor_t c2{f.vt2.u, f.vt2.v};
          tri.c0 = c0;
          tri.c1 = c1;
          tri.c2 = c2;
        }
        if (f.normal_mapping && tri_mesh->normal_mapping) {
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
        newMesh.addTriangle(tri);
      }

      std::unique_ptr<Object> pMesh(new TriangleMesh(std::move(newMesh)));
      (*scene).addObjectUniquePtr(std::move(pMesh));
    } else
      throw std::invalid_argument("ERROR: wrong type of object");
    // get object and check whether it is a null obj, name is ""
    count++;
    o = stream->getObject();
  }
}

void SceneFactory::get_light_from_stream() {
  // get lights
  light* l = stream->getLight();
  // initialized value of l.s is -1
  // -> if l is not initialized, terminate
  for (int i = 0; l != nullptr; i++) {
    // get point, pixel_color, attenuation values
    std::cout << "reading light: " << (*l).s << std::endl;
    std::cout << "#: " << i << std::endl;
    /// Get directional light
    if ((*l).s == 0) {
      // get point, pixel_color, attenuation values
      Vector3D v((*l).arr[0], (*l).arr[1], (*l).arr[2]);
      I i{(*l).arr[3], (*l).arr[4], (*l).arr[5]};
      DirectionalLight newDirectionalLight(i, v);
      std::cout << "light created." << std::endl;
      // store the light in the scene
      (*scene).addLightUniquePtr(std::make_unique<DirectionalLight>(newDirectionalLight));
      std::cout << "light stored." << std::endl;
    }
    /// Get point light
    else if ((*l).s == 1) {
      // get point, pixel_color, attenuation values
      Point p((*l).arr[0], (*l).arr[1], (*l).arr[2]);
      I i{(*l).arr[3], (*l).arr[4], (*l).arr[5]};
      double c1 = (*l).arr[6], c2 = (*l).arr[7], c3 = (*l).arr[8];
      double radius = (*l).radius;
      // create light
      PointLight newPointLight(i, p, radius);
      // set attenuation values
      newPointLight.setAttenuation(c1, c2, c3);
      std::cout << "light created." << std::endl;
      // store the light in the scene
      (*scene).addLightUniquePtr(std::make_unique<PointLight>(newPointLight));
      std::cout << "light stored." << std::endl;
    }
    /// Get highlight
    else if ((*l).s == 2) {
      // get point, pixel_color, vector, cutoff
      Point p((*l).arr[0], (*l).arr[1], (*l).arr[2]);
      I i{(*l).arr[3], (*l).arr[4], (*l).arr[5]};
      Vector3D v((*l).arr[6], (*l).arr[7], (*l).arr[8]);
      double cutoff = (*l).angle;
      // create light
      Highlight newHighligh(i, p, 5.0, v, cutoff);
      std::cout << "light created." << std::endl;
      // store the light in the scene
      (*scene).addLightUniquePtr(std::make_unique<Highlight>(newHighligh));
      std::cout << "light stored." << std::endl;
    } else
      // if l.s is not 0 or 1 or 2
      throw std::invalid_argument("ERROR: wrong type of light");
    // get next light and check whether it is a null light or not
    l = stream->getLight();
  }
  get_ratio_of_intensity_of_each_light();
}

void SceneFactory::get_ratio_of_intensity_of_each_light() {
  int num_of_lights = scene->lights.size();
  Point total_ILs(0.0, 0.0, 0.0);
  for (int i = 0; i < num_of_lights; i++) {
    const auto intensity = scene->lights.at(i).get()->intensity;
    total_ILs = total_ILs + Point(intensity.r, intensity.g, intensity.b);
  }

  if (total_ILs.x > 1.0 || total_ILs.y > 1.0 || total_ILs.z > 1.0) {
    for (int i = 0; i < num_of_lights; i++) {
      auto l = scene->lights.at(i).get();
      l->intensity.r /= total_ILs.x;
      l->intensity.g /= total_ILs.y;
      l->intensity.b /= total_ILs.z;
    }
  }
}

void SceneFactory::get_depth_cueing_from_stream() {
  // turn on depth cueing
  scene->depth_cueing_on = true;
  // set color of intensity
  scene->depth_cue.I_dc = I{
    stream->depthcueing[0],
    stream->depthcueing[1],
    stream->depthcueing[2]
  };
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

void SceneFactory::convert_RGB_to_Intensity(const RGB& img, util::arr3& px) {
  px.x = ((double)img.r) / 255.0;
  px.y = ((double)img.g) / 255.0;
  px.z = ((double)img.b) / 255.0;
}
