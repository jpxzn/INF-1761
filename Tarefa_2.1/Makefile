
TARGET = build/simple_scene

.PHONY: all run clean build build-all help

all: $(TARGET)

UNAME_S := $(shell uname -s)

# Defaults
CXX ?= g++
CXXFLAGS ?= -g -Wall -std=c++17 -DGL_SILENCE_DEPRECATION
INC := -I. -Ithird_party -Ithird_party/stb
LIB := -L.

# Windows settings
ifeq ($(OS),Windows_NT)
  # Adjust these paths to where you installed GLFW and GLM
  # Example if using vcpkg: set GLFW_DIR to the package lib/include location
  GLFW_DIR ?= C:/libs/glfw
  GLM_DIR ?= C:/libs/glm
  INC += -I$(GLFW_DIR)/include -I$(GLM_DIR)/include
  LIB += -L$(GLFW_DIR)/lib
  # Choose the appropriate import library name: glfw3 for static, glfw3dll for DLL import
  GLFW_LIB ?= glfw3
  LDLIBS := -l$(GLFW_LIB) -lopengl32 -lgdi32
endif

# Linux/WSL settings
ifeq ($(UNAME_S),Linux)
  # Requires: sudo apt install build-essential pkg-config libglfw3-dev libassimp-dev libglm-dev libglew-dev
  INC += $(shell pkg-config --cflags glfw3 2>/dev/null)
  LDLIBS := $(shell pkg-config --libs glfw3 2>/dev/null) -lGLEW -lGL -ldl -lpthread
endif

SRC = \
  arcball.cpp \
  camera3d.cpp \
  color.cpp \
  cube.cpp \
  error.cpp \
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
  grid.cpp \
  texture.cpp \
  transform.cpp \
  main_3d.cpp \

OBJ = $(patsubst %.cpp,build/%.o,$(SRC))

# Build objects into build/
build/%.o: %.cpp | build_dir
	$(CXX) $(INC) $(CXXFLAGS) -c $< -o $@ 

# Create build directory (separate target to avoid name clash with 'build' alias)
build_dir:
	mkdir -p build

$(TARGET): $(OBJ) Makefile 
	$(CXX) $(LIB) -o $@ $(OBJ) $(LDLIBS)

# Convenience target to build and run the demo
run: $(TARGET)
	./$(TARGET)

# Alias targets to build the app
build: $(TARGET)
build-all: $(TARGET)

# Show available targets
help:
	@echo Available targets:
	@echo   make\t\tBuild default target ($(TARGET))
	@echo   make run\tBuild and run
	@echo   make clean\tRemove build artifacts
	@echo   make build\tBuild the application (alias)
	@echo   make build-all\tBuild the application explicitly

clean:
	rm -rf build
