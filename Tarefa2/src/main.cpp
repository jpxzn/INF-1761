#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Error.h"
#include "Shader.h"
#include "Circle.h"
#include "Triangle.h"
#include "Ticks.h"

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

static TrianglePtr ponteiroHora;
static TrianglePtr ponteiroMin;
static TrianglePtr ponteiroSeg;
static CirclePtr circle;
static ShaderPtr shd;
static TicksPtr ticks;
static const glm::mat4 IDENTITY(1.0f);

static double startMonotonic = 0.0;
static double baseSeconds = 0.0;

static void error (int code, const char* msg) {
  std::printf("GLFW error %d: %s\n", code, msg);
  glfwTerminate();
  std::exit(1);
}

static void keyboard (GLFWwindow* window, int key, int, int action, int) {
  if (key == GLFW_KEY_Q && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static void resize (GLFWwindow*, int width, int height) {
  glViewport(0,0,width,height);
}

static void initialize () {
  glClearColor(0.f,1.f,1.f,1.f);
  startMonotonic = glfwGetTime();
  std::time_t tnow = std::time(nullptr);
  std::tm* lt = std::localtime(&tnow);
  int hour12 = lt->tm_hour % 12;
  baseSeconds = hour12 * 3600.0 + lt->tm_min * 60.0 + lt->tm_sec;

  ponteiroHora = Triangle::Make(); ponteiroHora->setScale(0.6f);
  ponteiroMin  = Triangle::Make(); ponteiroMin->setScale(0.8f);
  ponteiroSeg  = Triangle::Make(); ponteiroSeg->setScale(1.0f);

  circle = Circle::Make(0.6f, 50);
  ticks  = Ticks::Make(0.6f);
  
  shd = Shader::Make();
  shd->AttachVertexShader("shaders/vertex.glsl");
  shd->AttachFragmentShader("shaders/fragment.glsl");
  shd->Link();

  Error::Check("initialize");
}

static void computeAngles(float &angHora, float &angMin, float &angSeg) {
  double elapsed = glfwGetTime() - startMonotonic;
  double totalSeconds = std::fmod(baseSeconds + elapsed, 43200.0);
  double seg = std::fmod(totalSeconds, 60.0);
  double totalMinutes = totalSeconds / 60.0;
  double min = std::fmod(totalMinutes, 60.0);
  double hora = std::fmod(totalMinutes / 60.0, 12.0);
  angSeg  = 360.f - (float)(seg  * 6.0);
  angMin  = 360.f - (float)(min  * 6.0);
  angHora = 360.f - (float)(hora * 30.0);
}

static void update() {
  float aH, aM, aS;
  computeAngles(aH, aM, aS);
  ponteiroHora->setRotation(aH);
  ponteiroMin->setRotation(aM);
  ponteiroSeg->setRotation(aS);
}

static void display (GLFWwindow*) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  shd->UseProgram();
  shd->SetUniform("uModel", IDENTITY); circle->Draw();
  shd->SetUniform("uModel", IDENTITY); ticks->Draw();
  ponteiroHora->Draw(*shd);
  ponteiroMin->Draw(*shd);
  ponteiroSeg->Draw(*shd);
  Error::Check("display");
}

int main () {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,1);
  glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
  glfwSetErrorCallback(error);
  GLFWwindow* win = glfwCreateWindow(600,600,"Relógio",nullptr,nullptr);
  glfwSetFramebufferSizeCallback(win, resize);
  glfwSetKeyCallback(win, keyboard);
  glfwMakeContextCurrent(win);
  if (glewInit() != GLEW_OK) { std::printf("Failed to initialize GLEW OpenGL context\n"); return 1; }
  std::printf("OpenGL version: %s\n", glGetString(GL_VERSION));
  initialize();
  while(!glfwWindowShouldClose(win)) { update(); display(win); glfwSwapBuffers(win); glfwPollEvents(); }
  glfwTerminate();
  return 0;
}
