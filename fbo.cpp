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

  // Create textures
  for (int i = 0; i < NUM_ATTACHMENTS; i ++) {
    this->attachments[i] = newTexture();
  }
  this->depth = newTexture();
  checkErrors();

  // Bind textures to frame buffer
  GLuint drawBuffers[NUM_ATTACHMENTS];
  for (int i = 0; i < NUM_ATTACHMENTS; i ++) {
    GLuint texture = this->attachments[i].texture;
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL /*no data*/);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texture, 0);
    checkErrors();

    drawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
  }
  glDrawBuffers(3, drawBuffers);

  glBindTexture(GL_TEXTURE_2D, this->depth.texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL /*no data*/);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->depth.texture, 0);
  checkErrors();

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cerr << "glCheckF ramebufferStatus() failed\n";
  }

  printStatus();

  // Return to rendering to the default framebuffer (the screen)
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  checkErrors();
}

FBO::~FBO() {
  glDeleteVertexArrays(1, &this->vao);
  glDeleteFramebuffers(1, &this->fbo);
  glDeleteBuffers(1, &this->vbo);
  cout << "~FBO()\n";
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

GLuint FBO::GetFrameBuffer() const {
  return this->fbo;
}

Texture FBO::GetAttachment (int i) const {
  return this->attachments[i];
}

Texture FBO::GetDepth() const {
  return this->depth;
}

void FBO::Render() const {
  glBindVertexArray(this->vao);

  // Disable tests
  glDisable(GL_DEPTH_TEST);

  // Render the fbo quad
  glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
  glDrawArrays(GL_TRIANGLES, 0, numElements);
}
