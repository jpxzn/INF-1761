
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
#include <glm/gtc/matrix_transform.hpp>
#include "texture.h"
#include "transform.h"
#include "cube.h"
#include "quad.h"
#include "sphere.h"
#include "instanced_points.h"
#include "objmesh.h"
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
static ShaderPtr g_instShadowShader = nullptr; // GS instancing lighting
static ShaderPtr g_instDepthShader = nullptr;  // GS instancing depth
static NodePtr g_instRoot = nullptr;
static ScenePtr g_instScene = nullptr;
static bool g_instEnabled = true;
static bool g_wireframe = false;               // debug: toggle wireframe
static bool g_printOnce = true;                // print matrices first frame

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
  // set background color (dark gray for contrast) 
  glClearColor(0.08f,0.08f,0.1f,1.0f);
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
  trf1->Scale(7.0f,0.3f,4.5f);
  trf1->Translate(0.0f,-1.0f,0.0f);
  ShapePtr cube = Cube::Make();

  // create shadow lighting shader
  g_shadowShader = Shader::Make(light,"world");
  g_shadowShader->AttachVertexShader("../shaders/shadow/shadow_vertex.glsl");
  g_shadowShader->AttachFragmentShader("../shaders/shadow/shadow_fragment.glsl");
  g_shadowShader->Link();

  // depth-only shader for shadow map generation 
  g_depthShader = Shader::Make(nullptr,"world");
  g_depthShader->AttachVertexShader("../shaders/shadow/depth_vertex.glsl");
  g_depthShader->AttachFragmentShader("../shaders/shadow/depth_fragment.glsl");
  g_depthShader->Link();

  // Geometry-shader instancing: lighting pass
  std::cout << "[INIT] Creating instancing shaders...\n";
  g_instShadowShader = Shader::Make(light, "world");
  g_instShadowShader->AttachVertexShader("../shaders/inst/inst_vertex.glsl");
  g_instShadowShader->AttachGeometryShader("../shaders/inst/inst_geometry.glsl");
  g_instShadowShader->AttachFragmentShader("../shaders/inst/inst_fragment.glsl");
  g_instShadowShader->Link();
  std::cout << "[INIT] Inst shadow shader linked OK\n";

  // Geometry-shader instancing: depth pass
  g_instDepthShader = Shader::Make(nullptr, "world");
  g_instDepthShader->AttachVertexShader("../shaders/inst/inst_vertex.glsl");
  g_instDepthShader->AttachGeometryShader("../shaders/inst/inst_depth_geometry.glsl");
  g_instDepthShader->AttachFragmentShader("../shaders/shadow/depth_fragment.glsl");
  g_instDepthShader->Link();
  std::cout << "[INIT] Inst depth shader linked OK\n";

  // create and position Luxor lamp
  LuxorPtr lux = Luxor::Make();
  g_lux = lux; // keep a reference so callbacks can access engine
  TransformPtr trf_lux = Transform::Make();
  trf_lux->Scale(0.035f,0.035f,0.035f);
  trf_lux->Translate(1.5f,-0.75f,-0.5f);
  NodePtr lux_node = Node::Make(trf_lux, { lux->GetNode() });

  // Load chair mesh
  std::cout << "[INIT] Loading chair.obj...\n";
  ObjMeshPtr chair = ObjMesh::Make("../obj/Chair.obj");
  TransformPtr trf_chair = Transform::Make();
  trf_chair->Scale(0.025f, 0.025f, 0.025f);
  trf_chair->Rotate(180.0f, 0.0f, 1.0f, 0.0f);
  trf_chair->Translate(-45.0f, -0.85f, 0.8f); 
  AppearancePtr chairMat = Material::Make(0.6f, 0.4f, 0.3f); // Brown color
  NodePtr chair_node = Node::Make(trf_chair, { chairMat }, { chair });

  // Load teapot mesh
  std::cout << "[INIT] Loading teapot.obj...\n";
  ObjMeshPtr teapot = ObjMesh::Make("../obj/teapot.obj");
  TransformPtr trf_teapot = Transform::Make();
  trf_teapot->Scale(0.05f, 0.05f, 0.05f);
  trf_teapot->Rotate(-90.0f, 1.0f, 0.0f, 0.0f);
  trf_teapot->Translate(-23.5f, -0.85f, -0.5f);
  AppearancePtr teapotMat = Material::Make(0.8f, 0.8f, 0.9f);
  NodePtr teapot_node = Node::Make(trf_teapot, { teapotMat }, { teapot });

  // root has the shader so children inherit shading
  g_root = Node::Make(g_shadowShader,
    { Node::Make(trf1, { red }, { cube }),
      chair_node,
      teapot_node,
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
  g_shadowDepth->SetCompareMode();
  g_shadowFBO = Framebuffer::Make(g_shadowDepth,{});

  // Create instanced objects
  std::vector<InstanceData> instances;
  float tableTop = 0.00f;
  
  // Dispersed cones across the wider table
  instances.push_back({ glm::vec3(-2.8f, tableTop, 1.5f), glm::vec3(0.15f, 0.4f, 0.15f), 2.0f, glm::vec3(0.9f, 0.5f, 0.2f) });
  instances.push_back({ glm::vec3(0.3f, tableTop, 1.6f), glm::vec3(0.15f, 0.4f, 0.15f), 2.0f, glm::vec3(0.9f, 0.5f, 0.2f) });
  instances.push_back({ glm::vec3(2.2f, tableTop, 1.2f), glm::vec3(0.15f, 0.4f, 0.15f), 2.0f, glm::vec3(0.9f, 0.5f, 0.2f) });
  instances.push_back({ glm::vec3(2.8f, tableTop, -0.8f), glm::vec3(0.15f, 0.4f, 0.15f), 2.0f, glm::vec3(0.9f, 0.5f, 0.2f) });
  instances.push_back({ glm::vec3(0.8f, tableTop, -1.5f), glm::vec3(0.15f, 0.4f, 0.15f), 2.0f, glm::vec3(0.9f, 0.5f, 0.2f) });
  instances.push_back({ glm::vec3(-2.2f, tableTop, -1.2f), glm::vec3(0.15f, 0.4f, 0.15f), 2.0f, glm::vec3(0.9f, 0.5f, 0.2f) });
  
  // Cylinders (pencil holders) - dispersed
  instances.push_back({ glm::vec3(-2.5f, tableTop, -0.2f), 
                        glm::vec3(0.2f, 0.5f, 0.2f), 1.0f, 
                        glm::vec3(0.3f, 0.7f, 0.9f) });
  instances.push_back({ glm::vec3(2.5f, tableTop, 0.5f), 
                        glm::vec3(0.18f, 0.45f, 0.18f), 1.0f, 
                        glm::vec3(0.8f, 0.3f, 0.5f) });
  
  // Cubes (boxes) - dispersed
  instances.push_back({ glm::vec3(3.0f, tableTop, -1.5f), 
                        glm::vec3(0.4f, 0.15f, 0.6f), 0.0f, 
                        glm::vec3(0.2f, 0.5f, 0.8f) });
  instances.push_back({ glm::vec3(-3.0f, tableTop, 0.8f), 
                        glm::vec3(0.35f, 0.12f, 0.5f), 0.0f, 
                        glm::vec3(0.9f, 0.2f, 0.3f) });
  
  // Spheres (octa) - dispersed
  instances.push_back({ glm::vec3(-1.2f, tableTop, 1.8f), 
                        glm::vec3(0.25f), 3.0f, 
                        glm::vec3(0.9f, 0.9f, 0.2f) });
  instances.push_back({ glm::vec3(1.8f, tableTop, -1.8f), 
                        glm::vec3(0.22f), 3.0f, 
                        glm::vec3(0.3f, 0.9f, 0.4f) });

  std::cout << "[INIT] Creating instanced objects (" << instances.size() << " instances)...\n";
  InstancedPointsPtr instShape = InstancedPoints::Make(instances);
  TransformPtr trf_inst = Transform::Make();
  AppearancePtr instMat = Material::Make(0.3f, 0.7f, 0.2f);
  g_instRoot = Node::Make(g_instShadowShader, trf_inst, { instMat }, { instShape });
  g_instScene = Scene::Make(g_instRoot);
  std::cout << "[INIT] Instanced scene created OK\n";
}

static void display (GLFWwindow* win)
{ 
  // optional wireframe mode
  if (g_wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  // 1st pass: render depth from light POV
  g_root->SetShader(g_depthShader); // swap to depth shader
  g_shadowFBO->Bind();
  glViewport(0,0,SHADOW_SIZE,SHADOW_SIZE);
  glClear(GL_DEPTH_BUFFER_BIT);
  scene->Render(light_camera);
  // instanced depth pass
  if (g_instEnabled && g_instScene) {
    g_instRoot->SetShader(g_instDepthShader);
    g_instScene->Render(light_camera);
  }
  g_shadowFBO->Unbind();

  // 2nd pass: render scene from viewer with shadows + planar reflection
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
  // bind depth texture
  g_shadowShader->ActiveTexture("shadowMap");
  glBindTexture(GL_TEXTURE_2D,g_shadowDepth->GetTexId());

  Error::Check("before render with shadows");
  
  scene->Render(camera);
  
  // instanced lighting pass
  if (g_instEnabled && g_instScene) {
    g_instShadowShader->UseProgram();
    g_instShadowShader->SetUniform("LightView", LightView);
    g_instShadowShader->SetUniform("LightProj", LightProj);
    g_instShadowShader->SetUniform("ReflectMat", glm::mat4(1.0f));
    
    g_instShadowShader->ActiveTexture("shadowMap");
    glBindTexture(GL_TEXTURE_2D, g_shadowDepth->GetTexId());
    g_instRoot->SetShader(g_instShadowShader);
    
    g_instScene->Render(camera);
    Error::Check("after instanced render");
    
    g_instShadowShader->DeactiveTexture();
  }
  // release texture unit counter
  g_shadowShader->DeactiveTexture();
  Error::Check("after render with shadows");

  if (g_printOnce) {
    g_printOnce = false;
    std::cout << "[INFO] First frame OK. Instancing: " << (g_instEnabled?"ON":"OFF") << " (toggle with I)\n";
  }
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
    else if (key == GLFW_KEY_P && action == GLFW_PRESS) {
      g_wireframe = !g_wireframe;
      std::cout << "Wireframe: " << (g_wireframe?"ON":"OFF") << "\n";
    }
    else if (key == GLFW_KEY_I && action == GLFW_PRESS) {
      g_instEnabled = !g_instEnabled;
      std::cout << "Instanced objects: " << (g_instEnabled?"ON":"OFF") << "\n";
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

  GLFWwindow* win = glfwCreateWindow(800,600,"Projeto 3",nullptr,nullptr);
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

