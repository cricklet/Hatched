#include "helper.h"
#include "uniforms.h"
#include "fbo.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLEW_STATIC
#include <GL/glew.h>

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

using namespace std;

#ifndef RENDERER_H_
#define RENDERER_H_

typedef function<void(UniformGetter)> RenderScene;
typedef function<void(GLuint)> BindScene;
struct Renderer {
  vector<string> sources;
  long int loadTime;
  GLuint sceneShader;

  function<void(RenderScene)> render;
  function<void(void)> cleanup;
};

Renderer generateHatchedRenderer();
Renderer generateDeferredRenderer();
Renderer generateSSAORenderer();
bool shouldUpdateRenderer(Renderer &r);

#endif
