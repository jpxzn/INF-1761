
#ifdef _WIN32
#define GLAD_GL_IMPLEMENTATION // Necessary for headeronly version.
#include <glad/gl.h>
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
#include "orbitEngine.h"

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
  glClearColor(0.05f,0.05f,0.07f,1.0f);
  glEnable(GL_DEPTH_TEST);

  camera = Camera2D::Make(0,10,0,10);

  auto shader = Shader::Make();
  shader->AttachVertexShader("shaders/vertex.glsl");
  shader->AttachFragmentShader("shaders/fragment.glsl");
  shader->Link();

  auto solTrf = Transform::Make();
  solTrf->Translate(5.0f, 5.0f, 0.0f);
  solTrf->Scale(0.6f, 0.6f, 1.0f);
  auto solNode = Node::Make(solTrf, {Color::Make(1.0f, 0.9f, 0.2f)}, {Disk::Make(64)});

  auto terraOrbitPivot = Transform::Make();
  terraOrbitPivot->Translate(5.0f, 5.0f, 0.0f);
  auto terraOrbitNode = Node::Make(terraOrbitPivot,std::initializer_list<std::shared_ptr<Appearance>>{}, std::initializer_list<std::shared_ptr<Node>>{});

  auto terraTranslate = Transform::Make();
  terraTranslate->Translate(3.0f, 0.0f, 0.0f);
  auto terraScale = Transform::Make();
  terraScale->Scale(0.25f, 0.25f, 1.0f);
  auto terraGeom = Node::Make(terraScale, {Color::Make(0.2f, 0.6f, 1.0f)}, {Disk::Make(64)});
  auto terraNode = Node::Make(terraTranslate, {terraGeom});

  auto luaOrbitPivot = Transform::Make();
  auto luaOrbitNode = Node::Make(luaOrbitPivot, std::initializer_list<std::shared_ptr<Appearance>>{}, std::initializer_list<std::shared_ptr<Node>>{});
  auto luaTranslate = Transform::Make();
  luaTranslate->Translate(0.7f, 0.0f, 0.0f);
  auto luaScale = Transform::Make();
  luaScale->Scale(0.08f, 0.08f, 1.0f);
  auto luaGeom = Node::Make(luaScale, {Color::Make(0.8f, 0.8f, 0.8f)}, {Disk::Make(48)});
  auto luaNode = Node::Make(luaTranslate, {luaGeom});

  luaOrbitNode->AddNode(luaNode);
  terraNode->AddNode(luaOrbitNode);
  terraOrbitNode->AddNode(terraNode);

  auto root = Node::Make(shader, std::initializer_list<std::shared_ptr<Appearance>>{}, std::initializer_list<std::shared_ptr<Node>>{solNode, terraOrbitNode});
  scene = Scene::Make(root);

  scene->AddEngine(OrbitEngine::Make(terraOrbitPivot, +30.0f));
  scene->AddEngine(OrbitEngine::Make(luaOrbitPivot, +120.0f));
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

    GLFWwindow* win = glfwCreateWindow(600, 400, "Window title", nullptr, nullptr);
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

