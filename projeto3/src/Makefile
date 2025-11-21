TARGET = t
BUILD  = ../build

INC = -I.
LIB = -L.

CPP = g++
#CPPFLAGS = -O2 -Wall -std=c++17 -DGL_SILENCE_DEPRECATION 
CPPFLAGS = -g -Wall -std=c++17 -DGL_SILENCE_DEPRECATION 

SRC = \
  arcball.cpp \
  camera2d.cpp \
  camera3d.cpp \
  color.cpp \
  cube.cpp \
  error.cpp \
  framebuffer.cpp \
  grid.cpp \
  image.cpp \
  light.cpp \
  material.cpp \
  mesh.cpp \
  node.cpp \
  instanced_points.cpp \
  objmesh.cpp \
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
  main_3d.cpp \
  luxor/animation.cpp \
  luxor/cubicinterpolator.cpp \
  luxor/linearinterpolator.cpp \
  luxor/luxor.cpp \
  luxor/luxorengine.cpp \
  luxor/movement.cpp \

OBJ = $(addprefix $(BUILD)/, $(addsuffix .o, $(basename $(SRC))))

$(BUILD)/%.o: %.cpp Makefile | prep
	@mkdir -p $(dir $@)
	$(CPP) $(INC) $(CPPFLAGS) -c $< -o $@

$(BUILD)/$(TARGET): $(OBJ) Makefile
	$(CPP) $(LIB) -o $@ $(OBJ) -lstdc++ -lGL -lGLEW -lglfw -lassimp -ldl -lpthread -lX11 -lXrandr -lXi

all: prep $(BUILD)/$(TARGET)

run: all
	$(BUILD)/$(TARGET)

prep:
	mkdir -p $(BUILD)

clean:
	rm -rf $(BUILD)


