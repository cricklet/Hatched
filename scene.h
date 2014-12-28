#include "renderable.h"

#include <memory>
#include <vector>
using namespace std;

#ifndef SCENE_SCENE_H_
#define SCENE_SCENE_H_

class Scene {
public:
  Scene();
  void Add(shared_ptr<Renderable> child, glm::mat4 transform, vector<string> flags);
  void Render(Uniforms uniforms, vector<string> flags);
  void BindToShader(GLuint shader, vector<string> flags);

private:
  vector<shared_ptr<Renderable>> objects;
  vector<glm::mat4> objectTransforms;
  vector<vector<string>> objectFlags;

  glm::mat4 sceneTransform;
};

#endif
