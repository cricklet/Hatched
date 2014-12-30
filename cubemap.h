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

class CubeMap {
public:
  CubeMap (int size,
      GLint internalFormat = GL_RGBA32F,
      GLint format = GL_RGBA,
      GLint type = GL_FLOAT
  ) {
    this->size = size;

    // initialize texture
    texture.InitializeAsCubemap();
    textureDepth.InitializeAsCubemap();
    checkErrors();

    //define each of the 6 faces
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture.texture);
    for (int i = 0; i < 6; i++) {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
          internalFormat, size, size, 0, format, type, NULL);
      checkErrors();
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, textureDepth.texture);
    for (int i = 0; i < 6; i++) {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
          GL_DEPTH_COMPONENT, size, size, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
      checkErrors();
    }

    // generate fbos
    glGenFramebuffers(6, &fbos[0]);
    for (int i = 0; i < 6; i++) {
      glBindFramebuffer(GL_FRAMEBUFFER, fbos[i]);

      GLuint drawBuffer = GL_COLOR_ATTACHMENT0;
      glFramebufferTexture2D(GL_FRAMEBUFFER, drawBuffer,
          GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, texture.texture, 0);
      glDrawBuffers(1, &drawBuffer);

      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
          GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, textureDepth.texture, 0);

      GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
      if (GL_FRAMEBUFFER_COMPLETE != result) {
        cout << "ERROR: Framebuffer is not complete.\n";
        printStatus();
      }
    }


    // Return to rendering to the default framebuffer (the screen)
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    checkErrors();
  }

  int Framebuffer(int i) {return fbos[i];};
  int GetTextureIndex() {return texture.index;};
  int Size() {return size;};

private:
  Texture texture;
  Texture textureDepth;
  GLuint fbos[6];
  int size;
};

#endif
