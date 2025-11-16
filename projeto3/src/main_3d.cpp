
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
#include "framebuffer.h"
#include "texdepth.h"
#include "polyoffset.h"
#include "luxor/luxor.h"

#include <iostream>
#include <cassert>

static float viewer_pos[3] = {2.0f, 3.5f, 2.0f};

static ScenePtr scene;
static Camera3DPtr camera;
static Camera3DPtr light_camera; // camera from light POV for shadow map
static ArcballPtr arcball;
// global fovy state (kept here because Camera3D has no getter)
static float g_cam_fovy = 45.0f;
static LuxorPtr g_lux = nullptr;
static NodePtr g_root = nullptr; // keep root so we can swap shaders in passes

// shadow mapping resources
static const int SHADOW_SIZE = 1024;
static TexDepthPtr g_shadowDepth = nullptr;
static FramebufferPtr g_shadowFBO = nullptr;
static ShaderPtr g_depthShader = nullptr;    // depth-only pass
static ShaderPtr g_shadowShader = nullptr;   // lighting + shadow
static glm::vec3 g_lightPos(3.0f,4.0f,3.0f);

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

  // world-space positional light (used also for shadow map)
  LightPtr light = Light::Make(g_lightPos.x,g_lightPos.y,g_lightPos.z,1.0f,"world");

  // red material for the table base
  AppearancePtr red = Material::Make(1.0f,0.0f,0.0f);

  // table base transform and cube shape
  TransformPtr trf1 = Transform::Make();
  trf1->Scale(3.0f,0.3f,3.0f);
  trf1->Translate(0.0f,-1.0f,0.0f);
  ShapePtr cube = Cube::Make();

  // create shadow lighting shader (world lighting so Mv = model matrix)
  g_shadowShader = Shader::Make(light,"world");
  g_shadowShader->AttachVertexShader("../shaders/shadow/shadow_vertex.glsl");
  g_shadowShader->AttachFragmentShader("../shaders/shadow/shadow_fragment.glsl");
  g_shadowShader->Link();

  // depth-only shader for shadow map generation (uses regular Mvp via light camera)
  g_depthShader = Shader::Make(nullptr,"world");
  g_depthShader->AttachVertexShader("../shaders/shadow/depth_vertex.glsl");
  g_depthShader->AttachFragmentShader("../shaders/shadow/depth_fragment.glsl");
  g_depthShader->Link();


  // create and position Luxor lamp
  LuxorPtr lux = Luxor::Make();
  g_lux = lux; // keep a reference so callbacks can access engine
  TransformPtr trf_lux = Transform::Make();
  trf_lux->Scale(0.035f,0.035f,0.035f);
  trf_lux->Translate(0.0f,-0.75f,0.0f);
  NodePtr lux_node = Node::Make(trf_lux, { lux->GetNode() });

  // root has the shader so children inherit shading; add table base and lux
  g_root = Node::Make(g_shadowShader,
    { Node::Make(trf1, { red }, { cube }),
      lux_node
    }
  );
  scene = Scene::Make(g_root);
  // register Luxor engine so animations advance every frame
  if (g_lux && g_lux->GetEngine())
    scene->AddEngine(g_lux->GetEngine());

  // setup light camera (independent of view camera) for shadow mapping
  light_camera = Camera3D::Make(g_lightPos.x,g_lightPos.y,g_lightPos.z);
  light_camera->SetCenter(0.0f,0.0f,0.0f); // look at origin
  light_camera->SetAngle(60.0f);
  light_camera->SetZPlanes(0.1f,50.0f);

  // create depth texture + framebuffer
  g_shadowDepth = TexDepth::Make("shadowMap", SHADOW_SIZE, SHADOW_SIZE);
  g_shadowDepth->SetCompareMode(); // enable hardware depth comparison
  g_shadowFBO = Framebuffer::Make(g_shadowDepth,{}); // depth-only

}

static void display (GLFWwindow* win)
{ 
  // 1st pass: render depth from light POV
  g_root->SetShader(g_depthShader); // swap to depth shader
  g_shadowFBO->Bind();
  glViewport(0,0,SHADOW_SIZE,SHADOW_SIZE);
  glClear(GL_DEPTH_BUFFER_BIT);
  scene->Render(light_camera);
  g_shadowFBO->Unbind();

  // 2nd pass: render scene from viewer with shadows
  g_root->SetShader(g_shadowShader); // swap back to lighting shader
  int wn_w, wn_h, fb_w, fb_h;
  glfwGetWindowSize(win,&wn_w,&wn_h);
  glfwGetFramebufferSize(win,&fb_w,&fb_h);
  glViewport(0,0,fb_w,fb_h);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // set light view/proj uniforms once
  glm::mat4 LightView = light_camera->GetViewMatrix();
  glm::mat4 LightProj = light_camera->GetProjMatrix();
  g_shadowShader->UseProgram();
  g_shadowShader->SetUniform("LightView",LightView);
  g_shadowShader->SetUniform("LightProj",LightProj);
  // bind depth texture (appearance load also works but we ensure here)
  g_shadowShader->ActiveTexture("shadowMap");
  glBindTexture(GL_TEXTURE_2D,g_shadowDepth->GetTexId());

  Error::Check("before render with shadows");
  scene->Render(camera);
  // release texture unit counter for base shader
  g_shadowShader->DeactiveTexture();
  Error::Check("after render with shadows");
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
  if (g_lux && g_lux->GetEngine()) {
    auto eng = g_lux->GetEngine();
    if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
      eng->StandUp();
    }
    else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
      eng->StandDown();
    }
    else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
      if (mods & GLFW_MOD_SHIFT) eng->TurnHead(-5.0f);
      // no movement on right arrow
    }
    else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
      if (mods & GLFW_MOD_SHIFT) eng->TurnHead(5.0f);
      // no movement on left arrow
    }
    else if (key == GLFW_KEY_W && action == GLFW_PRESS) {
      eng->JumpForward();
    }
    else if (key == GLFW_KEY_S && action == GLFW_PRESS) {
      eng->JumpBackward();
    }
    else if (key == GLFW_KEY_A && action == GLFW_PRESS) {
      eng->JumpLeft();
    }
    else if (key == GLFW_KEY_D && action == GLFW_PRESS) {
      eng->JumpRight();
    }
    else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
      // Mortal (flip) when crouched/down, fluid like side jumps
      eng->flip();
    }
    // removed 'J' flip shortcut to avoid accidental flips during tests
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

