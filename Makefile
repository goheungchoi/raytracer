all: obj scene stream raytracer

obj: build
	cd libs/obj; make

stream: build
	cd libs/stream; make

scene: build obj stream
	cd libs/scene; make

raytracer: build obj scene stream
	cd apps/raytracer; make

build:
	mkdir -p build

clean:
	cd apps/raytracer; make clean
	rm -rf build
