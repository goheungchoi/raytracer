all: stream scene raytracer

stream: build
	cd libs/stream; make

scene: build stream
	cd libs/scene; make

raytracer: build stream scene
	cd apps/raytracer; make

build:
	mkdir -p build

clean:
	cd apps/raytracer; make clean
	rm -rf build
