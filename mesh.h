#ifndef MESH
#define MESH

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <sys/time.h>

#define GLEW_STATIC
#include <GL/glew.h>

#include <string>
#include <vector>
using namespace std;

#include "shared.h"

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 uv;
};

struct Texture {
  GLuint index;
};

class Mesh {
 public:
  Mesh(const vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures);

  vector<Vertex> vertices;
  vector<GLuint> indices;
  vector<Texture> textures;
  
  void Draw(const Uniforms &uniforms);
  
 private:
  GLuint vao, vbo, ebo;
};  

#endif
