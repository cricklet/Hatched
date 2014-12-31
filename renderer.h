#include "helper.h"
#include "uniforms.h"
#include "fbo.h"
#include "lights.h"

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

typedef function<void(Uniforms)> SetupScene;
typedef function<Lights& (void)> GetLights;
typedef function<void(Uniforms)> RenderScene;
typedef function<void(GLuint)> BindScene;

class Renderer {
public:
  Renderer(
      vector<string> s,
      function<void(SetupScene, GetLights, RenderScene)> r,
      long int t
  );
  bool ShouldUpdate();
  function<void(SetupScene, GetLights, RenderScene)> Render;

private:
  vector<string> sources;
  long int loadTime;
};

//Renderer generateHatchedRenderer(BindScene bindScene);
Renderer generateSimpleRenderer(BindScene bindScene);
//Renderer generateDeferredRenderer();
Renderer generateSSAORenderer(BindScene bindScene);
Renderer generateNULLRenderer();

#endif
