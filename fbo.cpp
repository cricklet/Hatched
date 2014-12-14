#include "fbo.h"

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

static GLuint generateVBO() {
  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  checkErrors();

  return vbo;
}

static GLuint generateFBO(int index, int width, int height) {
  GLuint fbo;
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  checkErrors();

  // Create a texture for use by this framebuffer
  GLuint fboTexture;
  glActiveTexture(GL_TEXTURE0 + index);
  glGenTextures(1, &fboTexture);
  glBindTexture(GL_TEXTURE_2D, fboTexture);
  checkErrors();

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL /*no data*/);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  checkErrors();

  // Bind the texture to the frambuffer
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTexture, 0);
  checkErrors();

  // Create a render buffer object for use by this framebuffer
  GLuint fboRenderBuffer;
  glGenRenderbuffers(1, &fboRenderBuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, fboRenderBuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
  GL_RENDERBUFFER, fboRenderBuffer);
  checkErrors();

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cerr << "glCheckF ramebufferStatus() failed\n";
  }

  // Return to rendering to the default framebuffer (the screen)
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  return fbo;
}

FBO::FBO(int width, int height) {
  glGenVertexArrays(1, &this->vao);
  glBindVertexArray(this->vao);

  this->textureIndex = nextTextureIndex();
  this->fbo = generateFBO(this->textureIndex, width, height);
  this->vbo = generateVBO();
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

int FBO::GetTextureIndex() {
  return this->textureIndex;
}

void FBO::Render() {
  glBindVertexArray(this->vao);

  // Disable tests
  glDisable(GL_DEPTH_TEST);

  // Render the fbo quad
  glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
  glDrawArrays(GL_TRIANGLES, 0, numElements);
}

