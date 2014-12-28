#ifndef MESH
#define MESH

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <limits>

#include <sys/time.h>

#define GLEW_STATIC
#include <GL/glew.h>

#include <string>
#include <memory>
#include <vector>
using namespace std;

#include "helper.h"
#include "uniforms.h"
#include "renderable.h"

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 uv;
};

struct Bounds {
  float minx = FLT_MIN;
  float miny = FLT_MIN;
  float minz = FLT_MIN;
  float maxx = FLT_MAX;
  float maxy = FLT_MAX;
  float maxz = FLT_MAX;
};

class Mesh : public Renderable {
public:
  Mesh(const vector<Vertex> vertices, vector<GLuint> indices);
  ~Mesh();

  vector<Vertex> vertices;
  vector<GLuint> indices;

  void Render(Uniforms uniforms, glm::mat4 parentTransform);
  void BindToShader(GLuint shaderProgram);

  Bounds GetBounds();

private:
  GLuint vao, vbo, ebo;
  glm::vec3 color;
};

shared_ptr<Mesh> generateCube(
    float minX, float minY, float minZ,
    float maxX, float maxY, float maxZ,
    bool flipNorms = false);

#endif
