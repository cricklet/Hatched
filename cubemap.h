#define _USE_MATH_DEFINES
#include <cmath>

#define GLEW_STATIC
#include <GL/glew.h>

#include <string>
#include <vector>
using namespace std;

#include "helper.h"
#include "textures.h"

#ifndef CUBEMAP_H_
#define CUBEMAP_H_

class CubeMap {
public:
  CubeMap (int size,
      GLint internalFormat = GL_RGBA,
      GLint format = GL_RGBA,
      GLint types = GL_UNSIGNED_BYTE) {
    // make texture a cubemap
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture.texture);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);
    checkErrors();

    //define each of the 6 faces
    auto targets = {
        GL_TEXTURE_CUBE_MAP_POSITIVE_X,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
    };

    for (auto target : targets) {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RED,
          size, size, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
      checkErrors();
    }

    // generate fbo
    glGenFramebuffers(1, &this->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture.texture, 0);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    checkErrors();

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      std::cerr << "glCheckFramebufferStatus() failed\n";
    }

    // Return to rendering to the default framebuffer (the screen)
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    checkErrors();
  }

private:
  Texture texture;
  GLuint fbo;
};

#endif
