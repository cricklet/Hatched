#include "scene.h"

Scene::Scene() {
  sceneTransform = glm::mat4();
};

void Scene::AddObject(
    shared_ptr<Renderable> obj,
    glm::mat4 transform,
    vector<string> flags) {
  SceneObject o;
  o.renderable = obj;
  o.transform = transform;
  o.flags = flags;

  objects.push_back(o);
}

void Scene::AddLight(
    glm::vec3 position) {
  SceneLight l;
  l.position = position;

  lights.push_back(l);
}

static bool hasOverlap(auto v1, auto v2) {
  for (auto x : v1)
    for (auto y : v2)
      if (x == y)
        return true;

  return false;
}

void Scene::Render(Uniforms uniforms, vector<string> flags) {
  for (auto o : objects) {
    bool shouldRender = hasOverlap(flags, o.flags);
    if (!shouldRender) continue;

    glm::mat4 transform = sceneTransform * o.transform;
    o.renderable->Render(uniforms, transform);
  }
}

void Scene::BindToShader(GLuint shader, vector<string> flags) {
  for (auto o : objects) {
    bool shouldBind = hasOverlap(flags, o.flags);
    if (!shouldBind) continue;

    o.renderable->BindToShader(shader);
  }
}
