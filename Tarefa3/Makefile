TARGET = build/solar

all: $(TARGET)

INC = -I.
LIB = -L.

CPP = g++
CPPFLAGS = -g -Wall -std=c++17 -DGL_SILENCE_DEPRECATION 

SRC = \
src/arcball.cpp \
src/camera2d.cpp \
src/camera3d.cpp \
src/color.cpp \
src/cube.cpp \
src/error.cpp \
src/framebuffer.cpp \
src/grid.cpp \
src/image.cpp \
src/light.cpp \
src/material.cpp \
src/mesh.cpp \
src/node.cpp \
src/polyoffset.cpp \
src/scene.cpp \
src/shader.cpp \
src/sphere.cpp \
src/state.cpp \
src/texbuffer.cpp \
src/texcube.cpp \
src/texdepth.cpp \
src/texture.cpp \
src/transform.cpp \
src/orbitEngine.cpp \
src/disk.cpp \
src/main_2d.cpp \

OBJ = $(patsubst src/%.cpp,build/%.o,$(SRC))

build/%.o: src/%.cpp | build
	$(CPP) $(INC) $(CPPFLAGS) -c $< -o $@

build:
	mkdir -p build

$(TARGET): $(OBJ) Makefile 
	$(CPP) $(LIB) -o $@ $(OBJ) -lstdc++ -lGL -lGLEW -lglfw -ldl -lpthread -lX11 -lXrandr

clean:
	rm -rf build

run: $(TARGET)
	./$(TARGET)



