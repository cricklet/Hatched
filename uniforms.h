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

typedef map<string, GLint> UniformsMap;
typedef function<GLint (string)> UniformGetter;

const string MODEL_TRANS = "unifModelTrans";
const string VIEW_TRANS = "unifViewTrans";
const string PROJ_TRANS = "unifProjTrans";
const string USE_TEXTURE = "unifUseTexture";
const string COLOR = "unifColor";
const string TEXTURE = "unifTexture";
const string NUM_TILES = "unifNumTiles";
const string TILES_TEXTURE = "unifTilesTexture";
const string LIGHT_DIR = "unifLightDir";
const string DEPTHS = "unifDepths";
const string POSITIONS = "unifPositions";
const string NORMALS = "unifNormals";
const string RANDOM = "unifRandom";

const vector<string> TRANS_UNIFORMS = {
    MODEL_TRANS, VIEW_TRANS, PROJ_TRANS
};

const vector<string> TEXTURE_UNIFORMS = {
    USE_TEXTURE, TEXTURE
};

const vector<string> TILE_UNIFORMS = {
    NUM_TILES, TILES_TEXTURE
};

void addUniforms(UniformsMap &uniforms, GLint shaderProgram, vector<string> keys);
void addUniforms(UniformsMap &uniforms, GLint shaderProgram, string key);
UniformGetter generateUniformGetter(const UniformsMap &uniforms);
void printUniforms(UniformsMap &uniforms);

#endif
