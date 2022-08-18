all: obj stream scene raytracer

obj: build
	cd libs/obj; make

stream: build
	cd libs/stream; make

scene: build obj stream
	cd libs/scene; make

raytracer: build obj stream scene
	cd apps/raytracer; make

build:
	mkdir -p build

clean:
	cd apps/raytracer; make clean
	rm -rf build
