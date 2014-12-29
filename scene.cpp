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

void Scene::AddLight(glm::vec3 position) {
  this->lightPositions.push_back(position);
  this->lightConstants.push_back(glm::vec3(1,1,1));
  this->lightColors.push_back(glm::vec3(0.5,1,0.5));
}

static bool hasOverlap(auto v1, auto v2) {
  for (auto x : v1)
    for (auto y : v2)
      if (x == y)
        return true;

  return false;
}

void Scene::Light(Uniforms uniforms) {
  auto posValues = reinterpret_cast<GLfloat *>(this->lightPositions.data());
  auto conValues = reinterpret_cast<GLfloat *>(this->lightConstants.data());
  auto colValues = reinterpret_cast<GLfloat *>(this->lightColors.data());

  int numLights = this->lightPositions.size();
  glUniform1i(uniforms.get(NUM_LIGHTS), numLights);
  glUniform3fv(uniforms.get(LIGHT_POSITIONS), numLights, posValues);
  glUniform3fv(uniforms.get(LIGHT_CONSTANTS), numLights, conValues);
  glUniform3fv(uniforms.get(LIGHT_COLORS),    numLights, colValues);
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
