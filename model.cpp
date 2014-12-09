#include "model.h"

Model::Model(string path) {
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

  if(!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    cout << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
    return;
  }

  this->directory = path.substr(0, path.find_last_of('/'));

  this->ProcessNode(scene->mRootNode, scene);
}

void
Model::Draw(const Uniforms &uniforms) {
  for (Mesh m : this->meshes) {
    m.Draw(uniforms);
  }
}

static vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type) {
  vector<Texture> textures;
  for(GLuint i = 0; i < mat->GetTextureCount(type); i++) {
    aiString str;
    mat->GetTexture(type, i, &str);
    cout << str.C_Str() << "    \n";
    /*Texture texture;
    texture.id = TextureFromFile(str.C_Str(), this->directory);
    texture.type = typeName;
    texture.path = str;
    textures.push_back(texture); */
  }
  return textures;
}  

void
Model::ProcessNode(aiNode *node, const aiScene *scene) {
  // Process all the node's meshes (if any)
  for(GLuint i = 0; i < node->mNumMeshes; i++) {
    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]]; 
    this->meshes.push_back(processMesh(mesh, scene));
  }
  // Then do the same for each of its children
  for(GLuint i = 0; i < node->mNumChildren; i++) {
    ProcessNode(node->mChildren[i], scene);
  }
}
