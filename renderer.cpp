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
  return glm::perspective((float) (M_PI / 2), 1.0f, 0.5f, 100.0f);
}

static auto getShadowMapTransform(int dir, glm::vec3 origin) {
  switch (dir) {
    case 0: // +X
      return glm::lookAt(origin, origin + glm::vec3(+1, +0, 0), glm::vec3(0, -1, 0));
    case 1: // -X
      return glm::lookAt(origin, origin + glm::vec3(-1, +0, 0), glm::vec3(0, -1, 0));
    case 2: // +Y
      return glm::lookAt(origin, origin + glm::vec3(0, +1, 0), glm::vec3(0, 0, 1));
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
  int quarterWidth = WIDTH * 0.5;
  int quarterHeight = HEIGHT * 0.5;

  auto gInternalFormats = {GL_RGBA32F, GL_RGBA32F, GL_RGBA32F};
  auto gFormats = {GL_RGBA, GL_RGBA, GL_RGBA};
  auto gTypes = {GL_FLOAT, GL_FLOAT, GL_FLOAT};

  auto gFBO = make_shared<FBO>(halfWidth, halfHeight,
      3, gInternalFormats, gFormats, gTypes,
      true, GL_DEPTH_COMPONENT24, GL_FLOAT);

  auto lInternalFormats = {GL_RGBA, GL_RGBA};
  auto lFormats = {GL_RGBA, GL_RGBA};
  auto lTypes = {GL_UNSIGNED_BYTE, GL_UNSIGNED_BYTE};
  auto lightingFBO = make_shared<FBO>(halfWidth, halfHeight,
      2, lInternalFormats, lFormats, lTypes);

  auto ssaoFBO = make_shared<FBO>(halfWidth, halfHeight);
  auto blur0FBO = make_shared<FBO>(halfWidth, halfHeight);
  auto blur1FBO = make_shared<FBO>(halfWidth, halfHeight);
  auto hatchedFBO = make_shared<FBO>(halfWidth, halfHeight);

  auto smInternalFormats = {GL_RGBA32F};
  auto smFormats = {GL_RGBA};
  auto smTypes = {GL_FLOAT};

  auto sm0FBO = make_shared<FBO>(HEIGHT, HEIGHT,
      1, smInternalFormats, smFormats, smTypes);
  auto sm1FBO = make_shared<FBO>(HEIGHT, HEIGHT,
      1, smInternalFormats, smFormats, smTypes);
  const int MAX_SHADOW_MAPS = 8;
  vector<shared_ptr<CubeMap>> shadowMaps;
  for (int i = 0; i < MAX_SHADOW_MAPS; i ++) {
    auto s = make_shared<CubeMap>(HEIGHT);
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
  GLuint lightingShader = generateShaderProgram("render_buffer.vert", "deferred_shadows.frag");
  Uniforms lightingUniforms;
  lightingUniforms.add(lightingShader, {
      LIGHT_POSITIONS, LIGHT_CONSTANTS, LIGHT_COLORS, NUM_LIGHTS,
      SHADOW_MAPS, NUM_SHADOW_MAPS,
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
  GLuint blurShader = generateShaderProgram("render_buffer.vert", "gaussian_blur.frag");
  Uniforms blurUniforms;
  blurUniforms.add(blurShader, {
      BUFFER, BUFFER_SIZE, BLUR_DIR
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
      LIGHT_BUFFER, SHADOW_BUFFER, SSAO_BUFFER,
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
      "gaussian_blur.frag",
      "deferred_hatched.frag",
      "render_buffer.frag",
      "shadowmap.frag",
      "deferred_shadows.frag"
  };
  long int t = getModifiedTime(sources);

  auto shadowsRendered = make_shared<bool>(false);

  auto render = [=] (SetupScene setupScene, GetLights getLights, RenderScene renderScene) {

    auto lights = getLights();
    int numShadowMaps = min(lights.num(), (int) shadowMaps.size());

    if (*shadowsRendered == false) { // render each shadowmap
      *shadowsRendered = true;

      glm::mat4 projTrans = getShadowMapProjection();
      for (int lightIndex = 0; lightIndex < numShadowMaps; lightIndex ++) {
        auto shadowMap = shadowMaps[lightIndex];
        auto position = lights.getPosition(lightIndex);

        for (int faceIndex = 0; faceIndex < 6; faceIndex ++) {
          glm::mat4 viewTrans = getShadowMapTransform(faceIndex, position);

          { // render the shadow map
            glUseProgram(smShader);
            glEnable(GL_DEPTH_TEST);

            glViewport(0,0, sm0FBO->Width(), sm0FBO->Height());
            glBindFramebuffer(GL_FRAMEBUFFER, sm0FBO->GetFrameBuffer());
            clearActiveBuffer();

            glUniformMatrix4fv(smUniforms.get(VIEW_TRANS),
                1, GL_FALSE, glm::value_ptr(viewTrans));
            glUniformMatrix4fv(smUniforms.get(PROJ_TRANS),
                1, GL_FALSE, glm::value_ptr(projTrans));

            renderScene(smUniforms);
            checkErrors();
          }

          { // blur the shadow map
            if (sm1FBO->Width() != shadowMap->Size()) {
              throw runtime_error(
                  string("Blur FBO needs to be the same size as what's being blurred."));
            }

            auto xDir = glm::vec2(1,0);
            auto yDir = glm::vec2(0,1);

            glUseProgram(blurShader);

            // blur using the intermediate
            glViewport(0,0, sm1FBO->Width(), sm1FBO->Height());
            glBindFramebuffer(GL_FRAMEBUFFER, sm1FBO->GetFrameBuffer());
            clearActiveBuffer();

            glUniform1i(blurUniforms.get(BUFFER), sm0FBO->GetAttachmentIndex(0));
            glUniform1i(blurUniforms.get(BUFFER_SIZE), sm0FBO->Width());
            glUniform2fv(blurUniforms.get(BLUR_DIR), 1, glm::value_ptr(xDir));
            checkErrors();

            fboRenderer->Render();
            checkErrors();

            // finish blur
            glViewport(0,0, shadowMap->Size(), shadowMap->Size());
            glBindFramebuffer(GL_FRAMEBUFFER, shadowMap->Framebuffer(faceIndex));
            clearActiveBuffer();

            glUniform1i(blurUniforms.get(BUFFER), sm1FBO->GetAttachmentIndex(0));
            glUniform1i(blurUniforms.get(BUFFER_SIZE), sm1FBO->Width());
            glUniform2fv(blurUniforms.get(BLUR_DIR), 1, glm::value_ptr(yDir));
            checkErrors();

            fboRenderer->Render();
            checkErrors();
          }
        }
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

    { // lighting/shadowing render pass
      glUseProgram(lightingShader);
      bindFBO(lightingFBO);

      glEnable(GL_DEPTH_TEST);
      clearActiveBuffer(0,0,0,0, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glUniform1i(lightingUniforms.get(POSITIONS), gFBO->GetAttachmentIndex(0));
      glUniform1i(lightingUniforms.get(NORMALS), gFBO->GetAttachmentIndex(1));

      auto posValues = lights.getPositions();
      auto conValues = lights.getConstants();
      auto colValues = lights.getColors();

      int numLights = lights.num();
      glUniform1i(lightingUniforms.get(NUM_LIGHTS), numLights);
      glUniform3fv(lightingUniforms.get(LIGHT_POSITIONS), numLights, posValues);
      glUniform3fv(lightingUniforms.get(LIGHT_CONSTANTS), numLights, conValues);
      glUniform3fv(lightingUniforms.get(LIGHT_COLORS),    numLights, colValues);

      int shadowMapIndices[MAX_SHADOW_MAPS];
      for (int i = 0; i < MAX_SHADOW_MAPS; i ++) {
        shadowMapIndices[i] = shadowMaps[i]->GetTextureIndex();
      }
      glUniform1iv(lightingUniforms.get(SHADOW_MAPS), MAX_SHADOW_MAPS, shadowMapIndices);
      glUniform1i(lightingUniforms.get(NUM_SHADOW_MAPS), numShadowMaps);

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
      if (ssaoFBO->Width() != blur0FBO->Width()) {
        throw runtime_error(
            string("Blur FBO needs to be the same size as what's being blurred."));
      }

      auto xDir = glm::vec2(1,0);
      auto yDir = glm::vec2(0,1);

      glUseProgram(blurShader);
      bindFBO(blur0FBO);

      glUniform1i(blurUniforms.get(BUFFER), ssaoFBO->GetAttachmentIndex(0));
      glUniform1i(blurUniforms.get(BUFFER_SIZE), ssaoFBO->Width());
      glUniform2fv(blurUniforms.get(BLUR_DIR), 1, glm::value_ptr(xDir));
      checkErrors();

      fboRenderer->Render();
      checkErrors();

      bindFBO(blur1FBO);

      glUniform1i(blurUniforms.get(BUFFER), blur0FBO->GetAttachmentIndex(0));
      glUniform1i(blurUniforms.get(BUFFER_SIZE), blur0FBO->Height());
      glUniform2fv(blurUniforms.get(BLUR_DIR), 1, glm::value_ptr(yDir));
      checkErrors();

      fboRenderer->Render();
      checkErrors();
    }

    { // hatching render pass
      glUseProgram(hatchShader);
      bindFBO(hatchedFBO);

      glUniform1i(hatchUniforms.get(HATCH0_TEXTURE), hatch0Texture->index);
      glUniform1i(hatchUniforms.get(HATCH1_TEXTURE), hatch1Texture->index);
      glUniform1i(hatchUniforms.get(HATCH2_TEXTURE), hatch2Texture->index);
      glUniform1i(hatchUniforms.get(HATCH3_TEXTURE), hatch3Texture->index);
      glUniform1i(hatchUniforms.get(HATCH4_TEXTURE), hatch4Texture->index);
      glUniform1i(hatchUniforms.get(HATCH5_TEXTURE), hatch5Texture->index);

      glUniform1i(hatchUniforms.get(POSITIONS), gFBO->GetAttachmentIndex(0));
      glUniform1i(hatchUniforms.get(NORMALS), gFBO->GetAttachmentIndex(1));
      glUniform1i(hatchUniforms.get(UVS), gFBO->GetAttachmentIndex(2));

      glUniform1i(hatchUniforms.get(LIGHT_BUFFER), lightingFBO->GetAttachmentIndex(0));
      glUniform1i(hatchUniforms.get(SHADOW_BUFFER), lightingFBO->GetAttachmentIndex(1));
      glUniform1i(hatchUniforms.get(SSAO_BUFFER), blur1FBO->GetAttachmentIndex(0));
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

      drawBuffer(blur1FBO->GetAttachmentIndex(0), 0.5, -0.5,0.5);
      drawBuffer(hatchedFBO->GetAttachmentIndex(0), 0.5, 0.5,0.5);
      drawBuffer(lightingFBO->GetAttachmentIndex(1), 0.5, -0.5,-0.5);
      drawBuffer(lightingFBO->GetAttachmentIndex(0), 0.5, 0.5,-0.5);

      checkErrors();
    }
  };

  return Renderer(sources, render, t);
}

Renderer generateNULLRenderer() {
  Uniforms uniforms;
  auto render = [=] (SetupScene setupScene, GetLights getLights, RenderScene renderScene) {
  };
  vector<string> sources = {};
  long int t = 0;
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
