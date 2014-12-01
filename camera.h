class Camera {
 public:
  Camera();
  ~Camera();

  void MouseDown();
  void MouseUp();
  void MouseMove(int x, int y);

  void BindToShader(GLuint shaderProgram);
  
 private:
  int lastX;
  int lastY;

  glm::mat4 viewTrans;
  glm::mat4 projTrans;
}
