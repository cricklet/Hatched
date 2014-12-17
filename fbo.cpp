#include "fbo.h"
#include "textures.h"

static GLfloat vertices[] = {
    -1, -1, 0, 0,
    -1, 1, 0, 1,
    1, 1, 1, 1,

    1, 1, 1, 1,
    1, -1, 1, 0,
    -1, -1, 0, 0,
};

static GLuint vertexStride = sizeof(GLfloat) * 4;
static void *positionOffset = 0;
static void *coordOffset = (void *) (2 * sizeof(GLfloat));
static int numElements = 6;

static void printStatus() {
  int res, i = 0;
  GLint buffer;

  do {
    glGetIntegerv(GL_DRAW_BUFFER0+i, &buffer);

    if (buffer != GL_NONE) {

      printf("Shader Output Location %d - color attachment %d\n",
          i, buffer - GL_COLOR_ATTACHMENT0);

      glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, buffer,
          GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &res);
      printf("\tAttachment Type: %s\n",
          res==GL_TEXTURE?"Texture":"Render Buffer");
      glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, buffer,
          GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &res);
      printf("\tAttachment object name: %d\n",res);
    }
    ++i;

  } while (buffer != GL_NONE);
}

FBO::FBO(int width, int height) {
  glGenVertexArrays(1, &this->vao);
  glBindVertexArray(this->vao);
  checkErrors();

  glGenFramebuffers(1, &this->fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
  checkErrors();

  glGenBuffers(1, &this->vbo);
  glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  checkErrors();

  // Create attachment 0 texture
  this->attachment0 = createTexture();
  this->attachment1 = createTexture();
  this->depth = createTexture();
  checkErrors();

  glBindTexture(GL_TEXTURE_2D, this->attachment0.texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL /*no data*/);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->attachment0.texture, 0);
  checkErrors();

  glBindTexture(GL_TEXTURE_2D, this->attachment1.texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL /*no data*/);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, this->attachment1.texture, 0);
  checkErrors();

  glBindTexture(GL_TEXTURE_2D, this->depth.texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL /*no data*/);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->depth.texture, 0);
  checkErrors();

  GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
  glDrawBuffers(2,  attachments);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cerr << "glCheckF ramebufferStatus() failed\n";
  }

  printStatus();

  // Return to rendering to the default framebuffer (the screen)
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  checkErrors();
}

FBO::~FBO() {
}

void FBO::BindToShader(GLuint shaderProgram) {
  glBindVertexArray(this->vao);
  glBindBuffer(GL_ARRAY_BUFFER, this->vbo);

  GLint posAttrib = glGetAttribLocation(shaderProgram, "inVertPosition");
  glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, vertexStride, positionOffset);
  glEnableVertexAttribArray(posAttrib);
  checkErrors();

  GLint coordAttrib = glGetAttribLocation(shaderProgram, "inVertBufferCoord");
  glVertexAttribPointer(coordAttrib, 2, GL_FLOAT, GL_FALSE, vertexStride, coordOffset);
  glEnableVertexAttribArray(coordAttrib);
  checkErrors();
}

GLuint FBO::GetFrameBuffer() {
  return this->fbo;
}

Texture FBO::GetAttachment0 () {
  return this->attachment0;
}

Texture FBO::GetAttachment1 () {
  return this->attachment1;
}

Texture FBO::GetDepth() {
  return this->depth;
}

void FBO::Render() {
  glBindVertexArray(this->vao);

  // Disable tests
  glDisable(GL_DEPTH_TEST);

  // Render the fbo quad
  glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
  glDrawArrays(GL_TRIANGLES, 0, numElements);
}

static map<string, FBO *> cache;
FBO *FBOFactory(string id, int width, int height) {
  if (cache.find(id) != cache.end()) {
    return cache[id];
  }

  FBO *f = new FBO(width, height);
  cache[id] = f;
  return f;
}
