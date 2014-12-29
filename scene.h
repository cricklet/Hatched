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

class Scene {
public:
  Scene();
  void AddObject(shared_ptr<Renderable> child, glm::mat4 transform, vector<string> flags);
  void AddLight(glm::vec3 position);
  void Render(Uniforms uniforms, vector<string> flags);
  void Light(Uniforms uniforms);
  void BindToShader(GLuint shader, vector<string> flags);

private:
  vector<SceneObject> objects;
  vector<glm::vec3> lightPositions;
  vector<glm::vec3> lightConstants;
  vector<glm::vec3> lightColors;
  glm::mat4 sceneTransform;
};

#endif
