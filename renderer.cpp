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
    function<void(SetupScene, LightScene, RenderScene)> r,
    long int t)
: sources(s), Render(r), loadTime(t) {};

Renderer generateSSAORenderer(BindScene bindScene) {
  auto gFBO = make_shared<FBO>(WIDTH, HEIGHT);
  auto lightingFBO = make_shared<FBO>(WIDTH, HEIGHT);
  auto ssaoFBO = make_shared<FBO>(WIDTH, HEIGHT);
  auto blurFBO = make_shared<FBO>(WIDTH, HEIGHT);

  auto noiseTexture = make_shared<Texture>("noise.png");
  checkErrors();

  auto hatch0Texture = make_shared<Texture>("hatch_0.jpg");
  auto hatch1Texture = make_shared<Texture>("hatch_1.jpg");
  auto hatch2Texture = make_shared<Texture>("hatch_2.jpg");
  auto hatch3Texture = make_shared<Texture>("hatch_3.jpg");
  auto hatch4Texture = make_shared<Texture>("hatch_4.jpg");
  auto hatch5Texture = make_shared<Texture>("hatch_5.jpg");
  checkErrors();

  // Renders normals/positions/uv to an fbo
  GLuint gShader = generateShaderProgram("gbuffer.vert", "gbuffer.frag");
  Uniforms gUniforms;
  gUniforms.add(gShader, {
      MODEL_TRANS, VIEW_TRANS, PROJ_TRANS,
  });
  bindScene(gShader);
  checkErrors();

  // Render lighting to an fbo
  GLuint lightShader = generateShaderProgram("render_buffer.vert", "deferred_lighting.frag");
  Uniforms lightUniforms;
  lightUniforms.add(lightShader, {
      LIGHT_POSITIONS, NUM_LIGHTS,
      POSITIONS, NORMALS,
      LIGHT_DIR,
  });
  gFBO->BindToShader(lightShader);
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
  gFBO->BindToShader(ssaoShader);
  checkErrors();

  // Blur the ssao
  GLuint blurShader = generateShaderProgram("render_buffer.vert", "blur.frag");
  Uniforms blurUniforms;
  blurUniforms.add(blurShader, {
      BUFFER, DEPTHS
  });
  ssaoFBO->BindToShader(blurShader);
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
  blurFBO->BindToShader(hatchShader);
  checkErrors();

  vector<string> sources = {
      "gbuffer.vert", "gbuffer.frag",
      "render_buffer.vert",
      "deferred_lighting.frag",
      "deferred_ssao.frag",
      "blur.frag",
      "deferred_hatched.frag"
  };
  long int t = getModifiedTime(sources);
  auto render = [=] (SetupScene setupScene, LightScene lightScene, RenderScene renderScene) {
    { // gbuffer render pass
      glUseProgram(gShader);
      glBindFramebuffer(GL_FRAMEBUFFER, gFBO->GetFrameBuffer());

      glEnable(GL_DEPTH_TEST);
      clearActiveBuffer(0,0,0,0, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // render scene
      setupScene(gUniforms);
      renderScene(gUniforms);
      checkErrors();
    }

    { // lighting render pass
      glUseProgram(lightShader);
      glBindFramebuffer(GL_FRAMEBUFFER, 0);

      glEnable(GL_DEPTH_TEST);
      clearActiveBuffer(0,0,0,0, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glUniform1i(lightUniforms.get(POSITIONS), gFBO->GetAttachmentIndex(0));
      glUniform1i(lightUniforms.get(NORMALS), gFBO->GetAttachmentIndex(1));

      lightScene(lightUniforms);

      // render scene
      gFBO->Render();
      checkErrors();
    }

    /*{ // ssao render pass
      glUseProgram(ssaoShader);
      glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO->GetFrameBuffer());

      setupScene(ssaoUniforms);

      glUniform1i(ssaoUniforms.get(RANDOM), noiseTexture->index);
      glUniform1i(ssaoUniforms.get(POSITIONS), gFBO->GetAttachmentIndex(0));
      glUniform1i(ssaoUniforms.get(NORMALS), gFBO->GetAttachmentIndex(1));
      glUniform1i(ssaoUniforms.get(UVS), gFBO->GetAttachmentIndex(2));
      glUniform1i(ssaoUniforms.get(DEPTHS), gFBO->GetDepthIndex());
      checkErrors();

      gFBO->Render();
      checkErrors();
    }

    { // blur render pass
      glUseProgram(blurShader);
      glBindFramebuffer(GL_FRAMEBUFFER, blurFBO->GetFrameBuffer());

      glUniform1i(blurUniforms.get(BUFFER), ssaoFBO->GetAttachmentIndex(0));
      glUniform1i(blurUniforms.get(DEPTHS), gFBO->GetDepthIndex());
      checkErrors();

      ssaoFBO->Render();
      checkErrors();
    }

    { // hatching render pass
      glUseProgram(hatchShader);
      glBindFramebuffer(GL_FRAMEBUFFER, 0);

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

      blurFBO->Render();
      checkErrors();
    }*/
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

  auto render = [=] (SetupScene setupScene, LightScene lightScene, RenderScene renderScene) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(shader);

    glEnable(GL_DEPTH_TEST);
    clearActiveBuffer();
    checkErrors();

    setupScene(uniforms);
    lightScene(uniforms);
    renderScene(uniforms);
    checkErrors();
  };
  vector<string> sources = {"simple.vert", "simple.frag"};
  long int t = getModifiedTime(sources);

  return Renderer(sources, render, t);
}
