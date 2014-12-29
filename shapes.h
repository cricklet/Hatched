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

Shape generatePlane(float xSize, float ySize) {
  vector<Vertex> vertices;
  vector<GLuint> indices;

  for (int i = 0; i < numVertices; i ++) {
    int vertexOffset = i * vertexSize;

    Vertex v;
    v.position.x = plane[vertexOffset + 0] * xSize / 2;
    v.position.y = plane[vertexOffset + 1] * ySize / 2;
    v.position.z = plane[vertexOffset + 2];
    v.normal.x = plane[vertexOffset + 3];
    v.normal.y = plane[vertexOffset + 4];
    v.normal.z = plane[vertexOffset + 5];
    v.uv.x = plane[vertexOffset + 6] * xSize / 2;
    v.uv.y = plane[vertexOffset + 7] * ySize / 2;

    vertices.push_back(v);
    indices.push_back(i);
  }

  Shape s;
  auto m = make_shared<Mesh>(vertices, indices);
  s.meshes.push_back(m);
  s.transforms.push_back(glm::mat4());

  return s;
}

Shape generateBox(
    float xSize, float ySize, float zSize,
    bool genTop = true, bool flipNormals = false) {

  Shape s;
  auto addSide = [&] (Shape n) {
    s.meshes.insert(s.meshes.end(), n.meshes.begin(), n.meshes.end());
    s.transforms.insert(s.transforms.end(), n.transforms.begin(), n.transforms.end());
  };

  {
    auto bottom = generatePlane(xSize, ySize);
    bottom.transforms[0] *= glm::translate(glm::mat4(), glm::vec3(0,0,-zSize/2));
    bottom.transforms[0] *= glm::rotate(glm::mat4(), (float) (M_PI), glm::vec3(0,1,0));
    addSide(bottom);
  }

  if (genTop) {
    auto top = generatePlane(xSize, ySize);
    top.transforms[0] *= glm::translate(glm::mat4(), glm::vec3(0,0,zSize/2));
    addSide(top);
  }

  {
    auto left = generatePlane(xSize, zSize);
    left.transforms[0] *= glm::translate(glm::mat4(), glm::vec3(0,-ySize/2,0));
    left.transforms[0] *= glm::rotate(glm::mat4(), (float) (M_PI / 2), glm::vec3(1,0,0));
    addSide(left);
  }

  {
    auto right = generatePlane(xSize, zSize);
    right.transforms[0] *= glm::translate(glm::mat4(), glm::vec3(0,ySize/2,0));
    right.transforms[0] *= glm::rotate(glm::mat4(), (float) (-M_PI / 2), glm::vec3(1,0,0));
    addSide(right);
  }

  {
    auto front = generatePlane(zSize, ySize);
    front.transforms[0] *= glm::translate(glm::mat4(), glm::vec3(xSize/2,0,0));
    front.transforms[0] *= glm::rotate(glm::mat4(), (float) (M_PI / 2), glm::vec3(0,1,0));
    addSide(front);
  }

  {
    auto back = generatePlane(zSize, ySize);
    back.transforms[0] *= glm::translate(glm::mat4(), glm::vec3(-xSize/2,0,0));
    back.transforms[0] *= glm::rotate(glm::mat4(), (float) (-M_PI / 2), glm::vec3(0,1,0));
    addSide(back);
  }

  if (flipNormals) {
    for (auto &t: s.transforms) {
      t = t * glm::rotate(glm::mat4(), (float) (M_PI), glm::vec3(0,1,0));
    }
  }

  return s;
}

