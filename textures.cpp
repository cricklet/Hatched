#include "textures.h"

#include <SOIL/soil.h>

static int _id = 0;
int nextTextureIndex() {
  cout << "Next texture index: " << _id << "\n";
  return _id++;
}

static GLuint loadTexture(const char *filename, int index) {
  GLuint tex;
  glGenTextures(1, &tex);
  glActiveTexture(index + GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tex);

  int width, height;
  unsigned char* image = SOIL_load_image(filename, &width, &height, 0, SOIL_LOAD_RGB);

  glTexImage2D(GL_TEXTURE_2D, 0 /*mipmap*/, GL_RGB, width, height, 0, GL_RGB,
  GL_UNSIGNED_BYTE, image);
  SOIL_free_image_data(image);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glGenerateMipmap(GL_TEXTURE_2D);
}

static map<string, Texture> cache;

static Texture createTextureFromFile(string filename) {
  if (cache.find(filename) != cache.end()) {
    return cache[filename];
  }

  Texture t;
  t.index = nextTextureIndex();
  t.texture = loadTexture(filename.c_str(), t.index);

  cache[filename] = t;

  return t;
}

static Texture createTextureBlank() {
  Texture t;
  t.index = nextTextureIndex();

  glActiveTexture(GL_TEXTURE0 + t.index);
  glGenTextures(1, &t.texture);
  glBindTexture(GL_TEXTURE_2D, t.texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  return t;
}

Texture createTexture(string filename) {
  if (filename.length() > 0) {
    return createTextureFromFile(filename);
  } else {
    return createTextureBlank();
  }
}


