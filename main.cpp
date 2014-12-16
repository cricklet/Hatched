#include <string>
#include <vector>
#include <fstream>
#include <streambuf>
#include <functional>

#include <iostream>
#include <stdexcept>

#include <sys/time.h>

#define GLEW_STATIC
#include <GL/glew.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <QtGui/QApplication>
#include <QtGui/QDialog>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMessageBox>
#include <QtGui/QFormLayout>
#include <QtCore/QThread>
#include <QtCore/QThreadPool>
#include <QtCore/QRunnable>

#include "helper.h"
#include "uniforms.h"
#include "camera.h"
#include "mesh.h"
#include "model.h"
#include "fbo.h"

using namespace std;

static int WIDTH = 1024, HEIGHT = 768;

static void clearActiveBuffer(float rgb = 1, float a = 1) {
  glEnable(GL_DEPTH_TEST);
  glClearColor(rgb, rgb, rgb, a);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  checkErrors();
}

static GLuint loadShader(string vertSource, string fragSource) {
  GLuint shaderProgram = generateShaderProgram(vertSource, fragSource);
  checkErrors();

  glBindFragDataLocation(shaderProgram, 0, "outFragColor");
  checkErrors();

  return shaderProgram;
}

typedef function<void(float)> Renderer;

static Renderer generateSimpleRenderer(
    function<void(GLuint)> bindShader,
    function<void(UniformGetter)> renderScene
    ) {
  GLuint shader = loadShader("simple.vert", "simple.frag");
  UniformsMap uniformsMap;
  addUniforms(uniformsMap, shader, TRANS_UNIFORMS);
  addUniforms(uniformsMap, shader, COLOR);
  UniformGetter uniforms = generateUniformGetter(uniformsMap);

  bindShader(shader);

  return [=] (float time) {
    glUseProgram(shader);

    clearActiveBuffer();
    checkErrors();

    // render scene (includes setting up camera)
    renderScene(uniforms);
    checkErrors();
  };
}

static Renderer generateDirLightRenderer(
    function<void(GLuint)> bindShader,
    function<void(UniformGetter)> renderScene
    ) {
  GLuint shader = loadShader("dir_light.vert", "dir_light.frag");
  UniformsMap uniformsMap;
  addUniforms(uniformsMap, shader, TRANS_UNIFORMS);
  addUniforms(uniformsMap, shader, COLOR);
  addUniforms(uniformsMap, shader, TEXTURE_UNIFORMS);
  addUniforms(uniformsMap, shader, LIGHT_DIR);
  UniformGetter uniforms = generateUniformGetter(uniformsMap);

  string gridSource = "grid.png";
  int gridIndex = nextTextureIndex();
  GLuint gridTexture = loadTexture(gridSource, gridIndex);
  checkErrors();

  bindShader(shader);

  return [=] (float time) {
    glUseProgram(shader);

    clearActiveBuffer();
    checkErrors();

    // setup lighting
    glm::vec3 lightDir = glm::vec3(-1,-1,-1);
    lightDir = glm::rotate(lightDir, time, glm::vec3(0,0,1));
    glUniform3fv(uniforms(LIGHT_DIR), 1, glm::value_ptr(lightDir));
    checkErrors();

    // render a test texture
    glUniform1i(uniforms(USE_TEXTURE), 1);
    glUniform1i(uniforms(TEXTURE), gridIndex);
    checkErrors();

    // render scene (includes setting up camera)
    renderScene(uniforms);
    checkErrors();
  };
}

static Renderer generateHatchRenderer(
    function<void(GLuint)> bindShader,
    function<void(UniformGetter)> renderScene
    ) {
  GLuint shader = loadShader("dir_light.vert", "hatched.frag");
  UniformsMap uniformsMap;
  addUniforms(uniformsMap, shader, TRANS_UNIFORMS);
  addUniforms(uniformsMap, shader, COLOR);
  addUniforms(uniformsMap, shader, TILE_UNIFORMS);
  addUniforms(uniformsMap, shader, LIGHT_DIR);
  UniformGetter uniforms = generateUniformGetter(uniformsMap);

  string tilesSource = "tiled_hatches.png";
  int tilesNum = 6;
  int tilesIndex = nextTextureIndex();
  GLuint tilesTexture = loadTexture(tilesSource, tilesIndex);
  checkErrors();

  bindShader(shader);

  return [=] (float time) {
    glUseProgram(shader);
    clearActiveBuffer();
    checkErrors();

    // setup lighting
    glm::vec3 lightDir = glm::vec3(-1,-1,-1);
    lightDir = glm::rotate(lightDir, time, glm::vec3(0,0,1));
    glUniform3fv(uniforms(LIGHT_DIR), 1, glm::value_ptr(lightDir));

    // render hatched tiles texture
    glUniform1i(uniforms(NUM_TILES), tilesNum);
    glUniform1i(uniforms(TILES_TEXTURE), tilesIndex);

    // render scene (includes setting up camera)
    renderScene(uniforms);
    checkErrors();
  };
}

static Renderer generateSSAORenderer(
    function<void(GLuint)> bindShader,
    function<void(UniformGetter)> renderScene
    ) {
  GLuint normalsShader = loadShader("normals.vert", "normals.frag");
  UniformsMap normalsUniformsMap;
  addUniforms(normalsUniformsMap, normalsShader, TRANS_UNIFORMS);
  UniformGetter normalsUniforms = generateUniformGetter(normalsUniformsMap);

  GLuint frameShader = loadShader("render_buffer.vert", "ssao.frag");
  UniformsMap frameUniformsMap;
  addUniforms(frameUniformsMap, frameShader, BUFFER);
  addUniforms(frameUniformsMap, frameShader, RANDOM);
  UniformGetter frameUniforms = generateUniformGetter(frameUniformsMap);
  checkErrors();

  cout << "SSAO Renderer:\n";
  printUniforms(frameUniformsMap);

  int noiseIndex = nextTextureIndex();
  GLuint noiseTexture = loadTexture("noise.png", noiseIndex);

  FBO *normalsFBO = new FBO(WIDTH, HEIGHT, GL_RGBA);
  normalsFBO->BindToShader(frameShader);
  checkErrors();

  bindShader(normalsShader);
  checkErrors();

  return [=] (float time) {
    // draw to the frame buffer
    glUseProgram(normalsShader);
    glBindFramebuffer(GL_FRAMEBUFFER, normalsFBO->GetFrameBuffer());

    clearActiveBuffer(0,0);
    checkErrors();

    // render scene (includes setting up camera)
    renderScene(normalsUniforms);
    checkErrors();

    // draw the frame buffer to the screen
    glUseProgram(frameShader);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    clearActiveBuffer();

    glUniform1i(frameUniforms(RANDOM), noiseIndex);
    glUniform1i(frameUniforms(BUFFER), normalsFBO->GetTextureIndex());
    checkErrors();

    normalsFBO->Render();
  };
}

static Model *loadNanosuit() {
  Model *model = new Model("models/nanosuit/nanosuit2.obj");
  glm::mat4 modelTrans = glm::mat4();

  modelTrans = glm::translate(modelTrans, glm::vec3(0, 0, -0.5));
  modelTrans = glm::rotate(modelTrans, (float) (M_PI / 2.0), glm::vec3(0, 0, 1));
  modelTrans = glm::rotate(modelTrans, (float) (M_PI / 2.0), glm::vec3(1, 0, 0));
  float scale = 1.0f / model->GetSize();
  modelTrans = glm::scale(modelTrans, glm::vec3(scale, scale, scale));

  model->SetTransform(modelTrans);

  return model;
}

static Model *loadCharacter() {
  Model *model = new Model("models/minion/minion.obj");
  glm::mat4 modelTrans = glm::mat4();

  modelTrans = glm::translate(modelTrans, glm::vec3(0, 0, -0.5));
  modelTrans = glm::rotate(modelTrans, (float) (M_PI / 2.0), glm::vec3(0, 0, 1));
  modelTrans = glm::rotate(modelTrans, (float) (M_PI / 2.0), glm::vec3(1, 0, 0));
  float scale = 1.0f / model->GetSize();
  modelTrans = glm::scale(modelTrans, glm::vec3(scale, scale, scale));

  model->SetTransform(modelTrans);

  return model;
}

static Model *loadHouse() {
  Model *model = new Model("models/house/House01.obj");
  glm::mat4 modelTrans = glm::mat4();

  modelTrans = glm::rotate(modelTrans, (float) (M_PI / 2.0), glm::vec3(0, 0, 1));
  modelTrans = glm::rotate(modelTrans, (float) (M_PI / 2.0), glm::vec3(1, 0, 0));
  float scale = 1.0f / 10;
  modelTrans = glm::scale(modelTrans, glm::vec3(scale, scale, scale));

  model->SetTransform(modelTrans);

  return model;
}

int sdlMain() {
  SDL_Init(SDL_INIT_VIDEO);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

  SDL_Window *window = SDL_CreateWindow("OpenGL", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
  SDL_GLContext context = SDL_GL_CreateContext(window);

  glewExperimental = GL_TRUE; // necessary for modern opengl calls
  glewInit();
  checkErrors();

  //  Camera *camera = new FPSCamera();
  Camera *camera = new RotationCamera();

  //  Model *model = loadNanosuit();
  //  Model *model = loadHouse();
  Model *model = loadCharacter();

  glm::mat4 viewTrans = glm::lookAt(
      glm::vec3(3.0f, 0.0f, 1.0f), // location of camera
      glm::vec3(0, 0, 0), // look at
      glm::vec3(0, 0, 1)  // camera up vector
      );

  glm::mat4 projTrans = glm::perspective(
      45.0f, // fov y
      (float) WIDTH / HEIGHT, // aspect
      0.2f,  // near
      10.0f  //far
      );

  auto renderScene = [&] (UniformGetter u) {
    camera->SetupTransforms(u(VIEW_TRANS), u(PROJ_TRANS));
    checkErrors();

    model->Render(u);
    checkErrors();
  };

  auto bindShader = [&] (GLuint s) {
    model->BindToShader(s);
  };

  vector<Renderer> renderers = {
      generateSimpleRenderer(bindShader, renderScene),
      generateDirLightRenderer(bindShader, renderScene),
      generateHatchRenderer(bindShader, renderScene),
      generateSSAORenderer(bindShader, renderScene),
  };

  int rendererIndex = 0;

  struct timeval t;
  gettimeofday(&t, NULL);
  long int startTime = t.tv_sec * 1000 + t.tv_usec / 1000;
  long int lastTime = startTime;

  bool quit = false;

  SDL_Event windowEvent;
  while (quit == false) {
    gettimeofday(&t, NULL);
    long int currentTime = t.tv_sec * 1000 + t.tv_usec / 1000;
    float time = (float) (currentTime - startTime) / 1000.0f;
    float dt = (float) (currentTime - lastTime) / 1000.0f;

    while (SDL_PollEvent(&windowEvent)) {
      if (windowEvent.type == SDL_QUIT) {
        quit = true;
      }
      if (windowEvent.type == SDL_KEYDOWN) {
        switch (windowEvent.key.keysym.sym) {
          case SDLK_ESCAPE:
            quit = true;
            break;
          case SDLK_r:
            rendererIndex++;
            rendererIndex %= renderers.size();
            break;
        }
      }
      camera->HandleEvent(windowEvent);
    }

    camera->Think(dt);

    renderers[rendererIndex](time);

    SDL_GL_SwapWindow(window);
    checkErrors();

    lastTime = currentTime;
  }

  SDL_GL_DeleteContext(context);
  SDL_Quit();
}

struct UniformEditor {
  QLabel *label;
  QLineEdit *edit;
};

class UniformsDialog : public QDialog {
public:
  UniformsDialog(QWidget *parent = 0);
private:
  vector<UniformEditor> editors;
};

static UniformEditor createUniformEditor(string label, string def) {
  UniformEditor editor;

  editor.label = new QLabel("Set:");
  editor.edit = new QLineEdit;

  editor.label->setBuddy(editor.edit);

  return editor;
}

UniformsDialog::UniformsDialog(QWidget *parent): QDialog(parent) {
  QFormLayout *layout = new QFormLayout;

  for (int i = 0; i < 20; i ++) {
    UniformEditor e = createUniformEditor("Set 1:", "0");
    editors.push_back(e);
    layout->addRow(e.label, e.edit);
  }

  setLayout(layout);
}

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  UniformsDialog d;
  d.setModal(false);
  d.setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint);
  d.show();

  sdlMain();

  return app.exec();
}
