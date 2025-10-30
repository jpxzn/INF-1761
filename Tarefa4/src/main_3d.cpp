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
#include "cylinder.h"

#include <iostream>
#include <cassert>

static float viewer_pos[3] = {2.0f, 3.5f, 4.0f};

static ScenePtr scene;
static Camera3DPtr camera;
static ArcballPtr arcball;

static void initialize(void)
{
    // Set background color and enable depth test
    glClearColor(0.0f, 0.0f, 0.00f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);  // Cull back faces

    // Create objects and camera
    camera = Camera3D::Make(viewer_pos[0], viewer_pos[1], viewer_pos[2]);
    arcball = camera->CreateArcball();

    // Lighting setup
    LightPtr light = Light::Make(1.5f, 2.5f, 2.0f, 1.0f, "camera");

    // Materials (for other objects)
    MaterialPtr esfera = Material::Make(0.6f, 1.0f, 0.6f);  // Material for the sphere (texture will replace this)
    MaterialPtr tampo = Material::Make(1.0f, 0.5f, 0.5f);
    MaterialPtr cubo = Material::Make(0.85f, 0.73f, 0.6f);
    MaterialPtr cilindro = Material::Make(0.6f, 0.4f, 0.2f);  // Material for the cylinder

    // Load texture for the sphere
    AppearancePtr tex_earth = Texture::Make("decal", "../images/earth.jpg");  // Textura for the sphere

    // Transformations for each object
    TransformPtr trf_floor = Transform::Make();
    trf_floor->Scale(3.0f, 0.3f, 3.0f);
    trf_floor->Translate(0.0f, -1.0f, 0.0f);

    TransformPtr trf_cube = Transform::Make();
    trf_cube->Scale(0.8f, 0.5f, 0.8f);
    trf_cube->Translate(0.0f, -0.35f, 0.0f);

    TransformPtr trf_sphere = Transform::Make();
    trf_sphere->Scale(0.3f, 0.3f, 0.3f);
    trf_sphere->Translate(0.0f, 2.0f, 0.0f);

    TransformPtr trf_cylinder = Transform::Make();
    trf_cylinder->Scale(0.5f, 0.5f, 0.5f);  // Adjust the scale
    trf_cylinder->Translate(2.0f, 1.0f, 0.0f);  // Position for the cylinder

    // Shapes
    ShapePtr cube = Cube::Make();
    ShapePtr sphere = Sphere::Make();
    ShapePtr cylinder = Cylinder::Make(64, 64, 2.0f, 0.5f);  // Creating the cylinder

    // Shader setup
    ShaderPtr shader = Shader::Make(light, "camera");
    shader->AttachVertexShader("../shaders/ilum_vert/vertex_texture.glsl");
    shader->AttachFragmentShader("../shaders/ilum_vert/fragment_texture.glsl");
    shader->Link();

    // Create nodes and apply textures
    NodePtr floor_node = Node::Make(trf_floor, {tampo}, {cube});
    NodePtr cube_node = Node::Make(trf_cube, {cubo}, {cube});  // No texture for the cube

    // Apply texture only to the sphere
    NodePtr sphere_node = Node::Make(trf_sphere, {tex_earth}, {sphere});

    // Create cylinder node without texture
    NodePtr cylinder_node = Node::Make(trf_cylinder, {cilindro}, {cylinder});  // No texture for the cylinder

    // Create the scene tree (only the sphere node uses texture)
    NodePtr root = Node::Make(shader, 
        { 
            floor_node,  // Floor node
            cube_node,   // Cube node without texture
            sphere_node, // Sphere node with texture
            {cylinder_node} // Cylinder node without texture
        }
    );

    scene = Scene::Make(root);
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

  while(!glfwWindowShouldClose(win)) {
    display(win);
    glfwSwapBuffers(win);
    glfwPollEvents();
  }
  glfwTerminate();
  return 0;
}
