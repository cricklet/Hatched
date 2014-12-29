#include "renderable.h"

#include <memory>
#include <vector>
using namespace std;

#ifndef SCENE_SCENE_H_
#define SCENE_SCENE_H_

struct SceneObject {
  shared_ptr<Renderable> renderable;
  glm::mat4 transform;
  vector<string> flags;
};

struct SceneLight {
  glm::vec3 position;
};

class Scene {
public:
  Scene();
  void AddObject(shared_ptr<Renderable> child, glm::mat4 transform, vector<string> flags);
  void AddLight(glm::vec3 position);
  void Render(Uniforms uniforms, vector<string> flags);
  void BindToShader(GLuint shader, vector<string> flags);

private:
  vector<SceneObject> objects;
  vector<SceneLight> lights;

  glm::mat4 sceneTransform;
};

#endif
