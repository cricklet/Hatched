#include <string>
#include <vector>
#include <fstream>
#include <streambuf>

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

using namespace std;

int main(int argv, char *argc[]) {
  SDL_Init(SDL_INIT_VIDEO);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

  SDL_Window *window = SDL_CreateWindow("OpenGL", 100, 100, 800, 600, SDL_WINDOW_OPENGL);
  SDL_GLContext context = SDL_GL_CreateContext(window);

  glewExperimental = GL_TRUE; // necessary for modern opengl calls
  glewInit();
  checkErrors();

  Camera *camera = new Camera();

  Model *model = new Model("nanosuit/nanosuit2.obj");
  checkErrors();

  vector<string> vertSources;
  vector<string> fragSources;

  vertSources.push_back("simple.vert");
  fragSources.push_back("simple.frag");

  vertSources.push_back("dir_light.vert");
  fragSources.push_back("dir_light.frag");

  vertSources.push_back("dir_light.vert");
  fragSources.push_back("hatched.frag");

  GLuint shaderPrograms[3];

  Uniforms shaderUniforms[3];

  int numShaders = 3;
  int shaderIndex = 0;

  for (int i = 0; i < numShaders; i++) {
    GLuint shaderProgram = generateShaderProgram(vertSources[i], fragSources[i]);
    checkErrors();

    glBindFragDataLocation(shaderProgram, 0, "outFragColor");
    glUseProgram(shaderProgram);
    checkErrors();

    setupUniforms(shaderUniforms[i], shaderProgram);

    model->BindToShader(shaderProgram);

    shaderPrograms[i] = shaderProgram;
  }

  glm::mat4 viewTrans = glm::lookAt(
      glm::vec3(3.0f, 0.0f, 1.0f), // location of camera
      glm::vec3(0, 0, 0), // look at
      glm::vec3(0, 0, 1)  // camera up vector
  );

  glm::mat4 projTrans = glm::perspective(
      45.0f, // fov y
      800.0f / 600.0f, // aspect
      0.2f,  // near
      10.0f  //far
  );

  string gridSource = "grid.png";
  int gridIndex = nextTextureIndex();
  GLuint gridTexture = loadTexture(gridSource, gridIndex);
  checkErrors();

  string tilesSource = "tiled_hatches.png";
  int tilesNum = 6;
  int tilesIndex = nextTextureIndex();
  GLuint tilesTexture = loadTexture(tilesSource, tilesIndex);
  checkErrors();

  struct timeval t;
  gettimeofday(&t, NULL);
  long int startTime = t.tv_sec * 1000 + t.tv_usec / 1000;

  bool quit = false;

  SDL_Event windowEvent;
  while (quit == false) {
    while (SDL_PollEvent(&windowEvent)) {
      if (windowEvent.type == SDL_QUIT) {
        quit = true;
      }
      if (windowEvent.type == SDL_KEYDOWN) {
        if (windowEvent.key.keysym.sym == SDLK_ESCAPE) {
          quit = true;
        }
        if (windowEvent.key.keysym.sym == SDLK_s) {
          shaderIndex = (shaderIndex + 1) % numShaders;
        }
      }

      camera->HandleEvent(windowEvent);
    }

    gettimeofday(&t, NULL);
    long int currentTime = t.tv_sec * 1000 + t.tv_usec / 1000;
    float time = (float) (currentTime - startTime) / 1000.0f;

    Uniforms u = shaderUniforms[shaderIndex];

    // Render cube
    glUseProgram(shaderPrograms[shaderIndex]);
    glEnable(GL_DEPTH_TEST);
    checkErrors();

    // Camera setup
    camera->SetupTransforms(shaderUniforms[shaderIndex].viewTrans, shaderUniforms[shaderIndex].projTrans);
    checkErrors();

    // Lighting setup
    glm::vec3 lightDir = glm::vec3(-1,-1,-1);
    lightDir = glm::rotate(lightDir, time, glm::vec3(0,0,1));
    if (u.lightDir != -1) glUniform3fv(u.lightDir, 1, glm::value_ptr(lightDir));

    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    checkErrors();

    // render model
    if (u.useTexture != -1) glUniform1i(u.useTexture, 1);
    if (u.texture != -1) glUniform1i(u.texture, gridIndex);
    if (u.numTiles != -1) glUniform1i(u.numTiles, tilesNum);
    if (u.tilesTexture != -1) glUniform1i(u.tilesTexture, tilesIndex);
    checkErrors();

    model->Render(shaderUniforms[shaderIndex]);

    SDL_GL_SwapWindow(window);
    checkErrors();
  }

  SDL_GL_DeleteContext(context);
  SDL_Quit();
  return 0;
}
