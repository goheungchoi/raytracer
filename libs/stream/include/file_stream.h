#ifndef _FILE_STREAM_H_
#define _FILE_STREAM_H_

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <stdlib.h>
#include <vector>
#include <unordered_map>
#include <memory>

//ppm file reading helpers
struct RGB
{
    unsigned char r, g, b;
};

struct ImageRGB
{
    int w, h;
    std::vector<RGB> data;
};

const int NUM_OF_KEYWORDS = 22;
//keywords indecise
const int EYE_INDEX = 0;              
const int VIEW_DIR_INDEX = 1;     
const int UP_DIR_INDEX = 2;        
const int VFOV_INDEX = 3;          
const int IMSIZE_INDEX = 4;      
const int BKG_COLOR_INDEX = 5;   
const int MTLCOLOR_INDEX = 6;    
const int SPHERE_INDEX = 7;        
const int CYLINDER_INDEX = 8;    
const int LIGTH_INDEX = 9;          
const int ATTLIGTH_INDEX = 10;    
const int HIGHLIGHT_INDEX = 11;  
const int PLANE_INDEX = 12;
const int DEPTH_CUEING_INDEX = 13;
const int VERTEX_INDEX = 14; 
const int VERTEX_TEXTURE_COOR_INDEX = 15; 
const int VERTEX_NORMAL_INDEX = 16; 
const int FACE_INDEX = 17; 
const int TEXTURE_INDEX = 18; 
const int NORMAL_INDEX = 19; 
const int BUMP_INDEX = 20;
const int MESH_INDEX = 21;

struct object {
    std::string type = "";
    bool texture_mapping = false;
    bool normal_mapping = false;
    ImageRGB texture;
    ImageRGB normal_map;
    double color[12];
};

struct sphere : public object {
    double origin[3];
    double radius;
};

struct cylinder : public object {
    double origin[3];
    double dir[3];
    double radius;
    double length;
};

struct plane : public object {
    double n[3];
    double dir[2];
    double p0[3];
    double w, h;
};

struct vertex {
    double x;
    double y;
    double z;
};

struct vertex_text_coor {
    double u = -1;
    double v = -1;
};

struct vertex_normal {
    double x = 0;
    double y = 0;
    double z = 0;
};

struct face {
    bool texture_mapping = false;
    bool normal_mapping = false;
    bool smooth_shading = false;
    vertex v0;
    vertex_text_coor vt0;
    vertex_normal vn0;
    vertex v1;
    vertex_text_coor vt1;
    vertex_normal vn1;
    vertex v2;
    vertex_text_coor vt2;
    vertex_normal vn2;
};

struct triangle_mesh : public object {
    std::vector<face> triangles;
};

//light array size;
const int LIGHT_ARR_SIZE = 9;
const int MAKR_DOWN_INDEX = 3;
                    //s is for marking down the type of light
                    //s = 0 -> directional light
                    //s = 1 -> point light
                    //s = 2 -> highlight
                    // if highlight -> array stores dir vector 
                    //                 instead of attenuation values c
struct light { 
    int s = -1; 
    double arr[LIGHT_ARR_SIZE];
    double angle = 0; /*highlight angle*/
    double radius = 0;
};

class FileStream {
public:
    std::string filename;
    std::string outputfile;
    std::string input;
    std::ifstream inputfile;
    std::ofstream output_stream;
    //array lists
    double eye[3];
    double view_dir[3];
    double up_dir[3];
    double *vfov = new double;
    double imsize[2];
    double bkg_color[3];
    double mtlcolor[12];
    std::vector<double*> mtlcolors;
    bool depthcueing_on = false;
    double depthcueing[7];
    //object array;
    std::vector<std::unique_ptr<object>> objects;
    int get_obj_count = 0;  //how many elems are exported
    //light array;
    std::vector<std::unique_ptr<light>> lights;
    int get_light_count = 0;//how many elems are exported
    //vertex vector;
    std::vector<vertex> vs;
    std::vector<vertex_text_coor> vts;
    std::vector<vertex_normal> vns;
    std::vector<face> fs;
    //std::vector<triangle_mesh> meshes;
    //file directory;
    std::string texture_file;
    std::vector<ImageRGB> textures;
    std::string normal_map_file;
    std::vector<ImageRGB> normal_maps;
    std::string bump_map_file;
    std::vector<ImageRGB> bump_maps;

    //for initialize fixed size double arrays
    double *lists[NUM_OF_KEYWORDS];

    FileStream();
    FileStream(std::string filename);
    ~FileStream();
    void readFile();
    
    void initializeOutputFile();
    void writeFile(std::string str);
    object* getObject(); //return an object one by one
    light* getLight();   //return a light one by one
private:   
    const std::string EYE = "eye";              //0
    const std::string VIEW_DIR = "viewdir";     //1
    const std::string UP_DIR = "updir";         //2
    const std::string VFOV = "vfov";            //3
    const std::string IMSIZE = "imsize";        //4
    const std::string BKG_COLOR = "bkgcolor";   //5
    const std::string MTLCOLOR = "mtlcolor";    //6
    const std::string SPHERE = "sphere";        //7
    const std::string CYLINDER = "cylinder";    //8
    const std::string LIGTH = "light";          //9
    const std::string ATTLIGTH = "attlight";    //10
    const std::string HIGHLIGHT = "highlight";  //11
    const std::string PLANE = "plane";          //12
    const std::string DEPTH_CUEING = "depthcueing"; //13
    const std::string VERTEX = "v"; //14
    const std::string VERTEX_TEXTURE_COOR = "vt"; //15
    const std::string VERTEX_NORMAL = "vn"; //16
    const std::string FACE = "f"; //17
    const std::string TEXTURE = "texture"; //18
    const std::string NORMAL = "normal"; //19
    const std::string BUMP = "bump"; //20
    const std::string MESH = "mesh"; //21
    //contain keywords in a string array
    std::unordered_map<std::string, int> KEYWORDS;


    void readTriangleMesh();
    void readTuple(double *arr, int size);
    void readObject(std::string type);
    void readLight(int i);
    void readVertex();
    void readVertexTextureCoordinate();
    void readVertexNormal();
    void readFace();
    void readTextureMapFile();
    void readNormalMapFile();
    void readBumpMapFile();
    
    void load_ppm(ImageRGB &img, const std::string &name);
    void eat_comment(std::ifstream &f);
};

#endif  // _FILE_STREAM_H_
