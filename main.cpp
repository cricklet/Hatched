#include <string>
#include <vector>
#include <fstream>
#include <streambuf>
#include <functional>

#include <iostream>
#include <stdexcept>

#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>

#define GLEW_STATIC
#include <GL/glew.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "helper.h"
#include "uniforms.h"
#include "camera.h"
#include "mesh.h"
#include "model.h"
#include "fbo.h"

using namespace std;

static int WIDTH = 800, HEIGHT = 600;

static long int getTimeOfDay() {
  struct timeval t;
  gettimeofday(&t, NULL);
  return t.tv_sec * 1000 + t.tv_usec / 1000;
}

static long int getModifiedTime(string source) {
  struct stat attrib;
  stat(source.c_str(), &attrib);
  return attrib.st_mtime;
}

static long int getModifiedTime(vector<string> sources) {
  long int latestTime = 0;
  for (string source : sources) {
    long int time = getModifiedTime(source);
    if (time > latestTime) {
      latestTime = time;
    }
  }

  return latestTime;
}

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

typedef function<void (UniformGetter)> RenderScene;
typedef function<void (GLuint)> BindScene;
struct Renderer {
  vector<string> sources;
  long int loadTime;
  function<void (RenderScene)> render;
  GLuint sceneShader;
};

static Renderer generateSimpleRenderer() {
  GLuint shader = loadShader("simple.vert", "simple.frag");

  UniformsMap uniformsMap;
  addUniforms(uniformsMap, shader, TRANS_UNIFORMS);
  addUniforms(uniformsMap, shader, COLOR);
  UniformGetter uniforms = generateUniformGetter(uniformsMap);

  Renderer r;
  r.sources = {"simple.vert", "simple.frag"};
  r.sceneShader = shader;
  r.loadTime = getModifiedTime(r.sources);
  r.render = [=] (RenderScene renderScene) {
    glUseProgram(shader);

    clearActiveBuffer();
    checkErrors();

    // render scene (includes setting up camera)
    renderScene(uniforms);
    checkErrors();
  };

  return r;
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

static bool shouldUpdateRenderer(Renderer &r) {
  if (getModifiedTime(r.sources) > r.loadTime) {
    return true;
  }
  return false;
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

  auto bindScene = [&] (GLuint s) {
    model->BindToShader(s);
  };

  vector<Renderer> renderers = {
    generateSimpleRenderer()
  };

  for (Renderer r : renderers) {
    bindScene(r.sceneShader);
  }

  int rendererIndex = 0;

  long int startTime = getTimeOfDay();
  long int lastTime = startTime;

  float timeTillUpdateRenderer = 1.0f;

  bool quit = false;

  SDL_Event windowEvent;
  while (quit == false) {
    long int currentTime = getTimeOfDay();
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

    Renderer &r = renderers[rendererIndex];

    timeTillUpdateRenderer -= dt;
    if (timeTillUpdateRenderer < 0) {
      if (shouldUpdateRenderer(r)) {
        cout << "Attempting to reload renderer\n";

        r = generateSimpleRenderer();
        bindScene(r.sceneShader);

        cout << "Loaded new renderer\n";
      }
      timeTillUpdateRenderer = 1.0f;
    }

    camera->Think(dt);

    r.render(renderScene);

    SDL_GL_SwapWindow(window);
    checkErrors();

    lastTime = currentTime;
  }

  SDL_GL_DeleteContext(context);
  SDL_Quit();
}

int main(int argc, char *argv[]) {
  sdlMain();
}
