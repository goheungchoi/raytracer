## File Format
    **color values should be between 0 and 1**

    eye x y z
    viewdir dir_x dir_y dir_z
    updir dir_x dir_y dir_z
    vfov v_degree
    imsize image_width image_height
    depthcueing r g b a_max a_min dist_max dist_min (0 <= a_max, a_min <= 1)
    bkgcolor r g b

    mtlcolor Odr Odg Odb Osr Osg Osb ka kd ks n opacity index_of_refraction
    texture {directory}
    normal {directory}
    sphere x y z radius
    plane nx ny nz px py pz dir_x dir_y w h 

    # light_type 
        0 -> directional light - automatically this program doesn't store attenuation values
        1 -> point light - attenuation values& light source radius
        2 -> highlight - dir vector instead of attenuation values& highlight angle
    (# ) default attenuation values c1 = 0.0, c2 = 0.0, c3 = 0.0
    (# ) default angle = 0
    (# ) default radius = 1.0
    light x y z light_type r g b
    ex)
        attlight dir_x dir_y dir_z 0 r g b N/A N/A N/A N/A
        attlight x y z 1 r g b c1 c2 c3 radius
        attlight x y z 2 dir_v_x dir_v_y dir_v_z c1 c2 c3 angle

    **how to create triangle mesh**
    (# ) triangles must be stored in a triangle mesh
    (# ) meshes contains data about texturing and normal mapping etc..
    (# ) 0 <= u, v <= 1
    (# ) index starts from 1

        v vx vy vz
        vt u v
        vn nx ny nz

        f v_index/(vt_index)/(vn_index) v_index/(vt_index)/(vn_index) v_index/(vt_index)/(vn_index)

        mesh num_of_face face_index face_index face_index ...

    (# ) example:
        v ...
        v ...
        v ...
        v ...

        f 0 1 2
        f 3 2 1

        mtlcolor ...
        texture ...
        normal ...
        mesh 2 0 1

## Run This program
    - Linux :
        -Type following command lines:
            > $ make
            > $ ./raytracer {file_dir}
    - Windows :
        -Install C/C++ extension from Microsoft
        -Go to "Run" - "Run without debugging"
        -Type following command lines on Console:
            > raytracer {file_dir}

## Features
    - **shapes** folder contains multiple files that requires for representing 3D figures and calculating interactions between objects.
    The structure looks like this:
    
    object.h <------- line.h
    |                   |
    cylinder.h        vector.h
    sphere.h            |
    plane.h           point.h
    triangle_mesh.h
        ↑
    triangle.h
    
    - **streams** folder contains a C++ file that creats I/O streams for reading a .txt file and writing a .ppm file.
        - Once the object, file-stream, is created, input and output stream will not be terminated until the program is exited or the user destructed the object manually.
        - This file has a function in order to bulletproof. What it means is that it determines whether input is valid and invalid. If data is not valid format, it terminates the program and raises an appropriate error

    - **scene.cpp** contains every necessary information for creating an image. This is the actual backbone of the program. It contains objects information, the position of the camera, view direction, and etc.

    - **ray-casting.cpp** works like a main function. It puts everything together and creates a appropriate .ppm image file.

    - **anti aliasing**: anti aliasing is hard-coded.

## A1-c Added Feture
    1. Generating a Square
        - Plane class generates a square shape centered at O and extended along with the direction vector.
        - The direction vector is a vector on the plane that obtains x and y value. ex) dir(1, 0), w = 10, h = 5 --> a rectangular extending along x axis.

    2. Triangle Mesh
        - This program can generate triangle meshes.
        - Triangle object
            - 3 vertices: p0, p1, p2.
            - 2 edge vectors: e1 = vec3D(p1 - p0), e2 = vec3D(p2 - p0);
            - 3 direction vectors: T, B, N
            - 3 texture coordinate: c0, c1, c2
            - 3 vertex normal vectors: vn0, vn1, v2;
        - Every triangle object must be contained by a Triangle_Mesh object in order to be used.

    3. Texture Mapping
        - Texture mapping function is implemented.
        - Texture maps must be ppm file format

    4. Smooth Shading
        - Smooth shading becomes enabled automatically when vectex normals are inputted.
        
    5. Normal Mapping
        - Normal Mapping is available.
        - Normal maps must be ppm file format
        - Each blue value must be greater than 127 and smaller than 256
        
    6. Reflection & Refraction
        - Intensity of reflection is calculated via the intensity of reflected ray multiplied by ks value of an object. In other words, the value of ks determines whether there is reflectin on an object or not.
        - Refraction is calculated via Fresnal reflection.
        - This program uses the total distance that a ray travels rather than recursion depth in order to terminate a loop 

