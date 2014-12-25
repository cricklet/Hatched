#include "uniforms.h"

void
Uniforms::add(GLint shaderProgram, vector<string> keys) {
  for (string key : keys) {
    GLint val = glGetUniformLocation(shaderProgram, key.c_str());
    uniforms[key] = val;
  }
}

GLuint
Uniforms::get(string key) const {
  auto valIter = uniforms.find(key);
  if (valIter == uniforms.end()) {
    return -1;
  } else {
    return uniforms.find(key)->second;
  }
}

void
Uniforms::print() const {
  for (pair<string, GLint> pair: uniforms) {
    cout << "  " << pair.first << ": " << pair.second << "\n";
  }
}
