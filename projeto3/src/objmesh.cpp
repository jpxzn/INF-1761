#include "objmesh.h"

#ifdef _WIN32
#include <glad/gl.h>
#elif __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#include <GL/gl.h>
#endif

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <cstdlib>

ObjMeshPtr ObjMesh::Make(const std::string& filename)
{
  return ObjMeshPtr(new ObjMesh(filename));
}

ObjMesh::ObjMesh(const std::string& filename)
: m_nind(0)
{
  std::cout << "[ObjMesh] Loading: " << filename << std::endl;
  
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(filename,
    aiProcess_Triangulate |
    aiProcess_GenNormals |
    aiProcess_JoinIdenticalVertices |
    aiProcess_FlipUVs
  );
  
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    std::cerr << "[ObjMesh] Assimp Error: " << importer.GetErrorString() << std::endl;
    exit(1);
  }
  
  // For simplicity, just load the first mesh
  if (scene->mNumMeshes == 0) {
    std::cerr << "[ObjMesh] No meshes found in file: " << filename << std::endl;
    exit(1);
  }
  
  aiMesh* mesh = scene->mMeshes[0];
  std::cout << "[ObjMesh] Vertices: " << mesh->mNumVertices 
            << ", Faces: " << mesh->mNumFaces << std::endl;
  
  // Extract vertex data
  std::vector<float> vertices;
  std::vector<float> normals;
  std::vector<float> texcoords;
  std::vector<unsigned int> indices;
  
  vertices.reserve(mesh->mNumVertices * 3);
  normals.reserve(mesh->mNumVertices * 3);
  if (mesh->mTextureCoords[0]) {
    texcoords.reserve(mesh->mNumVertices * 2);
  }
  
  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    vertices.push_back(mesh->mVertices[i].x);
    vertices.push_back(mesh->mVertices[i].y);
    vertices.push_back(mesh->mVertices[i].z);
    
    if (mesh->mNormals) {
      normals.push_back(mesh->mNormals[i].x);
      normals.push_back(mesh->mNormals[i].y);
      normals.push_back(mesh->mNormals[i].z);
    }
    
    if (mesh->mTextureCoords[0]) {
      texcoords.push_back(mesh->mTextureCoords[0][i].x);
      texcoords.push_back(mesh->mTextureCoords[0][i].y);
    }
  }
  
  // Extract indices
  indices.reserve(mesh->mNumFaces * 3);
  for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
    aiFace face = mesh->mFaces[i];
    for (unsigned int j = 0; j < face.mNumIndices; j++) {
      indices.push_back(face.mIndices[j]);
    }
  }
  
  m_nind = (unsigned int)indices.size();
  
  // Create VAO and VBOs
  glGenVertexArrays(1, &m_vao);
  glBindVertexArray(m_vao);
  
  // Position buffer (location 0)
  if (!vertices.empty()) {
    GLuint vbo;
    glGenBuffers(1, &vbo);
    m_vbos.push_back(vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
  }
  
  // Normal buffer (location 1)
  if (!normals.empty()) {
    GLuint vbo;
    glGenBuffers(1, &vbo);
    m_vbos.push_back(vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
  }
  
  // Texcoord buffer (location 3)
  if (!texcoords.empty()) {
    GLuint vbo;
    glGenBuffers(1, &vbo);
    m_vbos.push_back(vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, texcoords.size() * sizeof(float), texcoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(3);
  }
  
  // Index buffer
  if (!indices.empty()) {
    GLuint ibo;
    glGenBuffers(1, &ibo);
    m_vbos.push_back(ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
  }
  
  glBindVertexArray(0);
  std::cout << "[ObjMesh] Loaded successfully with " << m_nind << " indices" << std::endl;
}

ObjMesh::~ObjMesh()
{
  if (m_vao) {
    glDeleteVertexArrays(1, &m_vao);
  }
  if (!m_vbos.empty()) {
    glDeleteBuffers((GLsizei)m_vbos.size(), m_vbos.data());
  }
}

void ObjMesh::Draw(StatePtr)
{
  glBindVertexArray(m_vao);
  glDrawElements(GL_TRIANGLES, m_nind, GL_UNSIGNED_INT, 0);
}
