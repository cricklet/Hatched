#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera {
 public:
  Camera();
  ~Camera();

 private:
  void Update();

  glm::vec3 location;
  glm::vec3 lookAt;
  glm::vec3 up;

  glm::mat4 viewTrans;
  glm::mat4 projTrans;
}
