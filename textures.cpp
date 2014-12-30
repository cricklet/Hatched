#include "textures.h"
#include "helper.h"

#include <stdexcept>
#include <SOIL/soil.h>

static bool indicesInUse[100]; // initialized to all zero b/c static

static int newTextureIndex () {
  for (int i = 0; i < 100; i ++) {
    if (indicesInUse[i] == false) {
      indicesInUse[i] = true;
      // cout << "Next texture index: " << i << "\n";
      return i;
    }
  }
  // cout << "Failed to retrieve texture index: " << -1 << "\n";
  return -1;
}

static void freeTextureIndex (int i) {
  indicesInUse[i] = false;
  // cout << "Freeing texture index: " << i << "\n";
}

static GLuint loadTexture(const char *filename, int index) {
  GLuint texture;

  glGenTextures(1, &texture);
  glActiveTexture(index + GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);

  int width, height;
  unsigned char* image = SOIL_load_image(filename, &width, &height, 0, SOIL_LOAD_RGB);

  glTexImage2D(
      GL_TEXTURE_2D, 0 /*mipmap*/, GL_RGB, width, height,
      0, GL_RGB, GL_UNSIGNED_BYTE, image);
  checkErrors();

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glGenerateMipmap(GL_TEXTURE_2D);
  checkErrors();

  SOIL_free_image_data(image);

  return texture;
}

static GLuint loadTexture(int index) {
  GLuint texture;

  glActiveTexture(GL_TEXTURE0 + index);
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  return texture;
}

static GLuint loadCubemap(int index) {
  GLuint texture;

  glActiveTexture(GL_TEXTURE0 + index);
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);

  return texture;
}

static void checkInitialized(bool &initialized) {
  if (initialized) {
    throw runtime_error(string("Texture already initialized"));
  } else {
    initialized = true;
  }
}

Texture::Texture() {
}

void Texture::InitializeAs2D(string filename) {
  checkInitialized(this->initialized);

  this->index = newTextureIndex();

  if (filename.length() > 0) {
    this->texture = loadTexture(filename.c_str(), this->index);
  } else {
    this->texture = loadTexture(this->index);
  }
}

void Texture::InitializeAsCubemap() {
  checkInitialized(this->initialized);

  this->index = newTextureIndex();
  this->texture = loadCubemap(this->index);
}

Texture::~Texture() {
  if (initialized) {
    glDeleteTextures(1, &this->texture);
    freeTextureIndex(this->index);
  }
}

shared_ptr<Texture> makeTextureAs2D(string filename) {
  auto t = make_shared<Texture>();
  t->InitializeAs2D(filename);
  return t;
}
