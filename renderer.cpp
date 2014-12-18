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
Renderer generateHatchedRenderer() {
  GLuint bufferShader = generateShaderProgram("gbuffer.vert", "gbuffer.frag");
  checkErrors();

  UniformsMap bufferUniformsMap;
  addUniforms(bufferUniformsMap, bufferShader, TRANS_UNIFORMS);
  UniformGetter bufferUniforms = generateUniformGetter(bufferUniformsMap);

  GLuint hatchedShader = generateShaderProgram("render_buffer.vert", "deferred_hatched.frag");

  UniformsMap hatchedUniformsMap;
  addUniforms(hatchedUniformsMap, hatchedShader, POSITIONS);
  addUniforms(hatchedUniformsMap, hatchedShader, NORMALS);
  addUniforms(hatchedUniformsMap, hatchedShader, DEPTHS);
  addUniforms(hatchedUniformsMap, hatchedShader, UVS);
  addUniforms(hatchedUniformsMap, hatchedShader, LIGHT_DIR);
  addUniforms(hatchedUniformsMap, hatchedShader, TILE_UNIFORMS);
  UniformGetter hatchedUniforms = generateUniformGetter(hatchedUniformsMap);
  checkErrors();

  int tilesNum = 6;
  Texture tilesTexture = createTexture("tiled_hatches.png");
  checkErrors();

  FBO *fbo = FBOFactory("hatched_fbo", WIDTH, HEIGHT);
  fbo->BindToShader(hatchedShader);
  checkErrors();

  Renderer r;
  r.sources = {"gbuffer.vert", "gbuffer.frag", "render_buffer.vert", "deferred_dirlight.frag"};
  r.sceneShader = bufferShader;
  r.loadTime = getModifiedTime(r.sources);
  r.render = [=] (RenderScene renderScene) {
    // draw to the frame buffer
    glUseProgram(bufferShader);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo->GetFrameBuffer());
    glEnable(GL_DEPTH_TEST);
    clearActiveBuffer(0,0,0,0, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    checkErrors();

    // render scene (includes setting up camera)
    renderScene(bufferUniforms);
    checkErrors();

    // draw the frame buffer to the screen
    glUseProgram(hatchedShader);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    clearActiveBuffer(0,0,0,0, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // setup lighting
    glm::vec3 lightDir = glm::vec3(-1,-1,-1);
    glUniform3fv(hatchedUniforms(LIGHT_DIR), 1, glm::value_ptr(lightDir));

    glUniform1i(hatchedUniforms(NUM_TILES), tilesNum);
    glUniform1i(hatchedUniforms(TILES_TEXTURE), tilesTexture.index);

    glUniform1i(hatchedUniforms(POSITIONS), fbo->GetAttachment(0).index);
    glUniform1i(hatchedUniforms(NORMALS), fbo->GetAttachment(1).index);
    glUniform1i(hatchedUniforms(UVS), fbo->GetAttachment(2).index);
    glUniform1i(hatchedUniforms(DEPTHS), fbo->GetDepth().index);
    checkErrors();

    fbo->Render();
    checkErrors();
  };

  return r;
}

Renderer generateDeferredRenderer() {
  GLuint bufferShader = generateShaderProgram("gbuffer.vert", "gbuffer.frag");
  checkErrors();

  UniformsMap bufferUniformsMap;
  addUniforms(bufferUniformsMap, bufferShader, TRANS_UNIFORMS);
  UniformGetter bufferUniforms = generateUniformGetter(bufferUniformsMap);

  GLuint lightShader = generateShaderProgram("render_buffer.vert", "deferred_dirlight.frag");

  UniformsMap lightUniformsMap;
  addUniforms(lightUniformsMap, lightShader, POSITIONS);
  addUniforms(lightUniformsMap, lightShader, NORMALS);
  addUniforms(lightUniformsMap, lightShader, DEPTHS);
  addUniforms(lightUniformsMap, lightShader, UVS);
  addUniforms(lightUniformsMap, lightShader, LIGHT_DIR);
  UniformGetter lightUniforms = generateUniformGetter(lightUniformsMap);
  checkErrors();

  FBO *fbo = FBOFactory("deferred_fbo", WIDTH, HEIGHT);
  fbo->BindToShader(lightShader);
  checkErrors();

  Renderer r;
  r.sources = {"gbuffer.vert", "gbuffer.frag", "render_buffer.vert", "deferred_dirlight.frag"};
  r.sceneShader = bufferShader;
  r.loadTime = getModifiedTime(r.sources);
  r.render = [=] (RenderScene renderScene) {
    // draw to the frame buffer
    glUseProgram(bufferShader);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo->GetFrameBuffer());
    glEnable(GL_DEPTH_TEST);
    clearActiveBuffer(0,0,0,0, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    checkErrors();

    // render scene (includes setting up camera)
    renderScene(bufferUniforms);
    checkErrors();

    // draw the frame buffer to the screen
    glUseProgram(lightShader);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    clearActiveBuffer(0,0,0,0, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // setup lighting
    glm::vec3 lightDir = glm::vec3(-1,-1,-1);
    glUniform3fv(lightUniforms(LIGHT_DIR), 1, glm::value_ptr(lightDir));
    glUniform1i(lightUniforms(POSITIONS), fbo->GetAttachment(0).index);
    glUniform1i(lightUniforms(NORMALS), fbo->GetAttachment(1).index);
    glUniform1i(lightUniforms(UVS), fbo->GetAttachment(2).index);
    glUniform1i(lightUniforms(DEPTHS), fbo->GetDepth().index);
    checkErrors();

    fbo->Render();
    checkErrors();
  };

  return r;
}

Renderer generateSSAORenderer() {
  GLuint bufferShader = generateShaderProgram("gbuffer.vert", "gbuffer.frag");
  checkErrors();

  UniformsMap bufferUniformsMap;
  addUniforms(bufferUniformsMap, bufferShader, TRANS_UNIFORMS);
  UniformGetter bufferUniforms = generateUniformGetter(bufferUniformsMap);

  GLuint hatchedShader = generateShaderProgram("render_buffer.vert", "deferred_ssao.frag");

  UniformsMap hatchedUniformsMap;
  addUniforms(hatchedUniformsMap, hatchedShader, POSITIONS);
  addUniforms(hatchedUniformsMap, hatchedShader, NORMALS);
  addUniforms(hatchedUniformsMap, hatchedShader, DEPTHS);
  addUniforms(hatchedUniformsMap, hatchedShader, UVS);
  addUniforms(hatchedUniformsMap, hatchedShader, LIGHT_DIR);
  addUniforms(hatchedUniformsMap, hatchedShader, TILE_UNIFORMS);
  UniformGetter hatchedUniforms = generateUniformGetter(hatchedUniformsMap);
  checkErrors();

  int tilesNum = 6;
  Texture tilesTexture = createTexture("tiled_hatches.png");
  checkErrors();

  FBO *fbo = FBOFactory("hatched_fbo", WIDTH, HEIGHT);
  fbo->BindToShader(hatchedShader);
  checkErrors();

  Renderer r;
  r.sources = {"gbuffer.vert", "gbuffer.frag", "render_buffer.vert", "deferred_dirlight.frag"};
  r.sceneShader = bufferShader;
  r.loadTime = getModifiedTime(r.sources);
  r.render = [=] (RenderScene renderScene) {
    // draw to the frame buffer
    glUseProgram(bufferShader);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo->GetFrameBuffer());
    glEnable(GL_DEPTH_TEST);
    clearActiveBuffer(0,0,0,0, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    checkErrors();

    // render scene (includes setting up camera)
    renderScene(bufferUniforms);
    checkErrors();

    // draw the frame buffer to the screen
    glUseProgram(hatchedShader);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    clearActiveBuffer(0,0,0,0, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // setup lighting
    glm::vec3 lightDir = glm::vec3(-1,-1,-1);
    glUniform3fv(hatchedUniforms(LIGHT_DIR), 1, glm::value_ptr(lightDir));

    glUniform1i(hatchedUniforms(NUM_TILES), tilesNum);
    glUniform1i(hatchedUniforms(TILES_TEXTURE), tilesTexture.index);

    glUniform1i(hatchedUniforms(POSITIONS), fbo->GetAttachment(0).index);
    glUniform1i(hatchedUniforms(NORMALS), fbo->GetAttachment(1).index);
    glUniform1i(hatchedUniforms(UVS), fbo->GetAttachment(2).index);
    glUniform1i(hatchedUniforms(DEPTHS), fbo->GetDepth().index);
    checkErrors();

    fbo->Render();
    checkErrors();
  };

  return r;
}
