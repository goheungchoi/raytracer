raytracer: ray-casting.o scene.o file-stream.o plane.o light.o vector3D.o sphere.o point.o object.o line.o cylinder.o triangle.o triangle_mesh.o
	g++ ray-casting.o scene.o file-stream.o plane.o light.o vector3D.o sphere.o point.o object.o line.o cylinder.o triangle.o triangle_mesh.o -o raytracer

ray-casting.o: ray-casting.cpp ray-casting.h
	g++ -std=c++11 -c ray-casting.cpp -o ray-casting.o

scene.o: scene.cpp scene.h
	g++ -std=c++11 -c scene.cpp -o scene.o

file-stream.o: streams/file-stream.cpp streams/file-stream.h
	g++ -std=c++11 -c streams/file-stream.cpp -o file-stream.o

vector3D.o: shapes/vector3D.cpp shapes/vector3D.h
	g++ -std=c++11 -c shapes/vector3D.cpp -o vector3D.o

sphere.o: shapes/sphere.cpp shapes/sphere.h
	g++ -std=c++11 -c shapes/sphere.cpp -o sphere.o
	
point.o: shapes/point.cpp shapes/point.h
	g++ -std=c++11 -c shapes/point.cpp -o point.o

object.o: shapes/object.cpp shapes/object.h
	g++ -std=c++11 -c shapes/object.cpp -o object.o

line.o: shapes/line.cpp shapes/line.h
	g++ -std=c++11 -c shapes/line.cpp -o line.o

cylinder.o: shapes/cylinder.cpp shapes/cylinder.h
	g++ -std=c++11 -c shapes/cylinder.cpp -o cylinder.o

light.o: shapes/light.cpp shapes/light.h
	g++ -std=c++11 -c shapes/light.cpp -o light.o

plane.o: shapes/plane.cpp shapes/plane.h
	g++ -std=c++11 -c shapes/plane.cpp -o plane.o

triangle.o: shapes/triangle.cpp shapes/triangle.h
	g++ -std=c++11 -c shapes/triangle.cpp -o triangle.o

triangle_mesh.o: shapes/triangle_mesh.cpp shapes/triangle_mesh.h
	g++ -std=c++11 -c shapes/triangle_mesh.cpp -o triangle_mesh.o

.PHONY: clean run

clean:
	rm *.o raytracer

run:
	./raytracer
