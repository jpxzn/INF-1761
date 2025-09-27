
#ifdef _WIN32
#define GLAD_GL_IMPLEMENTATION // Necessary for headeronly version.
#include <glad/gl.h>
#elif __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>

#include "scene.h"
#include "state.h"
#include "camera2d.h"
#include "color.h"
#include "transform.h"
#include "error.h"
#include "shader.h"
#include "disk.h"
#include "solar_engine.h"

#include <iostream>

static ScenePtr scene;
static CameraPtr camera;

// ===================== Configuração =====================
namespace Config {
  constexpr float EarthOrbitSpeed = 20.0f;   // deg/s
  constexpr float EarthSpinSpeed = 90.0f;   // deg/s
  constexpr float MoonOrbitSpeed = 120.0f;  // deg/s
  constexpr float SunX = 5.0f;
  constexpr float SunY = 5.0f;
  constexpr float EarthRadius = 3.0f;
  constexpr float MoonRadius = 1.0f; // distância da lua à Terra
}

struct SolarSystemNodes {
  TransformPtr earthOrbit;
  TransformPtr earthSpin;
  TransformPtr moonOrbit;
};

// Cria shader 2D simples
static ShaderPtr CreateShader2D() {
  auto shader = Shader::Make();
  shader->AttachVertexShader("../shaders/2d/vertex.glsl");
  shader->AttachFragmentShader("../shaders/2d/fragment.glsl");
  shader->Link();
  return shader;
}

// Constrói a cena do sistema solar e retorna também os transforms relevantes
static std::pair<ScenePtr,SolarSystemNodes> BuildSolarSystem(ShaderPtr shader) {
  SolarSystemNodes nodes{};

  // Sun
  auto trfSun = Transform::Make();
  trfSun->Translate(Config::SunX,Config::SunY,0.0f);
  auto trfSunScale = Transform::Make();
  trfSunScale->Scale(1.5f,1.5f,1.0f);
  auto sunGeom = Node::Make(trfSunScale, {Color::Make(1.0f,0.9f,0.2f)},{Disk::Make(64)});
  auto sun = Node::Make(trfSun,{sunGeom});

  // Earth orbit root (rotation center at sun)
  nodes.earthOrbit = Transform::Make();
  auto earthOrbit = Node::Make(nodes.earthOrbit, std::initializer_list<NodePtr>{});

  // Earth translation to orbit radius
  auto trfEarthTranslate = Transform::Make();
  trfEarthTranslate->Translate(Config::EarthRadius,0.0f,0.0f);
  auto earthTranslate = Node::Make(trfEarthTranslate, std::initializer_list<NodePtr>{});

  // Earth spin
  nodes.earthSpin = Transform::Make();
  auto earthSpin = Node::Make(nodes.earthSpin, std::initializer_list<NodePtr>{});

  // Earth geometry
  auto trfEarthGeom = Transform::Make();
  trfEarthGeom->Scale(0.6f,0.6f,1.0f);
  auto earthGeom = Node::Make(trfEarthGeom, {Color::Make(0.2f,0.4f,1.0f)},{Disk::Make(48)});
  earthSpin->AddNode(earthGeom);
  earthTranslate->AddNode(earthSpin);
  earthOrbit->AddNode(earthTranslate);
  sun->AddNode(earthOrbit);

  // Moon orbit
  nodes.moonOrbit = Transform::Make();
  auto moonOrbit = Node::Make(nodes.moonOrbit, std::initializer_list<NodePtr>{});
  auto trfMoon = Transform::Make();
  trfMoon->Translate(Config::MoonRadius,0.0f,0.0f);
  trfMoon->Scale(0.25f,0.25f,1.0f);
  auto moon = Node::Make(trfMoon,{Color::Make(0.8f,0.8f,0.8f)},{Disk::Make(36)});
  moonOrbit->AddNode(moon);
  earthTranslate->AddNode(moonOrbit);

  auto root = Node::Make(shader, std::initializer_list<NodePtr>{sun});
  auto sc = Scene::Make(root);
  return {sc, nodes};
}

static void initialize () {
  glClearColor(0.8f,1.0f,1.0f,1.0f);
  glEnable(GL_DEPTH_TEST);
  camera = Camera2D::Make(0,10,0,10);
  auto shader = CreateShader2D();
  auto [sc, nodes] = BuildSolarSystem(shader);
  scene = sc;
  // Anexa engine
  scene->AddEngine(std::make_shared<SolarEngine>(nodes.earthOrbit, nodes.earthSpin, nodes.moonOrbit));
}

static void display (GLFWwindow* win)
{ 
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear window 
  Error::Check("before render");
  scene->Render(camera);
  Error::Check("after render");
}

static void error (int code, const char* msg)
{
  printf("GLFW error %d: %s\n", code, msg);
  glfwTerminate();
  exit(0);
}

static void keyboard (GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_Q && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static void resize (GLFWwindow* win, int width, int height)
{
  glViewport(0,0,width,height);
}

static void update (float dt)
{
  scene->Update(dt);
}

int main ()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);       // required for mac os
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);  // option for mac os
#endif

    glfwSetErrorCallback(error);

  GLFWwindow* win = glfwCreateWindow(600, 400, "Sistema Solar", nullptr, nullptr);
    assert(win);
    glfwSetFramebufferSizeCallback(win, resize);  // resize callback
    glfwSetKeyCallback(win, keyboard);            // keyboard callback

    glfwMakeContextCurrent(win);
#ifdef _WIN32
  if (!gladLoadGL(glfwGetProcAddress)) {
    printf("Failed to initialize GLAD OpenGL context\n");
    exit(1);
  }
#elif !defined(__APPLE__)
  if (glewInit() != GLEW_OK) {
    printf("Failed to initialize GLEW\n");
    exit(1);
  }
#endif
    printf("OpenGL version: %s\n", glGetString(GL_VERSION));

  initialize();

  float t0 = float(glfwGetTime());
  while(!glfwWindowShouldClose(win)) {
    float t = float(glfwGetTime());
    update(t-t0);
    t0 = t;
    display(win);
    glfwSwapBuffers(win);
    glfwPollEvents();
  }
  glfwTerminate();
  return 0;
}

