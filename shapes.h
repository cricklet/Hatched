#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <memory>
#include <vector>
using namespace std;

#include "mesh.h"

struct Shape {
  vector<shared_ptr<Mesh>> meshes;
  vector<glm::mat4> transforms;
};

float plane[] = { // xyz, norm, uv
    -1,-1,0, 0,0,1, 0,0,
    1,-1,0,  0,0,1, 2,0,
    1,1,0,   0,0,1, 2,2,

    -1,-1,0, 0,0,1, 0,0,
    1,1,0,   0,0,1, 2,2,
    -1,1,0,  0,0,1, 0,2,
};

int vertexSize = 8;
int numVertices = 6;

Shape generatePlane(float radius) {
  vector<Vertex> vertices;
  vector<GLuint> indices;

  for (int i = 0; i < numVertices; i ++) {
    int vertexOffset = i * vertexSize;

    Vertex v;
    v.position.x = plane[vertexOffset + 0] * radius;
    v.position.y = plane[vertexOffset + 1] * radius;
    v.position.z = plane[vertexOffset + 2] * radius;
    v.normal.x = plane[vertexOffset + 3];
    v.normal.y = plane[vertexOffset + 4];
    v.normal.z = plane[vertexOffset + 5];
    v.uv.x = plane[vertexOffset + 6] * radius * 2;
    v.uv.y = plane[vertexOffset + 7] * radius * 2;

    vertices.push_back(v);
    indices.push_back(i);
  }

  Shape s;
  auto m = make_shared<Mesh>(vertices, indices);
  s.meshes.push_back(m);
  s.transforms.push_back(glm::mat4());

  return s;
}

