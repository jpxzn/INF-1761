
#ifdef _WIN32
#define GLAD_GL_IMPLEMENTATION // Necessary for headeronly version.
#include <glad/gl.h>
#elif __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#include <GL/gl.h>
#endif
#include <GLFW/glfw3.h>

#include "arcball.h"
#include "scene.h"
#include "state.h"
#include "camera3d.h"
#include "material.h"
#include "texture.h"
#include "transform.h"
#include "cube.h"
#include "quad.h"
#include "sphere.h"
#include "error.h"
#include "shader.h"
#include "light.h"
#include "light.h"
#include "polyoffset.h"
#include "luxor/luxor.h"

#include <iostream>
#include <cassert>

static float viewer_pos[3] = {2.0f, 3.5f, 2.0f};

static ScenePtr scene;
static Camera3DPtr camera;
static ArcballPtr arcball;
// global fovy state (kept here because Camera3D has no getter)
static float g_cam_fovy = 45.0f;
static LuxorPtr g_lux = nullptr;

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
  if (!camera) return;
  // scroll up (positive yoffset) -> zoom in (decrease fov)
  g_cam_fovy -= float(yoffset) * 2.5f;
  if (g_cam_fovy < 15.0f) g_cam_fovy = 15.0f;
  if (g_cam_fovy > 90.0f) g_cam_fovy = 90.0f;
  camera->SetAngle(g_cam_fovy);
}

static void initialize (void)
{
  // set background color: white 
  glClearColor(0.0f,0.0f,0.0f,0.0f);
  // enable depth test 
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);  // cull back faces

  // create minimal scene: camera, a red table base (cube) and the Luxor lamp
  camera = Camera3D::Make(viewer_pos[0],viewer_pos[1],viewer_pos[2]);
  arcball = camera->CreateArcball();
  // initialize camera fov to global state
  camera->SetAngle(g_cam_fovy);

  LightPtr light = Light::Make(0.0f,0.0f,0.0f,1.0f,"camera");

  // red material for the table base
  AppearancePtr red = Material::Make(1.0f,0.0f,0.0f);

  // table base transform and cube shape
  TransformPtr trf1 = Transform::Make();
  trf1->Scale(3.0f,0.3f,3.0f);
  trf1->Translate(0.0f,-1.0f,0.0f);
  ShapePtr cube = Cube::Make();

  // create basic illumination shader (used by root node)
  ShaderPtr shader = Shader::Make(light,"world");
  shader->AttachVertexShader("../shaders/ilum_vert/vertex.glsl");
  shader->AttachFragmentShader("../shaders/ilum_vert/fragment.glsl");
  shader->Link();

  // create and position Luxor lamp
  LuxorPtr lux = Luxor::Make();
  g_lux = lux; // keep a reference so callbacks can access engine
  TransformPtr trf_lux = Transform::Make();
  trf_lux->Scale(0.035f,0.035f,0.035f);
  trf_lux->Translate(0.0f,-0.75f,0.0f);
  NodePtr lux_node = Node::Make(trf_lux, { lux->GetNode() });

  // root has the shader so children inherit shading; add table base and lux
  NodePtr root = Node::Make(shader,
    { Node::Make(trf1, { red }, { cube }),
      lux_node
    }
  );
  scene = Scene::Make(root);
  // register Luxor engine so animations advance every frame
  if (g_lux && g_lux->GetEngine())
    scene->AddEngine(g_lux->GetEngine());
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
  // press 'J' to trigger Luxor jump-forward animation
  if (key == GLFW_KEY_J && action == GLFW_PRESS) {
    if (g_lux && g_lux->GetEngine()) {
      auto eng = g_lux->GetEngine();
      bool ok = eng->flip();
      if (ok) {
        std::cout << "Luxor: flip started\n";
      } else {
        // try to put it in 'down' state so jump can be triggered next
        bool down = eng->StandDown();
        if (down) std::cout << "Luxor: StandDown started (press J again to jump)\n";
        else std::cout << "Luxor: Could not start JumpForward or StandDown (maybe already animating)\n";
      }
    }
  }
}

static void resize (GLFWwindow* win, int width, int height)
{
  glViewport(0,0,width,height);
}

static void cursorpos (GLFWwindow* win, double x, double y)
{
  // convert screen pos (upside down) to framebuffer pos (e.g., retina displays)
  int wn_w, wn_h, fb_w, fb_h;
  glfwGetWindowSize(win, &wn_w, &wn_h);
  glfwGetFramebufferSize(win, &fb_w, &fb_h);
  x = x * fb_w / wn_w;
  y = (wn_h - y) * fb_h / wn_h;
  arcball->AccumulateMouseMotion(int(x),int(y));
}
static void cursorinit (GLFWwindow* win, double x, double y)
{
  // convert screen pos (upside down) to framebuffer pos (e.g., retina displays)
  int wn_w, wn_h, fb_w, fb_h;
  glfwGetWindowSize(win, &wn_w, &wn_h);
  glfwGetFramebufferSize(win, &fb_w, &fb_h);
  x = x * fb_w / wn_w;
  y = (wn_h - y) * fb_h / wn_h;
  arcball->InitMouseMotion(int(x),int(y));
  glfwSetCursorPosCallback(win, cursorpos);     // cursor position callback
}
static void mousebutton (GLFWwindow* win, int button, int action, int mods)
{
  if (action == GLFW_PRESS) {
    glfwSetCursorPosCallback(win, cursorinit);     // cursor position callback
  }
  else // GLFW_RELEASE 
    glfwSetCursorPosCallback(win, nullptr);      // callback disabled
}

int main ()
{
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,1);
  glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);       // required for mac os
  glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER,GLFW_TRUE);  // option for mac os
#endif

  glfwSetErrorCallback(error);

  GLFWwindow* win = glfwCreateWindow(600,400,"Window title",nullptr,nullptr);
  assert(win);
  glfwSetFramebufferSizeCallback(win, resize);  // resize callback
  glfwSetKeyCallback(win, keyboard);            // keyboard callback
  glfwSetMouseButtonCallback(win, mousebutton); // mouse button callback
  glfwSetScrollCallback(win, scroll_callback); // zoom with scroll
  
  glfwMakeContextCurrent(win);
#ifdef _WIN32
  if (!gladLoadGL(glfwGetProcAddress)) {
      printf("Failed to initialize GLAD OpenGL context\n");
      exit(1);
  }
#endif
  printf("OpenGL version: %s\n", glGetString(GL_VERSION));

  glewInit();

  initialize();

  float t0 = float(glfwGetTime());
  while(!glfwWindowShouldClose(win)) {
    float t = float(glfwGetTime());
    // advance scene/engines
    scene->Update(t - t0);
    t0 = t;
    display(win);
    glfwSwapBuffers(win);
    glfwPollEvents();
  }
  glfwTerminate();
  return 0;
}

