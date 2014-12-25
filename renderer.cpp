#include "renderer.h"
#include "fbo.h"
#include "textures.h"

#include <memory>

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

bool
Renderer::ShouldUpdate() {
  if (getModifiedTime(this->sources) > this->loadTime) {
    return true;
  }
  return false;
}

Renderer::Renderer(
    vector<string> s,
    function<void(RenderScene)> r,
    long int t)
  : sources(s), Render(r), loadTime(t) {};

Renderer generateHatchedRenderer(BindScene bindScene) {
  GLuint bufferShader = generateShaderProgram("gbuffer.vert", "gbuffer.frag");
  checkErrors();

  Uniforms bufferUniforms;
  bufferUniforms.add(bufferShader, {
      MODEL_TRANS, VIEW_TRANS, PROJ_TRANS
  });

  GLuint hatchedShader = generateShaderProgram("render_buffer.vert", "deferred_hatched.frag");
  Uniforms hatchedUniforms;
  hatchedUniforms.add(hatchedShader, {
      MODEL_TRANS, VIEW_TRANS, PROJ_TRANS,
      POSITIONS, NORMALS, DEPTHS, UVS, LIGHT_DIR,
      NUM_TILES, TILES_TEXTURE
  });

  bindScene(hatchedShader);

  checkErrors();

  int tilesNum = 6;
  Texture tilesTexture = newTexture("tiled_hatches.png");
  checkErrors();

  auto fbo = make_shared<FBO>(WIDTH, HEIGHT);
  fbo->BindToShader(hatchedShader);
  checkErrors();

  vector<string> sources = {
      "gbuffer.vert", "gbuffer.frag", "render_buffer.vert", "deferred_dirlight.frag"
  };
  auto t = getModifiedTime(sources);
  auto render = [=] (RenderScene renderScene) {
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

    glUniform3fv(hatchedUniforms.get(LIGHT_DIR), 1, glm::value_ptr(lightDir));

    glUniform1i(hatchedUniforms.get(NUM_TILES), tilesNum);
    glUniform1i(hatchedUniforms.get(TILES_TEXTURE), tilesTexture.index);

    glUniform1i(hatchedUniforms.get(POSITIONS), fbo->GetAttachment(0).index);
    glUniform1i(hatchedUniforms.get(NORMALS), fbo->GetAttachment(1).index);
    glUniform1i(hatchedUniforms.get(UVS), fbo->GetAttachment(2).index);
    glUniform1i(hatchedUniforms.get(DEPTHS), fbo->GetDepth().index);
    checkErrors();

    fbo->Render();
    checkErrors();
  };

  return Renderer(sources, render, t);
}
