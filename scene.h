#include "renderable.h"
#include "lights.h"

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

class Scene {
public:
  Scene();
  void AddObject(shared_ptr<Renderable> child, glm::mat4 transform, vector<string> flags);
  void Render(Uniforms uniforms, vector<string> flags);
  void BindToShader(GLuint shader, vector<string> flags);

private:
  vector<SceneObject> objects;
  glm::mat4 sceneTransform;
};

#endif
