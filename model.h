#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <sys/time.h>

#define _USE_MATH_DEFINES
#include <cmath>

#define GLEW_STATIC
#include <GL/glew.h>

#include <string>
#include <memory>
#include <vector>
using namespace std;

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "helper.h"
#include "renderable.h"
#include "mesh.h"

class Model : public Renderable {
 public:
  Model(string path, glm::mat4 transform = glm::mat4());

  void Render(Uniforms uniforms, glm::mat4 parentTransform);
  void BindToShader(GLuint shaderProgram);

  float GetSize();
  Bounds GetBounds();
  void SetTransform(glm::mat4 transform);

 private:
  vector<shared_ptr<Mesh>> meshes;
  Bounds bounds;
  glm::mat4 transform;
};
