#include "scene.h"

Scene::Scene() {
};

void Scene::Add(
    shared_ptr<Renderable> obj,
    glm::mat4 transform,
    vector<string> flags) {
  objects.push_back(obj);
  objectTransforms.push_back(transform);
  objectFlags.push_back(flags);

  sceneTransform = glm::mat4();
}

static bool hasOverlap(auto v1, auto v2) {
  for (auto x : v1)
    for (auto y : v2)
      if (x == y)
        return true;

  return false;
}

void Scene::Render(Uniforms uniforms, vector<string> flags) {
  for (int i = 0; i < objects.size(); i ++) {
    bool shouldRender = hasOverlap(flags, objectFlags[i]);
    if (!shouldRender) continue;

    glm::mat4 transform = sceneTransform * objectTransforms[i];
    objects[i]->Render(uniforms, transform);
  }
}

void Scene::BindToShader(GLuint shader, vector<string> flags) {
  for (int i = 0; i < objects.size(); i ++) {
    bool shouldBind = hasOverlap(flags, objectFlags[i]);
    if (!shouldBind) continue;

    objects[i]->BindToShader(shader);
  }
}
