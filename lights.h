#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>

#ifndef LIGHTS_H_
#define LIGHTS_H_

// if i want to extend this, first turn it into a virtual abstract class!
class Lights {
public:
  glm::vec3 getPosition(int i) {
    return positions[i];
  };
  GLfloat *getPositions() {
    return reinterpret_cast<GLfloat *>(positions.data());
  };
  GLfloat *getConstants() {
    return reinterpret_cast<GLfloat *>(constants.data());
  };
  GLfloat *getColors() {
    return reinterpret_cast<GLfloat *>(colors.data());
  };
  int num() {
    return positions.size();
  };
  void add(glm::vec3 position, glm::vec3 constant, glm::vec3 color) {
    positions.push_back(position);
    constants.push_back(constant);
    colors.push_back(color);
  };

private:
  vector<glm::vec3> positions;
  vector<glm::vec3> constants;
  vector<glm::vec3> colors;
};

#endif
