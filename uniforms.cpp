#include "uniforms.h"

void addUniforms(UniformsMap &uniforms, GLint shaderProgram, vector<string> keys) {
  for (string key : keys) {
    GLint val = glGetUniformLocation(shaderProgram, key.c_str());
    uniforms[key] = val;
  }
}

void addUniforms(UniformsMap &uniforms, GLint shaderProgram, string key) {
  GLint val = glGetUniformLocation(shaderProgram, key.c_str());
  uniforms[key] = val;
}

UniformGetter generateUniformGetter(const UniformsMap &uniforms) {
  return [=] (string key) {
    auto valIter = uniforms.find(key);
    if (valIter == uniforms.end()) {
      return -1;
    } else {
      return uniforms.find(key)->second;
    }
  };
}

void printUniforms(UniformsMap &uniforms) {
  for (pair<string, GLint> pair: uniforms) {
    cout << "  " << pair.first << ": " << pair.second << "\n";
  }
}
