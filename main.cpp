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
#include "cube.h"
#include "camera.h"

int main (int argv, char *argc[]) {
  SDL_Init(SDL_INIT_VIDEO);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

  SDL_Window *window = SDL_CreateWindow("OpenGL", 100, 100, 640, 480, SDL_WINDOW_OPENGL);
  SDL_GLContext context = SDL_GL_CreateContext(window);

  glewExperimental = GL_TRUE; // necessary for modern opengl calls
  glewInit();
  checkErrors();

  Camera *camera = new Camera();

  Cube *cube1 = new Cube("kitten.png");
  Cube *cube2 = new Cube("puppy.png");
  checkErrors();

  std::vector<std::string> vertSources;
  std::vector<std::string> fragSources;

  vertSources.push_back("simple.vert");
  vertSources.push_back("dir_light.vert");

  fragSources.push_back("simple.frag");
  fragSources.push_back("dir_light.frag");

  GLuint shaderPrograms[2];

  GLint modelTransUniforms[2];
  GLint viewTransUniforms[2];
  GLint projTransUniforms[2];

  int numShaders = 2;
  int shaderIndex = 0;

  for (int i = 0; i < numShaders; i ++) {
    GLuint shaderProgram = generateShaderProgram(vertSources[i].c_str(), fragSources[i].c_str());
    checkErrors();

    glBindFragDataLocation(shaderProgram, 0, "outFragColor");
    glUseProgram(shaderProgram);
    checkErrors();

    modelTransUniforms[i] = glGetUniformLocation(shaderProgram, "inVertModelTrans");
    viewTransUniforms[i] = glGetUniformLocation(shaderProgram, "inVertViewTrans");
    projTransUniforms[i] = glGetUniformLocation(shaderProgram, "inVertProjTrans");

    cube1->BindToShader(shaderProgram);
    cube2->BindToShader(shaderProgram);

    shaderPrograms[i] = shaderProgram;
  }

  glm::mat4 viewTrans = glm::lookAt(
    glm::vec3(3.0f, 0.0f, 1.0f), // location of camera
    glm::vec3(0,0,0), // look at
    glm::vec3(0,0,1)  // camera up vector
  );

  glm::mat4 projTrans = glm::perspective(
    45.0f, // fov y
    800.0f / 600.0f, // aspect
    0.2f,  // near
    10.0f  //far
  );

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

    // Camera setup
    camera->SetupTransforms(viewTransUniforms[shaderIndex], projTransUniforms[shaderIndex]);
    
    // Render cube
    glUseProgram(shaderPrograms[shaderIndex]);

    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glm::mat4 world1 = glm::mat4();
    glm::mat4 model1 = glm::rotate(world1, time, glm::vec3(0,0,1));

    glm::mat4 world2 = glm::translate(glm::mat4(), glm::vec3(-0.5f,-1.0f,0.5f));
    glm::mat4 model2 = glm::rotate(world2, time, glm::vec3(0,0,1));

    glUniformMatrix4fv(modelTransUniforms[shaderIndex],
		       1, GL_FALSE, glm::value_ptr(model1));
    cube1->Render();

    glUniformMatrix4fv(modelTransUniforms[shaderIndex],
		       1, GL_FALSE, glm::value_ptr(model2));
    cube2->Render();
    checkErrors();

    SDL_GL_SwapWindow(window);
    checkErrors();
  }

  SDL_GL_DeleteContext(context);
  SDL_Quit();
  return 0;
}
