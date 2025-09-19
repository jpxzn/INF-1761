#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Error.h"
#include "Shader.h"
#include "Circle.h"
#include "Triangle.h"
#include "Ticks.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctime>

static TrianglePtr ponteiroHora;
static TrianglePtr ponteiroMin;
static TrianglePtr ponteiroSeg;
static CirclePtr circle;
static TicksPtr ticks;
static ShaderPtr shd;

static void error (int code, const char* msg)
{
  printf("GLFW error %d: %s\n", code, msg);
  glfwTerminate();
  exit(1);
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

static void initialize ()
{
  std::time_t agora = std::time(nullptr);
  std::tm* local = std::localtime(&agora);
  int hora = local->tm_hour;
  int min = local->tm_min;
  int seg = local->tm_sec;

  glClearColor(0.0f, 1.0f, 1.0f, 1.0f); // fundo azul claro

  float angHora = 360.0f - (hora * 30.0f);
  float angMin  = 360.0f - (min  * 6.0f);
  float angSeg  = 360.0f - (seg  * 6.0f);

  ponteiroHora = Triangle::Make(0,0,0);
  ponteiroHora->setScale(0.6f);
  ponteiroHora->setRotation(angHora);

  ponteiroMin = Triangle::Make(0,0,0);
  ponteiroMin->setScale(0.8f);
  ponteiroMin->setRotation(angMin);

  ponteiroSeg = Triangle::Make(255,0,0);
  ponteiroSeg->setScale(1.0f);
  ponteiroSeg->setRotation(angSeg);

  const float RAIO_MOSTRADOR = 0.6f;

  circle = Circle::Make(RAIO_MOSTRADOR, 50);

  ticks = Ticks::Make(RAIO_MOSTRADOR, 0.88f);

  shd = Shader::Make();
  shd->AttachVertexShader("shaders/vertex.glsl");
  shd->AttachFragmentShader("shaders/fragment.glsl");
  shd->Link();

  Error::Check("initialize");
}

static void display (GLFWwindow* win)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  shd->UseProgram();

  circle->Draw();
  ticks->Draw(*shd);

  ponteiroHora->Draw(*shd);
  ponteiroMin->Draw(*shd);
  ponteiroSeg->Draw(*shd);

  Error::Check("display");
}

void update(float deltaTime)
{
  std::time_t agora = std::time(nullptr);
  std::tm* local = std::localtime(&agora);

  int hora = local->tm_hour;
  int min  = local->tm_min;
  int seg  = local->tm_sec;

  float angSeg = 360.0f - (seg * 6.0f);
  float angMin = 360.0f - (min * 6.0f + seg * 0.1f);
  float angHora = 360.0f - ((hora % 12) * 30.0f + min * 0.5f);
  

  ponteiroSeg->setRotation(angSeg);
  ponteiroMin->setRotation(angMin);
  ponteiroHora->setRotation(angHora);
}

int main ()
{
  glfwSetErrorCallback(error);
  if (!glfwInit()) {
    printf("Failed to initialize GLFW\n");
    return 1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* win = glfwCreateWindow(600, 600, "Relógio", nullptr, nullptr);
  if (!win) {
    printf("Failed to create GLFW window\n");
    glfwTerminate();
    return 1;
  }

  glfwSetFramebufferSizeCallback(win, resize);
  glfwSetKeyCallback(win, keyboard);
  glfwMakeContextCurrent(win);

  GLenum glewErr = glewInit(); // chamada única
  if (glewErr != GLEW_OK) {
    printf("Failed to initialize GLEW OpenGL context: %s\n", glewGetErrorString(glewErr));
    glfwTerminate();
    return 1;
  }

  printf("OpenGL version: %s\n", glGetString(GL_VERSION));

  initialize();

  float lastTime = glfwGetTime();

  while(!glfwWindowShouldClose(win)) {
    float currentTime = glfwGetTime();
    float deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    update(deltaTime);
    display(win);
    glfwSwapBuffers(win);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
