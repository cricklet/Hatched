#include "renderer.h"
#include "fbo.h"
#include "textures.h"

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

static GLuint loadShader(string vertSource, string fragSource) {
  GLuint shaderProgram = generateShaderProgram(vertSource, fragSource);
  checkErrors();

  glBindFragDataLocation(shaderProgram, 0, "outFragColor");
  checkErrors();

  return shaderProgram;
}

static void clearActiveBuffer(
    float r = 1,
    float g = 1,
    float b = 1,
    float a = 1,
    GLuint flags = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT
) {
  glClearColor(r, g, b, a);
  glClear(flags);
  checkErrors();
}

bool shouldUpdateRenderer(Renderer &r) {
  if (getModifiedTime(r.sources) > r.loadTime) {
    return true;
  }
  return false;
}

Renderer generateSimpleRenderer() {
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

    glEnable(GL_DEPTH_TEST);
    clearActiveBuffer();
    checkErrors();

    // render scene (includes setting up camera)
    renderScene(uniforms);
    checkErrors();
  };

  return r;
}

Renderer generateDirLightRenderer() {
  GLuint shader = loadShader("dir_light.vert", "dir_light.frag");

  UniformsMap uniformsMap;
  addUniforms(uniformsMap, shader, TRANS_UNIFORMS);
  addUniforms(uniformsMap, shader, COLOR);
  addUniforms(uniformsMap, shader, TEXTURE_UNIFORMS);
  addUniforms(uniformsMap, shader, LIGHT_DIR);
  UniformGetter uniforms = generateUniformGetter(uniformsMap);

  Texture gridTexture = createTexture("grid.png");
  checkErrors();

  Renderer r;
  r.sources = {"dir_light.vert", "dir_light.frag"};
  r.sceneShader = shader;
  r.loadTime = getModifiedTime(r.sources);
  r.render = [=] (RenderScene renderScene) {
    glUseProgram(shader);

    glEnable(GL_DEPTH_TEST);
    clearActiveBuffer();
    checkErrors();

    // setup lighting
    glm::vec3 lightDir = glm::vec3(-1,-1,-1);
    //lightDir = glm::rotate(lightDir, t, glm::vec3(0,0,1));
    glUniform3fv(uniforms(LIGHT_DIR), 1, glm::value_ptr(lightDir));
    checkErrors();

    // render a test texture
    glUniform1i(uniforms(USE_TEXTURE), 1);
    glUniform1i(uniforms(TEXTURE), gridTexture.index);
    checkErrors();

    // render scene (includes setting up camera)
    renderScene(uniforms);
    checkErrors();
  };

  return r;
}

Renderer generateHatchedRenderer() {
  GLuint shader = loadShader("dir_light.vert", "hatched.frag");
  UniformsMap uniformsMap;
  addUniforms(uniformsMap, shader, TRANS_UNIFORMS);
  addUniforms(uniformsMap, shader, COLOR);
  addUniforms(uniformsMap, shader, TILE_UNIFORMS);
  addUniforms(uniformsMap, shader, LIGHT_DIR);
  UniformGetter uniforms = generateUniformGetter(uniformsMap);

  int tilesNum = 6;
  Texture tilesTexture = createTexture("tiled_hatches.png");
  checkErrors();

  Renderer r;
  r.sources = {"dir_light.vert", "hatched.frag"};
  r.sceneShader = shader;
  r.loadTime = getModifiedTime(r.sources);
  r.render = [=] (RenderScene renderScene) {
    glUseProgram(shader);
    glEnable(GL_DEPTH_TEST);
    clearActiveBuffer();
    checkErrors();

    // setup lighting
    glm::vec3 lightDir = glm::vec3(-1,-1,-1);
    // lightDir = glm::rotate(lightDir, time, glm::vec3(0,0,1));
    glUniform3fv(uniforms(LIGHT_DIR), 1, glm::value_ptr(lightDir));

    // render hatched tiles texture
    glUniform1i(uniforms(NUM_TILES), tilesNum);
    glUniform1i(uniforms(TILES_TEXTURE), tilesTexture.index);

    // render scene (includes setting up camera)
    renderScene(uniforms);
    checkErrors();
  };

  return r;
}

Renderer generateSSAORenderer() {
  GLuint normalsShader = loadShader("normals.vert", "normals.frag");
  UniformsMap normalsUniformsMap;
  addUniforms(normalsUniformsMap, normalsShader, TRANS_UNIFORMS);
  UniformGetter normalsUniforms = generateUniformGetter(normalsUniformsMap);

  GLuint frameShader = loadShader("render_buffer.vert", "ssao.frag");
  UniformsMap frameUniformsMap;
  addUniforms(frameUniformsMap, frameShader, DEPTHS);
  addUniforms(frameUniformsMap, frameShader, NORMALS);
  addUniforms(frameUniformsMap, frameShader, RANDOM);
  UniformGetter frameUniforms = generateUniformGetter(frameUniformsMap);
  checkErrors();

  Texture noiseTexture = createTexture("normal_noise.png");
  checkErrors();

  FBO *normalsFBO = new FBO(WIDTH, HEIGHT);
  normalsFBO->BindToShader(frameShader);
  checkErrors();

  Renderer r;
  r.sources = {"normals.vert", "normals.frag", "render_buffer.vert", "ssao.frag"};
  r.sceneShader = normalsShader;
  r.loadTime = getModifiedTime(r.sources);
  r.render = [=] (RenderScene renderScene) {
    // draw to the frame buffer
    glUseProgram(normalsShader);
    glBindFramebuffer(GL_FRAMEBUFFER, normalsFBO->GetFrameBuffer());
    glEnable(GL_DEPTH_TEST);
    clearActiveBuffer(0,0,0,0, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    checkErrors();

    // render scene (includes setting up camera)
    renderScene(normalsUniforms);
    checkErrors();

    // draw the frame buffer to the screen
    glUseProgram(frameShader);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    clearActiveBuffer(0,0,0,0, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUniform1i(frameUniforms(RANDOM), noiseTexture.index);
    glUniform1i(frameUniforms(NORMALS), normalsFBO->GetScreenTextureIndex());
    glUniform1i(frameUniforms(DEPTHS), normalsFBO->GetDepthTextureIndex());
    checkErrors();

    normalsFBO->Render();
  };

  return r;
}
