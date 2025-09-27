
TARGET = t

.PHONY: all clean run

all: $(TARGET)

UNAME_S := $(shell uname -s)

# Defaults
CXX ?= g++
CXXFLAGS ?= -g -Wall -std=c++17 -DGL_SILENCE_DEPRECATION
INC := -I. -Ithird_party -Ithird_party/stb
LIB := -L.

# macOS settings (keep original)
ifeq ($(UNAME_S),Darwin)
INC += -I/Users/celes/Work/include -I/opt/homebrew/include
LIB += -L/opt/homebrew/lib
LDLIBS := -lstdc++ -framework OpenGL -lglfw -lassimp
endif

# Linux/WSL settings
ifeq ($(UNAME_S),Linux)
  # Requires: sudo apt install build-essential pkg-config libglfw3-dev libassimp-dev libglm-dev libglew-dev
  INC += $(shell pkg-config --cflags glfw3 assimp 2>/dev/null)
  LDLIBS := $(shell pkg-config --libs glfw3 assimp 2>/dev/null) -lGLEW -lGL -ldl -lpthread
endif

SRC = \
  arcball.cpp \
  camera2d.cpp \
  camera3d.cpp \
  color.cpp \
  cube.cpp \
  error.cpp \
  framebuffer.cpp \
  grid.cpp \
  disk.cpp \
  image.cpp \
  light.cpp \
  material.cpp \
  mesh.cpp \
  node.cpp \
  quad.cpp \
  polyoffset.cpp \
  scene.cpp \
  shader.cpp \
  sphere.cpp \
  state.cpp \
  texbuffer.cpp \
  texcube.cpp \
  texdepth.cpp \
  texture.cpp \
  transform.cpp \
  triangle.cpp \
  main_2d.cpp \

OBJ = $(addsuffix .o, $(basename $(SRC))) 

.cpp.o:
	$(CXX) $(INC) $(CXXFLAGS) -c $< -o $@ 

$(TARGET): $(OBJ) Makefile 
	$(CXX) $(LIB) -o $@ $(OBJ) $(LDLIBS)


# Convenience target to build and run the demo
run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) $(OBJ)


