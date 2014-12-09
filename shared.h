#ifndef SHARED_H
#define SHARED_H

#define GLEW_STATIC
#include <GL/glew.h>

struct Uniforms {
  GLint modelTrans;
  GLint viewTrans;
  GLint projTrans;
  GLint color;
  GLint texture;
};

static void setupUniforms(Uniforms &uniforms, GLint shaderProgram) {
  uniforms.modelTrans = glGetUniformLocation(shaderProgram, "unifModelTrans");
  uniforms.viewTrans = glGetUniformLocation(shaderProgram, "unifViewTrans");
  uniforms.projTrans = glGetUniformLocation(shaderProgram, "unifProjTrans");
  uniforms.color = glGetUniformLocation(shaderProgram, "unifColor");
  uniforms.texture = glGetUniformLocation(shaderProgram, "unifTexture");
}

#endif
