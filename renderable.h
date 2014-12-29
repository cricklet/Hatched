#include "uniforms.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifndef RENDERABLE_H_
#define RENDERABLE_H_

class Renderable {
public:
  virtual void Render(Uniforms uniforms, glm::mat4 parentTransform) = 0;
  virtual void BindToShader(GLuint shader) = 0;
  virtual ~Renderable() {};
};

#endif
