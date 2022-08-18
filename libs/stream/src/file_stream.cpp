#include "file_stream.h"

FileStream::FileStream() {}

FileStream::FileStream(std::string filename)
    : inputfile(filename, std::ios::in)

{
  // initialize arrays
  KEYWORDS[EYE_INDEX] = EYE;                                  // 0
  KEYWORDS[VIEW_DIR_INDEX] = VIEW_DIR;                        // 1
  KEYWORDS[UP_DIR_INDEX] = UP_DIR;                            // 2
  KEYWORDS[VFOV_INDEX] = VFOV;                                // 3
  KEYWORDS[IMSIZE_INDEX] = IMSIZE;                            // 4
  KEYWORDS[BKG_COLOR_INDEX] = BKG_COLOR;                      // 5
  KEYWORDS[MTLCOLOR_INDEX] = MTLCOLOR;                        // 6
  KEYWORDS[SPHERE_INDEX] = SPHERE;                            // 7
  KEYWORDS[CYLINDER_INDEX] = CYLINDER;                        // 8
  KEYWORDS[LIGTH_INDEX] = LIGTH;                              // 9
  KEYWORDS[ATTLIGTH_INDEX] = ATTLIGTH;                        // 10
  KEYWORDS[HIGHLIGHT_INDEX] = HIGHLIGHT;                      // 11
  KEYWORDS[PLANE_INDEX] = PLANE;                              // 12
  KEYWORDS[DEPTH_CUEING_INDEX] = DEPTH_CUEING;                // 13
  KEYWORDS[VERTEX_INDEX] = VERTEX;                            // 14
  KEYWORDS[VERTEX_TEXTURE_COOR_INDEX] = VERTEX_TEXTURE_COOR;  // 15
  KEYWORDS[VERTEX_NORMAL_INDEX] = VERTEX_NORMAL;              // 16
  KEYWORDS[FACE_INDEX] = FACE;                                // 17
  KEYWORDS[TEXTURE_INDEX] = TEXTURE;                          // 18
  KEYWORDS[NORMAL_INDEX] = NORMAL;                            // 19
  KEYWORDS[BUMP_INDEX] = BUMP;                                // 20
  KEYWORDS[MESH_INDEX] = MESH;                                // 20

  lists[EYE_INDEX] = eye;
  lists[VIEW_DIR_INDEX] = view_dir;
  lists[UP_DIR_INDEX] = up_dir;
  lists[VFOV_INDEX] = vfov;
  lists[IMSIZE_INDEX] = imsize;
  lists[BKG_COLOR_INDEX] = bkg_color;
  lists[MTLCOLOR_INDEX] = mtlcolor;
  lists[DEPTH_CUEING_INDEX] = depthcueing;

  this->filename = filename;
  outputfile = filename.substr(0, filename.size() - 4).append(".ppm");
  output_stream = std::ofstream(outputfile, std::ios::out);
  readFile();
  initializeOutputFile();
}

FileStream::~FileStream() {
  inputfile.close();
  output_stream.close();
  free(vfov);
}

void FileStream::readFile() {
  bool not_found;
  while (inputfile >> input) {
    not_found = true;
    for (int i = 0; i < NUM_OF_KEYWORDS; i++) {
      if (input.compare(KEYWORDS[i]) == 0) {
        not_found = false;
        switch (i) {
          case EYE_INDEX:        // eye : Point
          case VIEW_DIR_INDEX:   // view_dir : Vector
          case UP_DIR_INDEX:     // up_dir : Vector
          case BKG_COLOR_INDEX:  // bkg_color : struct pixel {r, g, b}
            readTuple(lists[i], 3);
            break;
          case VFOV_INDEX:  // vfor : double
            readTuple(lists[i], 1);
            break;
          case IMSIZE_INDEX:  // imsize : 2-duple
            readTuple(lists[i], 2);
            break;
          case MTLCOLOR_INDEX:  // mtlcolor :
                                // struct color
                                // {dr,dg,db,sr,sg,sb,ka,kd,ks,n,alpha,etc}
            readTuple(lists[i], 12);
            mtlcolors.push_back(lists[i]);
            break;
          case DEPTH_CUEING_INDEX:
            depthcueing_on = true;
            readTuple(lists[i], 7);
            break;
          case SPHERE_INDEX:  // object::sphere
            readObject("sphere");
            break;
          case CYLINDER_INDEX:  // object::cylinder
            readObject("cylinder");
            break;
          case PLANE_INDEX:  // object::cylinder
            readObject("plane");
            break;
          case LIGTH_INDEX:      // light
          case ATTLIGTH_INDEX:   // attlight
          case HIGHLIGHT_INDEX:  // highlight
            readLight(i);
            break;
          case VERTEX_INDEX:  // object::plane
            readVertex();
            break;
          case VERTEX_TEXTURE_COOR_INDEX:  // object::plane
            readVertexTextureCoordinate();
            break;
          case VERTEX_NORMAL_INDEX:  // object::plane
            readVertexNormal();
            break;
          case FACE_INDEX:  // object::plane
            readFace();
            break;
          case TEXTURE_INDEX:  // object::plane
            readTextureMapFile();
            break;
          case NORMAL_INDEX:  // object::plane
            readNormalMapFile();
            break;
          case BUMP_INDEX:  // object::plane
            readBumpMapFile();
            break;
          case MESH_INDEX:
            readTriangleMesh();
            break;
          default:
            not_found = true;
        }
        break;
      }
    }
    if (not_found)
      throw std::invalid_argument(input.append(" is invalid keyword"));
  }
}
// triangle mash texture and maps should be writen at the end of the file
void FileStream::readTriangleMesh() {
  triangle_mesh tri_mesh;
  // mtlcolor
  double *mtlcolor = nullptr;
  if (mtlcolors.size() != 0) {
    mtlcolor = mtlcolors.back();
    mtlcolors.pop_back();
  } else
    throw std::invalid_argument("triangle mesh: no default color");
  // texture mapping
  bool texture_mapping = false;
  ImageRGB texture;
  if (textures.size() != 0) {
    texture_mapping = true;
    texture = textures.back();
    textures.pop_back();
  }
  // normal mapping
  bool normal_mapping = false;
  ImageRGB normal_map;
  if (normal_maps.size() != 0) {
    normal_mapping = true;
    normal_map = normal_maps.back();
    normal_maps.pop_back();
  }
  // use the color value from the file
  if (mtlcolor != nullptr) {
    for (int i = 0; i < 10; i++) tri_mesh.color[i] = mtlcolor[i];
  }
  // texture mapping
  if (texture_mapping) {
    tri_mesh.texture_mapping = true;
    tri_mesh.texture = texture;
  }
  // normal mapping
  if (normal_mapping) {
    tri_mesh.normal_mapping = true;
    tri_mesh.normal_map = normal_map;
  }

  inputfile >> input;
  int number_of_faces = std::stoi(input);

  tri_mesh.triangles.resize(number_of_faces);
  for (int i = 0; i < number_of_faces; i++) {
    inputfile >> input;
    int index = std::stoi(input) - 1;
    tri_mesh.triangles.at(i) = fs.at(index);
  }

  tri_meshes.push_back(tri_mesh);
}

void FileStream::initializeOutputFile() {
  output_stream << "P3\n"
                << (int)imsize[0] << " " << (int)imsize[1] << "\n"
                << 255 << "\n";
}

void FileStream::writeFile(std::string str) { output_stream << str; }
/*
    return an object one by one
    check the type of an object
 */
object FileStream::getObject() {
  if (get_obj_count > obj_count || obj_count == 0)
    return object{};
  else
    return objects[get_obj_count++];
}

light FileStream::getLight() {
  if (get_light_count > light_count || light_count == 0)
    return light{};
  else
    return lights[get_light_count++];
}

void FileStream::readTuple(double *arr, int size) {
  if (size == 1) {
    inputfile >> input;
    double val = std::stod(input);
    *arr = val;
  } else {
    for (int i = 0; i < size; i++) {
      inputfile >> input;
      arr[i] = std::stod(input);
    }
  }
}

void FileStream::readObject(std::string type) {
  // mtlcolor
  double *mtlcolor = nullptr;
  if (mtlcolors.size() != 0) {
    mtlcolor = mtlcolors.back();
    mtlcolors.pop_back();
  }
  // texture mapping
  bool texture_mapping = false;
  ImageRGB texture;
  if (textures.size() != 0) {
    texture_mapping = true;
    texture = textures.back();
    textures.pop_back();
  }
  // normal mapping
  bool normal_mapping = false;
  ImageRGB normal_map;
  if (normal_maps.size() != 0) {
    normal_mapping = true;
    normal_map = normal_maps.back();
    normal_maps.pop_back();
  }
  // generate the certain type of object
  if (type == "sphere") {
    sphere s;
    s.name = std::to_string(obj_count);
    // use the color value from the file
    if (mtlcolor != nullptr) {
      for (int i = 0; i < 12; i++) s.color[i] = mtlcolor[i];
    }
    // texture mapping
    if (texture_mapping) {
      s.texture_mapping = true;
      s.texture = texture;
    }
    // normal mapping
    if (normal_mapping) {
      s.normal_mapping = true;
      s.normal_map = normal_map;
    }

    // read origin
    for (int i = 0; i < 3; i++) {
      inputfile >> input;
      s.origin[i] = stod(input);
    }
    // read radius
    inputfile >> input;
    s.radius = stod(input);
    // store it as an object
    object o;
    o.type = "sphere";
    o.sph = s;
    objects[obj_count++] = o;
  } else if (type == "cylinder") {
    cylinder c;
    c.name = std::to_string(obj_count);
    // use the color value from the file
    if (mtlcolor != nullptr) {
      for (int i = 0; i < 12; i++) c.color[i] = mtlcolor[i];
    }
    // texture mapping
    if (texture_mapping) {
      c.texture_mapping = true;
      c.texture = texture;
    }
    // normal mapping
    if (normal_mapping) {
      c.normal_mapping = true;
      c.normal_map = normal_map;
    }
    // read origin
    for (int i = 0; i < 3; i++) {
      inputfile >> input;
      c.origin[i] = stod(input);
    }
    // read dir vector
    for (int i = 0; i < 3; i++) {
      inputfile >> input;
      c.dir[i] = stod(input);
    }
    // read radius
    inputfile >> input;
    c.radius = stod(input);
    // read length
    inputfile >> input;
    c.length = stod(input);
    // store it as an object
    object o;
    o.type = "cylinder";
    o.cyl = c;
    objects[obj_count++] = o;
  }
  // plane nx ny nz px py pz dir_x dir_y w h
  else if (type == "plane") {
    plane p;
    p.name = std::to_string(obj_count);
    // use the color value from the file
    if (mtlcolor != nullptr) {
      for (int i = 0; i < 12; i++) p.color[i] = mtlcolor[i];
    }
    // texture mapping
    if (texture_mapping) {
      p.texture_mapping = true;
      p.texture = texture;
    }
    // normal mapping
    if (normal_mapping) {
      p.normal_mapping = true;
      p.normal_map = normal_map;
    }
    // read normal vector
    for (int i = 0; i < 3; i++) {
      inputfile >> input;
      p.n[i] = stod(input);
    }
    // read a point
    for (int i = 0; i < 3; i++) {
      inputfile >> input;
      p.p0[i] = stod(input);
    }
    // read direction
    for (int i = 0; i < 2; i++) {
      inputfile >> input;
      p.dir[i] = stod(input);
    }
    // read width and height
    inputfile >> input;
    p.w = stod(input);
    inputfile >> input;
    p.h = stod(input);
    // store it as an object
    object o;
    o.type = "plane";
    o.pln = p;
    objects[obj_count++] = o;
  } else
    // if keyword is not a recognizable word
    throw std::invalid_argument("invalid object");
}

void FileStream::readLight(int i) {
  light l;
  // if it's a plane light without att values
  // automatically radius is set to be 1
  if (i == 9) {
    for (int i = 0; i < LIGHT_ARR_SIZE + 1; i++) {
      if (i == MAKR_DOWN_INDEX)  // 3
      {
        inputfile >> input;
        l.s = std::stod(input);
      } else if (i < MAKR_DOWN_INDEX)  // position 0 1 2
      {
        inputfile >> input;
        l.arr[i] = std::stod(input);
      } else if (i < 7)  // color values 4 5 6
      {
        inputfile >> input;
        l.arr[i - 1] = std::stod(input);
      } else  // set att 7 8 9 to be 0.0
        l.arr[i - 1] = 0.0;
    }
  }
  // if light has att values or it's highlight
  // manually radius can be set
  else {  // 9 + 1
    for (int i = 0; i < LIGHT_ARR_SIZE + 1; i++) {
      inputfile >> input;
      if (i == MAKR_DOWN_INDEX)  // 3
        l.s = std::stod(input);
      else if (i < MAKR_DOWN_INDEX)  // position 0 1 2
        l.arr[i] = std::stod(input);
      else  // color values 4 5 6, att 7 8 9
        l.arr[i - 1] = std::stod(input);
    }
    // if point light, set radius
    if (l.s == 1) {
      inputfile >> input;
      l.radius = std::stod(input);
    }
    // if highlight, set highlight angle
    if (l.s == 2) {
      inputfile >> input;
      l.angle = std::stod(input);
    }
  }

  lights[light_count++] = l;
}

void FileStream::readVertex() {
  vertex v;
  inputfile >> input;
  double x = std::stod(input);
  v.x = x;
  inputfile >> input;
  double y = std::stod(input);
  v.y = y;
  inputfile >> input;
  double z = std::stod(input);
  v.z = z;

  // put in the vector
  vs.push_back(v);
}

void FileStream::readVertexTextureCoordinate() {
  vertex_text_coor vt;
  inputfile >> input;
  double u = std::stod(input);
  vt.u = u;
  inputfile >> input;
  double v = std::stod(input);
  vt.v = v;

  // put in the vector
  vts.push_back(vt);
}

void FileStream::readVertexNormal() {
  vertex_normal vn;
  inputfile >> input;
  double x = std::stod(input);
  vn.x = x;
  inputfile >> input;
  double y = std::stod(input);
  vn.y = y;
  inputfile >> input;
  double z = std::stod(input);
  vn.z = z;

  vns.push_back(vn);
}

void FileStream::readFace() {
  const int MAXSIZE = 1024;
  char line[MAXSIZE];
  inputfile.getline(line, MAXSIZE);
  face f;
  int v0, vt0, vn0, v1, vt1, vn1, v2, vt2, vn2;
  if (sscanf(line, "%d/%d/%d %d/%d/%d %d/%d/%d", &v0, &vt0, &vn0, &v1, &vt1,
             &vn1, &v2, &vt2, &vn2) == 9) {
    f.texture_mapping = true;
    f.normal_mapping = true;
    f.smooth_shading = true;

    f.v0 = vs.at(v0 - 1);
    f.vt0 = vts.at(vt0 - 1);
    f.vn0 = vns.at(vn0 - 1);

    f.v1 = vs.at(v1 - 1);
    f.vt1 = vts.at(vt1 - 1);
    f.vn1 = vns.at(vn1 - 1);

    f.v2 = vs.at(v2 - 1);
    f.vt2 = vts.at(vt2 - 1);
    f.vn2 = vns.at(vn2 - 1);
  } else if (sscanf(line, "%d//%d %d//%d %d//%d", &v0, &vn0, &v1, &vn1, &v2,
                    &vn2) == 6) {
    f.texture_mapping = false;
    f.normal_mapping = false;
    f.smooth_shading = true;

    f.v0 = vs.at(v0 - 1);
    f.vn0 = vns.at(vn0 - 1);

    f.v1 = vs.at(v1 - 1);
    f.vn1 = vns.at(vn1 - 1);

    f.v2 = vs.at(v2 - 1);
    f.vn2 = vns.at(vn2 - 1);
  } else if (sscanf(line, "%d/%d %d/%d %d/%d", &v0, &vt0, &v1, &vt1, &v2,
                    &vt2) == 6) {
    f.texture_mapping = true;
    f.normal_mapping = true;
    f.smooth_shading = false;

    f.v0 = vs.at(v0 - 1);
    f.vt0 = vts.at(vt0 - 1);

    f.v1 = vs.at(v1 - 1);
    f.vt1 = vts.at(vt1 - 1);

    f.v2 = vs.at(v2 - 1);
    f.vt2 = vts.at(vt2 - 1);
  } else if (sscanf(line, "%d %d %d", &v0, &v1, &v2) == 3) {
    f.texture_mapping = false;
    f.normal_mapping = false;
    f.smooth_shading = false;

    f.v0 = vs.at(v0 - 1);

    f.v1 = vs.at(v1 - 1);

    f.v2 = vs.at(v2 - 1);
  } else
    std::invalid_argument("invalid face");

  fs.push_back(f);
}

void FileStream::readTextureMapFile() {
  inputfile >> input;
  texture_file = input;
  ImageRGB img;

  load_ppm(img, texture_file);

  textures.push_back(img);
}

void FileStream::readNormalMapFile() {
  inputfile >> input;
  normal_map_file = input;
  ImageRGB map;

  load_ppm(map, normal_map_file);

  normal_maps.push_back(map);
}

void FileStream::readBumpMapFile() {
  inputfile >> input;
  bump_map_file = input;
  ImageRGB map;

  load_ppm(map, bump_map_file);

  bump_maps.push_back(map);
}

void FileStream::load_ppm(ImageRGB &img, const std::string &name) {
  std::ifstream f(name.c_str(), std::ios::binary);
  if (f.fail()) {
    std::cout << "Can't open file: " << name << std::endl;
    return;
  }

  // get type of file
  eat_comment(f);
  int mode = 0;
  std::string s;
  f >> s;
  if (s == "P3")
    mode = 3;
  else if (s == "P6")
    mode = 6;

  // get w
  eat_comment(f);
  f >> img.w;

  // get h
  eat_comment(f);
  f >> img.h;

  // get bits
  eat_comment(f);
  int bits = 0;
  f >> bits;

  // error checking
  if (mode != 3 && mode != 6) {
    std::cout << "Unsupported mode number" << std::endl;
    f.close();
    return;
  }
  if (img.w < 1) {
    std::cout << "Unsupported width: " << img.w << std::endl;
    f.close();
    return;
  }
  if (img.h < 1) {
    std::cout << "Unsupported height: " << img.h << std::endl;
    f.close();
    return;
  }
  if (bits < 0 || bits > 255) {
    std::cout << "Unsupported number of bits: " << bits << std::endl;
    f.close();
    return;
  }

  // load image data
  img.data.resize(img.w * img.h);

  if (mode == 6) {
    f.get();
    f.read((char *)&img.data[0], img.data.size() * 3);
  } else if (mode == 3) {
    for (int i = 0; i < img.data.size(); i++) {
      int v;
      f >> v;
      img.data[i].r = v;
      f >> v;
      img.data[i].g = v;
      f >> v;
      img.data[i].b = v;
    }
  }

  // close file
  f.close();
}

void FileStream::eat_comment(std::ifstream &f) {
  char linebuf[1024];
  char ppp;
  while (ppp = f.peek(), ppp == '\n' || ppp == '\r') f.get();
  if (ppp == '#') f.getline(linebuf, 1023);
}