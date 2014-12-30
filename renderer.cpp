#include "renderer.h"
#include "fbo.h"
#include "textures.h"
#include "cubemap.h"
#include "fbo_renderer.h"

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

static auto getShadowMapProjection() {
  return glm::perspective(90.0f, 1.0f, 0.5f, 100.0f);
}

static auto getShadowMapTransform(int dir, glm::vec3 origin) {
  switch (dir) {
  case 0: // +X
    return glm::lookAt(origin, origin + glm::vec3(+1, +0, 0), glm::vec3(0, -1, 0));
  case 1: // -X
    return glm::lookAt(origin, origin + glm::vec3(-1, +0, 0), glm::vec3(0, -1, 0));
  case 2: // +Y
    return glm::lookAt(origin, origin + glm::vec3(0, +1, 0), glm::vec3(0, 0, -1));
  case 3: // -Y
    return glm::lookAt(origin, origin + glm::vec3(0, -1, 0), glm::vec3(0, 0, -1));
  case 4: // +Z
    return glm::lookAt(origin, origin + glm::vec3(0, 0, +1), glm::vec3(0, -1, 0));
  case 5: // -Z
    return glm::lookAt(origin, origin + glm::vec3(0, 0, -1), glm::vec3(0, -1, 0));
  }
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
    function<void(SetupScene, GetLights, RenderScene)> r,
    long int t):
                        sources(s), Render(r), loadTime(t) {
}

Renderer generateSSAORenderer(BindScene bindScene) {
  int halfWidth = WIDTH * 0.5;
  int halfHeight = HEIGHT * 0.5;

  auto gInternalFormats = {GL_RGBA32F, GL_RGBA32F, GL_RGBA};
  auto gFormats = {GL_RGBA, GL_RGBA, GL_RGBA};
  auto gTypes = {GL_FLOAT, GL_FLOAT, GL_UNSIGNED_BYTE};

  auto gFBO = make_shared<FBO>(halfWidth, halfHeight, 3,
      gInternalFormats, gFormats, gTypes,
      true, GL_DEPTH_COMPONENT24, GL_FLOAT);

  auto lightingFBO = make_shared<FBO>(halfWidth, halfHeight);
  auto ssaoFBO = make_shared<FBO>(halfWidth, halfHeight);
  auto blurFBO = make_shared<FBO>(halfWidth, halfHeight);
  auto hatchedFBO = make_shared<FBO>(halfWidth, halfHeight);

  auto sInternalFormats = {GL_R32F, GL_R32F, GL_R32F, GL_R32F, GL_R32F, GL_R32F};
  auto sFormats = {GL_RED, GL_RED, GL_RED, GL_RED, GL_RED, GL_RED};
  auto sTypes = {GL_FLOAT, GL_FLOAT, GL_FLOAT, GL_FLOAT, GL_FLOAT, GL_FLOAT};

  vector<shared_ptr<CubeMap>> shadowMaps;
  for (int i = 0; i < 4; i ++) {
    auto s = make_shared<CubeMap>(halfHeight);
    shadowMaps.push_back(s);
  };

  auto noiseTexture = makeTextureAs2D("noise.png");
  checkErrors();

  auto hatch0Texture = makeTextureAs2D("hatch_0.jpg");
  auto hatch1Texture = makeTextureAs2D("hatch_1.jpg");
  auto hatch2Texture = makeTextureAs2D("hatch_2.jpg");
  auto hatch3Texture = makeTextureAs2D("hatch_3.jpg");
  auto hatch4Texture = makeTextureAs2D("hatch_4.jpg");
  auto hatch5Texture = makeTextureAs2D("hatch_5.jpg");
  checkErrors();

  auto fboRenderer = make_shared<FBORenderer>();

  // Renders normals/positions/uv to an fbo
  GLuint gShader = generateShaderProgram("gbuffer.vert", "gbuffer.frag");
  Uniforms gUniforms;
  gUniforms.add(gShader, {
      MODEL_TRANS, VIEW_TRANS, PROJ_TRANS,
  });
  bindScene(gShader);
  checkErrors();

  // Generates shadow maps
  GLuint smShader = generateShaderProgram("gbuffer.vert", "shadowmap.frag");
  Uniforms smUniforms;
  smUniforms.add(gShader, {
      MODEL_TRANS, VIEW_TRANS, PROJ_TRANS,
  });
  bindScene(smShader);
  checkErrors();

  // Renders scene with shadows
//  GLuint shShader = generateShaderProgram("render_buffer.vert", "deferred_shadows.frag");
//  Uniforms shUniforms;
//  shUniforms.add(shShader, {
//      LIGHT_POSITIONS, LIGHT_CONSTANTS, LIGHT_COLORS, NUM_LIGHTS,
//      SHADOW_MAP,
//      POSITIONS, NORMALS,
//  });
//  bindScene(shShader);
//  checkErrors();

  // Render lighting to an fbo
  GLuint lightShader = generateShaderProgram("render_buffer.vert", "deferred_lighting.frag");
  Uniforms lightUniforms;
  lightUniforms.add(lightShader, {
      LIGHT_POSITIONS, LIGHT_CONSTANTS, LIGHT_COLORS, NUM_LIGHTS,
      POSITIONS, NORMALS,
  });
  checkErrors();

  // Computes ssao from the previous fbo
  GLuint ssaoShader = generateShaderProgram("render_buffer.vert", "deferred_ssao.frag");
  Uniforms ssaoUniforms;
  ssaoUniforms.add(ssaoShader, {
      POSITIONS, NORMALS, DEPTHS, UVS,
      RANDOM,
      VIEW_TRANS, PROJ_TRANS,
      INV_VIEW_TRANS, INV_PROJ_TRANS,
  });
  fboRenderer->BindToShader(ssaoShader);
  checkErrors();

  // Blur the ssao
  GLuint blurShader = generateShaderProgram("render_buffer.vert", "blur.frag");
  Uniforms blurUniforms;
  blurUniforms.add(blurShader, {
      BUFFER, DEPTHS
  });
  fboRenderer->BindToShader(blurShader);
  checkErrors();

  // Renders the hatched ssao
  GLuint hatchShader = generateShaderProgram("render_buffer.vert", "deferred_hatched.frag");
  Uniforms hatchUniforms;
  hatchUniforms.add(hatchShader, {
      HATCH0_TEXTURE, HATCH1_TEXTURE, HATCH2_TEXTURE,
      HATCH3_TEXTURE, HATCH4_TEXTURE, HATCH5_TEXTURE,
      POSITIONS, NORMALS, UVS,
      BUFFER,
      LIGHT_DIR,
      VIEW_TRANS,
  });
  fboRenderer->BindToShader(hatchShader);
  checkErrors();

  // Render the buffers
  GLuint bufferShader = generateShaderProgram("render_buffer.vert", "render_buffer.frag");
  Uniforms bufferUniforms;
  bufferUniforms.add(bufferShader, {
      BUFFER, SCALE, OFFSET_X, OFFSET_Y
  });
  fboRenderer->BindToShader(bufferShader);

  vector<string> sources = {
      "gbuffer.vert", "gbuffer.frag",
      "render_buffer.vert",
      "deferred_lighting.frag",
      "deferred_ssao.frag",
      "blur.frag",
      "deferred_hatched.frag",
      "render_buffer.frag",
      "shadowmap.frag",
      "deferred_shadows.frag"
  };
  long int t = getModifiedTime(sources);

  auto render = [=] (SetupScene setupScene, GetLights getLights, RenderScene renderScene) {

    auto lights = getLights();
    { // render each shadowmap
      glm::mat4 projTrans = getShadowMapProjection();
      for (int i = 0; i < lights.num(); i ++) {
        auto cubeMap = shadowMaps[i];

        auto position = lights.getPosition(i);
        glm::mat4 viewTrans = getShadowMapTransform(i, position);

        glUseProgram(smShader);
        glBindFramebuffer(GL_FRAMEBUFFER, cubeMap->Framebuffer(i));
        glViewport(0,0, cubeMap->Size(), cubeMap->Size());

        glEnable(GL_DEPTH_TEST);
        clearActiveBuffer(0,0,0,0, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUniformMatrix4fv(lightUniforms.get(VIEW_TRANS),
            1, GL_FALSE, glm::value_ptr(viewTrans));

        glUniformMatrix4fv(lightUniforms.get(PROJ_TRANS),
            1, GL_FALSE, glm::value_ptr(projTrans));

        renderScene(lightUniforms);
        checkErrors();
      }
    }

    auto bindFBO = [&] (auto fbo) {
      glBindFramebuffer(GL_FRAMEBUFFER, fbo->GetFrameBuffer());
      glViewport(0,0, fbo->Width(), fbo->Height());
    };

    { // gbuffer render pass
      glUseProgram(gShader);
      bindFBO(gFBO);

      glEnable(GL_DEPTH_TEST);
      clearActiveBuffer(0,0,0,0, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // render scene
      setupScene(gUniforms);
      renderScene(gUniforms);
      checkErrors();
    }

    { // lighting render pass
      glUseProgram(lightShader);
      bindFBO(lightingFBO);

      glEnable(GL_DEPTH_TEST);
      clearActiveBuffer(0,0,0,0, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glUniform1i(lightUniforms.get(POSITIONS), gFBO->GetAttachmentIndex(0));
      glUniform1i(lightUniforms.get(NORMALS), gFBO->GetAttachmentIndex(1));

      auto posValues = lights.getPositions();
      auto conValues = lights.getConstants();
      auto colValues = lights.getColors();

      int numLights = lights.num();
      glUniform1i(lightUniforms.get(NUM_LIGHTS), numLights);
      glUniform3fv(lightUniforms.get(LIGHT_POSITIONS), numLights, posValues);
      glUniform3fv(lightUniforms.get(LIGHT_CONSTANTS), numLights, conValues);
      glUniform3fv(lightUniforms.get(LIGHT_COLORS),    numLights, colValues);

      // render scene
      fboRenderer->Render();
      checkErrors();
    }

    { // ssao render pass
      glUseProgram(ssaoShader);
      bindFBO(ssaoFBO);

      setupScene(ssaoUniforms);

      glUniform1i(ssaoUniforms.get(RANDOM), noiseTexture->index);
      glUniform1i(ssaoUniforms.get(POSITIONS), gFBO->GetAttachmentIndex(0));
      glUniform1i(ssaoUniforms.get(NORMALS), gFBO->GetAttachmentIndex(1));
      glUniform1i(ssaoUniforms.get(UVS), gFBO->GetAttachmentIndex(2));
      glUniform1i(ssaoUniforms.get(DEPTHS), gFBO->GetDepthIndex());
      checkErrors();

      fboRenderer->Render();
      checkErrors();
    }

    { // blur render pass
      glUseProgram(blurShader);
      bindFBO(blurFBO);

      glUniform1i(blurUniforms.get(BUFFER), ssaoFBO->GetAttachmentIndex(0));
      glUniform1i(blurUniforms.get(DEPTHS), gFBO->GetDepthIndex());
      checkErrors();

      fboRenderer->Render();
      checkErrors();
    }

    { // hatching render pass
      glUseProgram(hatchShader);
      bindFBO(hatchedFBO);

      glm::vec3 lightDir = glm::vec3(-1,-1,-1);
      glUniform3fv(hatchUniforms.get(LIGHT_DIR), 1, glm::value_ptr(lightDir));

      glUniform1i(hatchUniforms.get(HATCH0_TEXTURE), hatch0Texture->index);
      glUniform1i(hatchUniforms.get(HATCH1_TEXTURE), hatch1Texture->index);
      glUniform1i(hatchUniforms.get(HATCH2_TEXTURE), hatch2Texture->index);
      glUniform1i(hatchUniforms.get(HATCH3_TEXTURE), hatch3Texture->index);
      glUniform1i(hatchUniforms.get(HATCH4_TEXTURE), hatch4Texture->index);
      glUniform1i(hatchUniforms.get(HATCH5_TEXTURE), hatch5Texture->index);

      glUniform1i(hatchUniforms.get(POSITIONS), gFBO->GetAttachmentIndex(0));
      glUniform1i(hatchUniforms.get(NORMALS), gFBO->GetAttachmentIndex(1));
      glUniform1i(hatchUniforms.get(UVS), gFBO->GetAttachmentIndex(2));
      glUniform1i(hatchUniforms.get(BUFFER), blurFBO->GetAttachmentIndex(0));
      checkErrors();

      setupScene(hatchUniforms);

      fboRenderer->Render();
      checkErrors();
    }

    { // render buffers
      glViewport(0,0, WIDTH, HEIGHT);
      glUseProgram(bufferShader);
      glBindFramebuffer(GL_FRAMEBUFFER, 0);

      auto drawBuffer = [&] (auto attachment, float scale, float offX, float offY) {
        glUniform1f(bufferUniforms.get(SCALE), scale);
        glUniform1f(bufferUniforms.get(OFFSET_X), offX);
        glUniform1f(bufferUniforms.get(OFFSET_Y), offY);
        glUniform1i(bufferUniforms.get(BUFFER), attachment);
        fboRenderer->Render();
      };

      drawBuffer(lightingFBO->GetAttachmentIndex(0), 0.5, -0.5,-0.5);
      drawBuffer(ssaoFBO->GetAttachmentIndex(0), 0.5, -0.5,0.5);
      drawBuffer(shadowMaps[0]->GetTextureIndex(), 0.5, 0.5,-0.5);
      drawBuffer(hatchedFBO->GetAttachmentIndex(0), 0.5, 0.5,0.5);

      checkErrors();
    }
  };

  return Renderer(sources, render, t);
}

Renderer generateSimpleRenderer(BindScene bindScene) {
  GLuint shader = generateShaderProgram("simple.vert", "simple.frag");
  bindScene(shader);

  Uniforms uniforms;
  uniforms.add(shader, {
      MODEL_TRANS, VIEW_TRANS, PROJ_TRANS, COLOR
  });

  auto render = [=] (SetupScene setupScene, GetLights getLights, RenderScene renderScene) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(shader);

    glEnable(GL_DEPTH_TEST);
    clearActiveBuffer();
    checkErrors();

    setupScene(uniforms);
    renderScene(uniforms);
    checkErrors();
  };
  vector<string> sources = { "simple.vert", "simple.frag" };
  long int t = getModifiedTime(sources);

  return Renderer(sources, render, t);
}
