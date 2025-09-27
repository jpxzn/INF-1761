
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
#include "quad.h"
#include "triangle.h"
#include "disk.h"

#include <iostream>

static ScenePtr scene;
static CameraPtr camera;

class MovePointer;
using MovePointerPtr = std::shared_ptr<MovePointer>;
class MovePointer : public Engine 
{
  TransformPtr m_trf;
protected:
  MovePointer (TransformPtr trf) 
  : m_trf(trf) 
  {
  }
public:
  static MovePointerPtr Make (TransformPtr trf)
  {
    return MovePointerPtr(new MovePointer(trf));
  }
  virtual void Update (float dt)
  {
    m_trf->Rotate(-dt/30.0f*180.0f,0,0,1);
  }
};

static void initialize (void)
{
  // set background color: white 
  glClearColor(0.8f,1.0f,1.0f,1.0f);
  // enable depth test 
  glEnable(GL_DEPTH_TEST);

  // create objects
  camera = Camera2D::Make(0,10,0,10);

  // Shader for 2D colored/texture rendering
  auto shader = Shader::Make();
  shader->AttachVertexShader("../shaders/2d/vertex.glsl");
  shader->AttachFragmentShader("../shaders/2d/fragment.glsl");
  shader->Link();

  // Solar System (top view):
  // Sun
  auto trfSun = Transform::Make();
  trfSun->Translate(5.0f,5.0f,0.0f);
  auto trfSunScale = Transform::Make();
  trfSunScale->Scale(1.5f,1.5f,1.0f);
  auto sunGeom = Node::Make(trfSunScale, {Color::Make(1.0f,0.9f,0.2f)},{Disk::Make(64)});
  auto sun = Node::Make(trfSun,{sunGeom});

  // Earth orbit
  auto trfEarthOrbit = Transform::Make();
  auto earthOrbit = Node::Make(trfEarthOrbit, std::initializer_list<NodePtr>{});
  // Earth translation to orbital radius
  auto trfEarthTranslate = Transform::Make();
  trfEarthTranslate->Translate(3.0f,0.0f,0.0f);
  auto earthTranslate = Node::Make(trfEarthTranslate, std::initializer_list<NodePtr>{});
  // Earth self spin
  auto trfEarthSpin = Transform::Make();
  auto earthSpin = Node::Make(trfEarthSpin, std::initializer_list<NodePtr>{});
  // Earth geometry & scale
  auto trfEarthGeom = Transform::Make();
  trfEarthGeom->Scale(0.6f,0.6f,1.0f);
  auto earthGeom = Node::Make(trfEarthGeom, {Color::Make(0.2f,0.4f,1.0f)},{Disk::Make(48)});
  earthSpin->AddNode(earthGeom);
  earthTranslate->AddNode(earthSpin);
  earthOrbit->AddNode(earthTranslate);
  sun->AddNode(earthOrbit);

  // Moon orbit
  auto trfMoonOrbit = Transform::Make();
  auto moonOrbit = Node::Make(trfMoonOrbit, std::initializer_list<NodePtr>{});
  auto trfMoon = Transform::Make();
  trfMoon->Translate(1.0f,0.0f,0.0f);
  trfMoon->Scale(0.25f,0.25f,1.0f);
  auto moon = Node::Make(trfMoon,{Color::Make(0.8f,0.8f,0.8f)},{Disk::Make(36)});
  moonOrbit->AddNode(moon);
  earthTranslate->AddNode(moonOrbit);

  // Root with shader managing whole scene
  auto root = Node::Make(shader, std::initializer_list<NodePtr>{sun});
  scene = Scene::Make(root);

  // Engine to animate orbits and self-rotation
  class SolarEngine : public Engine {
    TransformPtr m_earthOrbit, m_earthSpin, m_moonOrbit;
  public:
    SolarEngine(TransformPtr eo, TransformPtr es, TransformPtr mo)
      : m_earthOrbit(eo), m_earthSpin(es), m_moonOrbit(mo) {}
    void Update(float dt) override {
      m_earthOrbit->Rotate(20.0f * dt, 0,0,1);
      m_earthSpin->Rotate(90.0f * dt, 0,0,1);
      m_moonOrbit->Rotate(120.0f * dt, 0,0,1);
    }
  };

  scene->AddEngine(std::make_shared<SolarEngine>(trfEarthOrbit, trfEarthSpin, trfMoonOrbit));
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

