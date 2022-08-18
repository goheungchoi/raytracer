all: obj scene stream raytracer

obj: build
	cd libs/obj; make

scene: build obj
	cd libs/scene; make

stream: build obj scene
	cd libs/; make

raytracer: build obj scene stream
	cd apps/raytracer; make

build:
	mkdir -p build

clean:
	cd apps/raytracer; make clean
	rm -rf build
