#include "Shader.h"
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>

static std::string ReadFile (const std::string& filename) {
  std::ifstream fp(filename);
  if (!fp.is_open()) { std::cerr << "Could not open file: " << filename << std::endl; std::exit(1); }
  std::stringstream ss; ss << fp.rdbuf(); return ss.str();
}

static void CompileShader (const std::string& filename, GLuint id) {
  glCompileShader(id);
  GLint status; glGetShaderiv(id, GL_COMPILE_STATUS, &status);
  if (!status) {
    GLint len; glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);
    std::string log(len, '\0');
    glGetShaderInfoLog(id, len, 0, log.data());
    std::cerr << filename << ":\n" << log << std::endl;
    std::exit(1);
  }
}

static GLuint CreateShader (GLenum type, const std::string& filename) {
  GLuint id = glCreateShader(type);
  if (!id) { std::cerr << "Could not create shader object"; std::exit(1); }
  std::string src = ReadFile(filename); const char* csrc = src.c_str();
  glShaderSource(id,1,&csrc,nullptr); CompileShader(filename,id); return id;
}

ShaderPtr Shader::Make () { return ShaderPtr(new Shader()); }

Shader::Shader () { m_pid = glCreateProgram(); if (!m_pid) { std::cerr << "Could not create shader object"; std::exit(1);} }
Shader::~Shader () {}

void Shader::AttachVertexShader (const std::string& f) { glAttachShader(m_pid, CreateShader(GL_VERTEX_SHADER,f)); }
void Shader::AttachFragmentShader (const std::string& f) { glAttachShader(m_pid, CreateShader(GL_FRAGMENT_SHADER,f)); }
void Shader::AttachGeometryShader (const std::string& f) { glAttachShader(m_pid, CreateShader(GL_GEOMETRY_SHADER,f)); }
void Shader::AttachTesselationShader (const std::string& c, const std::string& e) {
  glAttachShader(m_pid, CreateShader(GL_TESS_CONTROL_SHADER,c));
  glAttachShader(m_pid, CreateShader(GL_TESS_EVALUATION_SHADER,e));
}

void Shader::Link () {
  glLinkProgram(m_pid);
  GLint status; glGetProgramiv(m_pid, GL_LINK_STATUS,&status);
  if (!status) {
    GLint len; glGetProgramiv(m_pid, GL_INFO_LOG_LENGTH,&len);
    std::string log(len,'\0'); glGetProgramInfoLog(m_pid,len,0,log.data());
    std::cerr << log << std::endl; std::exit(1);
  }
}

void Shader::UseProgram () const { glUseProgram(m_pid); }

void Shader::SetUniform (const std::string& n, int x) const { GLint loc = glGetUniformLocation(m_pid,n.c_str()); glUniform1i(loc,x); }
void Shader::SetUniform (const std::string& n, float x) const { GLint loc = glGetUniformLocation(m_pid,n.c_str()); glUniform1f(loc,x); }
void Shader::SetUniform (const std::string& n, const glm::vec3& v) const { GLint loc = glGetUniformLocation(m_pid,n.c_str()); glUniform3fv(loc,1,glm::value_ptr(v)); }
void Shader::SetUniform (const std::string& n, const glm::vec4& v) const { GLint loc = glGetUniformLocation(m_pid,n.c_str()); glUniform4fv(loc,1,glm::value_ptr(v)); }
void Shader::SetUniform (const std::string& n, const glm::mat4& m) const { GLint loc = glGetUniformLocation(m_pid,n.c_str()); glUniformMatrix4fv(loc,1,GL_FALSE,glm::value_ptr(m)); }
void Shader::SetUniform (const std::string& n, const std::vector<int>& v) const { GLint loc = glGetUniformLocation(m_pid,n.c_str()); glUniform1iv(loc,v.size(),v.data()); }
void Shader::SetUniform (const std::string& n, const std::vector<float>& v) const { GLint loc = glGetUniformLocation(m_pid,n.c_str()); glUniform1fv(loc,v.size(),v.data()); }
void Shader::SetUniform (const std::string& n, const std::vector<glm::vec3>& v) const { GLint loc = glGetUniformLocation(m_pid,n.c_str()); glUniform3fv(loc,v.size(),(float*)v.data()); }
void Shader::SetUniform (const std::string& n, const std::vector<glm::vec4>& v) const { GLint loc = glGetUniformLocation(m_pid,n.c_str()); glUniform4fv(loc,v.size(),(float*)v.data()); }
void Shader::SetUniform (const std::string& n, const std::vector<glm::mat4>& v) const { GLint loc = glGetUniformLocation(m_pid,n.c_str()); glUniformMatrix4fv(loc,v.size(),GL_FALSE,(float*)v.data()); }
