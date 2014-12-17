#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include <stdexcept>
#include <cstdlib>

#define GLEW_STATIC
#include <GL/glew.h>

#include "helper.h"

using namespace std;

float random(float max) {
  return max * ((float) rand()) / ((float) RAND_MAX);
}

bool _checkErrors(const char *filename, int line) {
  bool result = false;

  GLenum error;
  while ((error = glGetError()) != GL_NO_ERROR) {
    string str;

    switch (error) {
      case GL_INVALID_OPERATION:
        str = "INVALID_OPERATION";
        break;
      case GL_INVALID_ENUM:
        str = "INVALID_ENUM";
        break;
      case GL_INVALID_VALUE:
        str = "INVALID_VALUE";
        break;
      case GL_OUT_OF_MEMORY:
        str = "OUT_OF_MEMORY";
        break;
      case GL_INVALID_FRAMEBUFFER_OPERATION:
        str = "INVALID_FRAMEBUFFER_OPERATION";
        break;
    }

    // cerr << "GL_" << str.c_str() << "\n";
    cerr << "GL_" << str.c_str() << " - " << filename << ":" << line << "\n";
    result = true;
  }

  return result;
}

static char *getFileContents(const char *filename) {
  FILE *file;
  long fileSize;
  char *buffer; // this can get leaked if an exception is thrown before it's returned

  file = fopen(filename, "rb");
  if (!file) {
    throw runtime_error(string("Failed to open file: ") + filename);
  }

  // how big is the file?
  fseek(file, 0L, SEEK_END);
  fileSize = ftell(file);
  rewind(file);

  // make a buffer big enough to hold the file
  buffer = (char *) calloc(fileSize + 1, 1);
  if (!buffer) {
    throw runtime_error(string("Failed to create buffer for: ") + filename);
  }

  // copy the file into the buffer
  int result = fread(buffer, fileSize, 1, file);
  if (result != 1) {
    throw runtime_error(string("Failed to copy file: ") + filename);
  }

  fclose(file);
  return buffer;
}

static GLuint compileShader(const char *filename, GLenum shaderType) {
  char *fileContents;
  try {
    fileContents = getFileContents(filename);
  } catch (exception &e) {
    cerr << "Exception caught: " << e.what() << "\n";
  }

  GLuint shader = glCreateShader(shaderType);
  glShaderSource(shader, 1, &fileContents, NULL);
  glCompileShader(shader);

  // Print shader compile errors.
  GLint status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (status != GL_TRUE) {
    cerr << "Failed to compile shader\n";
    char buffer[512];
    glGetShaderInfoLog(shader, 512, NULL, buffer);
    cerr << buffer << "\n";

    throw runtime_error(string("Failed to compile shader: ") + filename);
  }

  free(fileContents);
  return shader;
}

static GLuint generateShaderProgram(const char *vertSource, const char *fragSource) {
  // Load the shaders from the filesystem.
  GLuint vertShader = compileShader(vertSource, GL_VERTEX_SHADER);
  GLuint fragShader = compileShader(fragSource, GL_FRAGMENT_SHADER);
  checkErrors();

  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertShader);
  glAttachShader(shaderProgram, fragShader);
  checkErrors();

  glLinkProgram(shaderProgram);
  checkErrors();

  return shaderProgram;
}

GLuint generateShaderProgram(string vertSource, string fragSource) {
  return generateShaderProgram(vertSource.c_str(), fragSource.c_str());
}
