#include "model.h"
#include "mesh.h"
#include "helper.h"

template<typename Func>
static void recursivelyProcess(aiNode* node, const aiScene* scene, Func processMesh) {
  for (GLuint i = 0; i < node->mNumMeshes; i++) {
    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
    processMesh(mesh, scene);
  }
  for (GLuint i = 0; i < node->mNumChildren; i++) {
    aiNode *child = node->mChildren[i];
    recursivelyProcess(child, scene, processMesh);
  }
}

static Mesh generateMesh(aiMesh *mesh, const aiScene *scene) {
  vector<Vertex> vertices;
  vector<GLuint> indices;

  // Process vertices
  for (GLuint i = 0; i < mesh->mNumVertices; i++) {
    aiVector3D vertex = mesh->mVertices[i];
    aiVector3D normal = mesh->mNormals[i];

    Vertex v;
    v.position.x = vertex.x;
    v.position.y = vertex.y;
    v.position.z = vertex.z;
    v.normal.x = normal.x;
    v.normal.y = normal.y;
    v.normal.z = normal.z;

    if (mesh->mMaterialIndex >= 0) {
      aiVector3D uv = mesh->mTextureCoords[0][i];
      v.uv.x = uv.x;
      v.uv.y = uv.y;
    } else {
      v.uv.x = 0;
      v.uv.y = 0;
    }

    vertices.push_back(v);
  }

  // Process indices
  for (GLuint i = 0; i < mesh->mNumFaces; i++) {
    aiFace face = mesh->mFaces[i];
    for (GLuint j = 0; j < face.mNumIndices; j++)
      indices.push_back(face.mIndices[j]);
  }

  return Mesh(vertices, indices);
}

static Bounds computeBounds(vector<Mesh> meshes) {
  Bounds overall = meshes[0].GetBounds();
  for (Mesh mesh : meshes) {
    Bounds local = mesh.GetBounds();
    if (local.minx < overall.minx) overall.minx = local.minx;
    if (local.maxx > overall.maxx) overall.maxx = local.maxx;
    if (local.miny < overall.miny) overall.miny = local.miny;
    if (local.maxy > overall.maxy) overall.maxy = local.maxy;
    if (local.minz < overall.minz) overall.minz = local.minz;
    if (local.maxz > overall.maxz) overall.maxz = local.maxz;
  }

  return overall;
}

static float maxDimension(Bounds b) {
  return max(max(b.maxx - b.minx, b.maxy - b.miny), b.maxz - b.minz);
}

Model::Model(string path) {
  Assimp::Importer importer;
  unsigned int flags = aiProcess_GenNormals | aiProcess_Triangulate | aiProcess_FlipUVs;

  const aiScene* scene = importer.ReadFile(path, flags);
  if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    cerr << "Couldn't load scene: " << importer.GetErrorString() << endl;
    return;
  }

  string directory = path.substr(0, path.find_last_of('/'));

  auto addMesh = [&] (aiMesh *mesh, const aiScene *scene) {
    this->meshes.push_back(generateMesh(mesh, scene));
  };

  recursivelyProcess(scene->mRootNode, scene, addMesh);

  this->bounds = computeBounds(this->meshes);
  cout << "Model bounds: "
      << this->bounds.minx << ", "
      << this->bounds.miny << ", "
      << this->bounds.minz << " - "
      << this->bounds.maxx << ", "
      << this->bounds.maxy << ", "
      << this->bounds.maxz << "\n";
}

void Model::Render(const Uniforms &uniforms) {
  glm::mat4 world = glm::translate(glm::mat4(), glm::vec3(0,0,-0.5));

  glm::mat4 model = glm::mat4();
  model = glm::rotate(model, (float) (M_PI / 2.0), glm::vec3(1, 0, 0));
  float scale = 1.0f / maxDimension(this->bounds);
  model = glm::scale(model, glm::vec3(scale, scale, scale));

  glUniformMatrix4fv(uniforms.modelTrans, 1, GL_FALSE, glm::value_ptr(world * model));

  for (Mesh m : this->meshes) {
    m.Render(uniforms);
    checkErrors();
  }
}

void Model::BindToShader(GLuint shaderProgram) {
  for (Mesh m : this->meshes) {
    m.BindToShader(shaderProgram);
    checkErrors();
  }
}
