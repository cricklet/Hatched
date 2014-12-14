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

#include "helper.h"
#include "camera.h"
#include "mesh.h"
#include "model.h"
#include "fbo.h"

using namespace std;

static int WIDTH = 1024, HEIGHT = 768;

static void clearActiveBuffer() {
  glEnable(GL_DEPTH_TEST);
  glClearColor(1, 1, 1, 1);
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
    function<void(Uniforms)> renderScene
    ) {
  GLuint shader = loadShader("simple.vert", "simple.frag");
  Uniforms uniforms = getUniforms(shader);

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
    function<void(Uniforms)> renderScene
    ) {
  GLuint shader = loadShader("dir_light.vert", "dir_light.frag");
  Uniforms uniforms = getUniforms(shader);

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
    glUniform3fv(uniforms.lightDir, 1, glm::value_ptr(lightDir));

    // render a test texture
    glUniform1i(uniforms.useTexture, 1);
    glUniform1i(uniforms.texture, gridIndex);

    // render scene (includes setting up camera)
    renderScene(uniforms);
    checkErrors();
  };
}

static Renderer generateHatchRenderer(
    function<void(GLuint)> bindShader,
    function<void(Uniforms)> renderScene
    ) {
  GLuint shader = loadShader("dir_light.vert", "hatched.frag");
  Uniforms uniforms = getUniforms(shader);

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
    glUniform3fv(uniforms.lightDir, 1, glm::value_ptr(lightDir));

    // render hatched tiles texture
    glUniform1i(uniforms.numTiles, tilesNum);
    glUniform1i(uniforms.tilesTexture, tilesIndex);

    // render scene (includes setting up camera)
    renderScene(uniforms);
    checkErrors();
  };
}

static Renderer generateSSAORenderer(
    function<void(GLuint)> bindShader,
    function<void(Uniforms)> renderScene
    ) {
  GLuint renderShader = loadShader("normals.vert", "normals.frag");
  Uniforms renderUniforms = getUniforms(renderShader);
  GLuint frameShader = loadShader("render_buffer.vert", "render_buffer.frag");
  Uniforms frameUniforms = getUniforms(frameShader);
  checkErrors();

  int noiseIndex = nextTextureIndex();
  GLuint noiseTexture = loadTexture("noise.png", noiseIndex);

  FBO *fbo = new FBO(WIDTH, HEIGHT);
  fbo->BindToShader(frameShader);
  checkErrors();

  bindShader(renderShader);
  checkErrors();

  return [=] (float time) {
    // draw to the frame buffer
    glUseProgram(renderShader);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo->GetFrameBuffer());

    clearActiveBuffer();
    checkErrors();

    // render scene (includes setting up camera)
    renderScene(renderUniforms);
    checkErrors();

    // draw the frame buffer to the screen
    glUseProgram(frameShader);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    clearActiveBuffer();

    glUniform1i(frameUniforms.buffer, fbo->GetTextureIndex());

    fbo->Render();
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

int main(int argv, char *argc[]) {
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

  Camera *camera = new FPSCamera();
//  Camera *camera = new RotationCamera();

  Model *model = loadNanosuit();
  //Model *model = loadHouse();

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

  auto renderScene = [&] (Uniforms u) {
    camera->SetupTransforms(u.viewTrans, u.projTrans);
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
  return 0;
}
