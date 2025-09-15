TARGET = build/t

all: $(TARGET)

INC = -I.
LIB = -L.

CPP = g++
CPPFLAGS = -g -Wall -std=c++17 -DGL_SILENCE_DEPRECATION 

SRC = \
  src/error.cpp \
  src/shader.cpp \
  src/polygon.cpp \
  src/triangle.cpp \
  src/main.cpp

OBJ = $(patsubst src/%.cpp,build/%.o,$(SRC))

build/%.o: src/%.cpp | build
	$(CPP) $(INC) $(CPPFLAGS) -c $< -o $@

build:
	mkdir -p build

$(TARGET): $(OBJ) Makefile 
	$(CPP) $(LIB) -o $@ $(OBJ) -lstdc++ -lGL -lGLEW -lglfw -lassimp -ldl -lpthread -lX11 -lXrandr -lXi

clean:
	rm -rf build
