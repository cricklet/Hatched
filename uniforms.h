#ifndef UNIFORMS_H
#define UNIFORMS_H

#include <iostream>

#include <vector>
#include <map>
#include <string>
#include <functional>
using namespace std;

#define GLEW_STATIC
#include <GL/glew.h>

const string MODEL_TRANS = "unifModelTrans";
const string VIEW_TRANS = "unifViewTrans";
const string PROJ_TRANS = "unifProjTrans";
const string INV_VIEW_TRANS = "unifInvViewTrans";
const string INV_PROJ_TRANS = "unifInvProjTrans";
const string USE_TEXTURE = "unifUseTexture";
const string COLOR = "unifColor";
const string TEXTURE = "unifTexture";
const string NUM_TILES = "unifNumTiles"; // for old hatching shader
const string NUM_MIPS = "unifNumMips"; // for new hatching shader
const string NUM_TONES = "unifNumTones";
const string TILES_TEXTURE = "unifTilesTexture";
const string LIGHT_DIR = "unifLightDir";
const string DEPTHS = "unifDepths";
const string POSITIONS = "unifPositions";
const string NORMALS = "unifNormals";
const string BUFFER = "unifBuffer";
const string UVS = "unifUVs";
const string RANDOM = "unifRandom";

class Uniforms {
public:
  void add(GLint shaderProgram, vector<string> keys);
  GLuint get(string key) const;
  void print() const;

private:
  map<string, GLint> uniforms;
};

#endif
