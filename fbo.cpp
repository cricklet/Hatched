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

FBO::FBO(int width, int height,
    int numAttachments,
    vector<GLint> attachmentInternalFormats,
    vector<GLint> attachmentFormats,
    vector<GLint> attachmentTypes,
    bool includeDepth,
    GLint depthInternalFormat,
    GLint depthInternalType
) {
  this->width = width;
  this->height = height;

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

  // Bind textures to frame buffer
  GLuint drawBuffers[numAttachments];
  for (int i = 0; i < numAttachments; i ++) {
    GLint internalFormat = attachmentInternalFormats[i];
    GLint format = attachmentFormats[i];
    GLint type = attachmentTypes[i];

    GLuint texture = this->attachments[i].texture;

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texture, 0);
    checkErrors();

    drawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
  }
  glDrawBuffers(numAttachments, drawBuffers);

  if (includeDepth) {
    glBindTexture(GL_TEXTURE_2D, this->depth.texture);
    glTexImage2D(GL_TEXTURE_2D, 0, depthInternalFormat, width, height, 0, GL_DEPTH_COMPONENT, depthInternalType, NULL /*no data*/);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->depth.texture, 0);
    checkErrors();
  }

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cerr << "glCheckFramebufferStatus() failed\n";
  }

  // printStatus();

  // Return to rendering to the default framebuffer (the screen)
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  checkErrors();
}

FBO::~FBO() {
  glDeleteVertexArrays(1, &this->vao);
  glDeleteFramebuffers(1, &this->fbo);
  glDeleteBuffers(1, &this->vbo);
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

int FBO::GetAttachmentIndex (int i) const {
  return this->attachments[i].index;
}

int FBO::GetDepthIndex () const {
  return this->depth.index;
}

int FBO::Width() const {
  return this->width;
}

int FBO::Height() const {
  return this->height;
}

void FBO::Render() const {
  glBindVertexArray(this->vao);

  // Disable tests
  glDisable(GL_DEPTH_TEST);

  // Render the fbo quad
  glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
  glDrawArrays(GL_TRIANGLES, 0, numElements);
}
