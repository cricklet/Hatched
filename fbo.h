#define _USE_MATH_DEFINES
#include <cmath>

#define GLEW_STATIC
#include <GL/glew.h>

#include <string>
#include <vector>
using namespace std;

#include "helper.h"
#include "textures.h"

#ifndef FBO_H_
#define FBO_H_

const int MAX_ATTACHMENTS = 6;

class FBO {
 public:
  FBO(int width, int height,
      int numAttachments = 1,
      vector<GLint> attachmentInternalFormats = {GL_RGBA},
      vector<GLint> attachmentFormats = {GL_RGBA},
      vector<GLint> attachmentTypes = {GL_UNSIGNED_BYTE},
      bool includeDepth = true,
      GLint depthInternalFormat = GL_DEPTH_COMPONENT24,
      GLint depthType = GL_FLOAT);
  ~FBO();

  GLuint GetFrameBuffer() const;
  int GetAttachmentIndex(int i) const;
  int GetDepthIndex() const;

  int Width() const;
  int Height() const;

 private:
  GLuint fbo;
  Texture attachments[MAX_ATTACHMENTS];
  Texture depth;

  int width, height;

 private: // disable copying
  FBO(const FBO& that) = delete;
  FBO& operator = (const FBO &that) = delete;
};

#endif
