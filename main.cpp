#include <string>
#include <memory>
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
#include "renderer.h"
#include "fbo.h"
#include "scene.h"
#include "shapes.h"

using namespace std;

static long int getTimeOfDay() {
  struct timeval t;
  gettimeofday(&t, NULL);
  return t.tv_sec * 1000 + t.tv_usec / 1000;
}

static auto loadLights() {
  Lights l;
  l.add(glm::vec3(0,0,1.5), glm::vec3(1,1,0), glm::vec3(0.8,0.8,1.5));
  return l;
}

static auto loadNanosuit() {
  auto model = make_shared<Model>("models/nanosuit/nanosuit2.obj");
  glm::mat4 modelTrans = glm::mat4();

  modelTrans = glm::translate(modelTrans, glm::vec3(0, 0, -0.5));
  modelTrans = glm::rotate(modelTrans, (float) (M_PI / 2.0), glm::vec3(0, 0, 1));
  modelTrans = glm::rotate(modelTrans, (float) (M_PI / 2.0), glm::vec3(1, 0, 0));

  float scale = 1.0f / model->GetSize();
  modelTrans = glm::scale(modelTrans, glm::vec3(scale, scale, scale));

  model->SetTransform(modelTrans);

  return model;
}

static auto loadScene() {
  auto scene = make_shared<Scene>();

  vector<string> envFlags = {"env"};
  vector<string> objFlags = {"obj"};

  auto addShape = [&] (Shape s, glm::mat4 t) {
    for (int i = 0; i < s.meshes.size(); i ++) {
      scene->AddObject(s.meshes[i], t * s.transforms[i], objFlags);
    }
  };

  {
    auto nano = static_pointer_cast<Renderable>(loadNanosuit());
    auto nanoTrans = glm::translate(glm::mat4(), glm::vec3(-2, -2, -0.5));
    scene->AddObject(nano, nanoTrans, objFlags);
  }

  {
    auto box = generateBox(16,16,2, false, true);
    auto boxTrans = glm::translate(glm::mat4(), glm::vec3(0,0,0));
    addShape(box, boxTrans);
  }

  {
    auto box = generateBox(0.5,0.5,0.5);
    auto boxTrans = glm::mat4();
    boxTrans *= glm::translate(glm::mat4(), glm::vec3(3,0,-0.75));
    boxTrans *= glm::rotate(glm::mat4(), 0.5f, glm::vec3(0,0,1));
    addShape(box, boxTrans);
  }

  {
    auto box = generateBox(0.5,0.5,0.5);
    auto boxTrans = glm::mat4();
    boxTrans *= glm::translate(glm::mat4(), glm::vec3(0,1,-0.75));
    boxTrans *= glm::rotate(glm::mat4(), 0.5f, glm::vec3(0,0,1));
    addShape(box, boxTrans);
  }

  {
    auto box = generateBox(0.5,0.5,0.5);
    auto boxTrans = glm::mat4();
    boxTrans *= glm::translate(glm::mat4(), glm::vec3(0,-4,-0.75));
    boxTrans *= glm::rotate(glm::mat4(), 0.5f, glm::vec3(0,0,1));
    addShape(box, boxTrans);
  }

  {
    auto box = generateBox(0.5,0.5,0.5);
    auto boxTrans = glm::mat4();
    boxTrans *= glm::translate(glm::mat4(), glm::vec3(-2,0,-0.75));
    boxTrans *= glm::rotate(glm::mat4(), 1.0f, glm::vec3(0,0,1));
    addShape(box, boxTrans);
  }

  {
    auto box = generateBox(0.5,0.5,0.5);
    auto boxTrans = glm::mat4();
    boxTrans *= glm::translate(glm::mat4(), glm::vec3(-2.2,0.7,-0.75));
    boxTrans *= glm::rotate(glm::mat4(), 0.5f, glm::vec3(0,0,1));
    addShape(box, boxTrans);
  }

  {
    auto box = generateBox(0.5,0.5,0.5);
    auto boxTrans = glm::mat4();
    boxTrans *= glm::translate(glm::mat4(), glm::vec3(-2.7,0.2,-0.75));
    boxTrans *= glm::rotate(glm::mat4(), 0.1f, glm::vec3(0,0,1));
    addShape(box, boxTrans);
  }

  {
    auto box = generateBox(0.5,0.5,0.5);
    auto boxTrans = glm::mat4();
    boxTrans *= glm::translate(glm::mat4(), glm::vec3(-2.3,0.4,-0.25));
    boxTrans *= glm::rotate(glm::mat4(), 0.2f, glm::vec3(0,0,1));
    addShape(box, boxTrans);
  }

  {
    auto box = generateBox(2,3,1);
    auto boxTrans = glm::mat4();
    boxTrans *= glm::translate(glm::mat4(), glm::vec3(-3,5,-0.5));
    boxTrans *= glm::rotate(glm::mat4(), 0.3f, glm::vec3(0,0,1));
    addShape(box, boxTrans);
  }

  return scene;
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

  auto camera = FPSCamera(); //RotationCamera();
  auto scene = loadScene();
  auto lights = loadLights();
  //auto model = loadNanosuit();

  glm::mat4 globalTrans;

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

  SetupScene setupScene = [&] (Uniforms u) {
    camera.SetupTransforms(u);
    checkErrors();
  };

  GetLights getLights = [&] () -> Lights & {
    return lights;
  };

  RenderScene renderScene = [&] (Uniforms u) {
    //model->Render(u, globalTrans);
    scene->Render(u, {"env", "obj"});
  };

  BindScene bindScene = [&] (GLuint s) {
    //model->BindToShader(s);
    scene->BindToShader(s, {"env", "obj"});
  };

  typedef function<Renderer(BindScene)> Generator;
  vector<Generator> generators = {
      // generateDeferredRenderer,
      // generateHatchedRenderer,
      generateSimpleRenderer,
      generateSSAORenderer,
  };

  vector<Renderer> renderers;

  for (Generator &generator : generators) {
    Renderer r = generator(bindScene);
    renderers.push_back(r);
    checkErrors();
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
      camera.HandleEvent(windowEvent);
    }

    Renderer &renderer = renderers[rendererIndex];

    timeTillUpdateRenderer -= dt;
    if (timeTillUpdateRenderer < 0) {
      if (renderer.ShouldUpdate()) {
        cout << "Attempting to reload renderer\n";

        try {
          Generator &generator = generators[rendererIndex];
          renderer = generator(bindScene);

          cout << "Loaded new renderer\n";
        } catch (runtime_error &e) {
          cerr << "Couldn't load new renderer...\n";
        }
      }
      timeTillUpdateRenderer = 1.0f;
    }

    camera.Think(dt);
    checkErrors();

    renderer.Render(setupScene, getLights, renderScene);
    checkErrors();

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
