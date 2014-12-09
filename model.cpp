#include "model.h"

Model::Model(string path) {
  Assimp::Importer importer;
  unsigned int flags = aiProcess_GenNormals | aiProcess_Triangulate | aiProcess_FlipUVs;
  aiScene* scene = importer.ReadFile(path, flags);

  if(!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    cerr << "Couldn't load scene: " << importer.GetErrorString() << endl;
    return;
  }

  string directory = path.substr(0, path.find_last_of('/'));
}

void
Model::Render(const Uniforms &uniforms) {
  for (Mesh m : this->meshes) {
    m.Render(uniforms);
  }
}
