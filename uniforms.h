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
const string TILES_TEXTURE = "unifTilesTexture";

const string HATCH0_TEXTURE = "unifHatch0";
const string HATCH1_TEXTURE = "unifHatch1";
const string HATCH2_TEXTURE = "unifHatch2";
const string HATCH3_TEXTURE = "unifHatch3";
const string HATCH4_TEXTURE = "unifHatch4";
const string HATCH5_TEXTURE = "unifHatch5";

const string LIGHT_DIR = "unifLightDir";

const string LIGHT_POSITIONS = "unifLightPositions";
const string NUM_LIGHTS = "unifNumLights";

const string DEPTHS = "unifDepths";
const string POSITIONS = "unifPositions";
const string NORMALS = "unifNormals";
const string UVS = "unifUVs";

const string BUFFER = "unifBuffer";

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
