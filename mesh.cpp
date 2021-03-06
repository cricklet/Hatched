#include "mesh.h"
#include "helper.h"

Mesh::Mesh(const vector<Vertex> vertices, vector<GLuint> indices) {
  this->vertices = vertices;
  // cout << "Created mesh with # " << vertices.size() << " vertices.\n";

  this->indices = indices;

  this->color = glm::vec3(random(1), random(1), random(1));

  glGenVertexArrays(1, &this->vao);
  glGenBuffers(1, &this->vbo);
  glGenBuffers(1, &this->ebo);

  glBindVertexArray(this->vao);
  glBindBuffer(GL_ARRAY_BUFFER, this->vbo);

  glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

  glBindVertexArray(0);
  checkErrors();
}

Mesh::~Mesh() {
  glDeleteVertexArrays(1, &this->vao);
  glDeleteBuffers(1, &this->vbo);
  glDeleteBuffers(1, &this->ebo);
}

void Mesh::BindToShader(GLuint shaderProgram) {
  glUseProgram(shaderProgram);
  glBindVertexArray(this->vao);
  glBindBuffer(GL_ARRAY_BUFFER, this->vbo);

  GLuint vertexStride = sizeof(Vertex);

  GLint posAttrib = glGetAttribLocation(shaderProgram, "inVertPosition");
  void *posOffset = (GLvoid*) 0;
  glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, vertexStride, posOffset);
  glEnableVertexAttribArray(posAttrib);
  checkErrors();

  GLint normAttrib = glGetAttribLocation(shaderProgram, "inVertNorm");
  if (normAttrib != -1) {
    void *normOffset = (GLvoid*) offsetof(Vertex, normal);
    glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, vertexStride, normOffset);
    glEnableVertexAttribArray(normAttrib);
    checkErrors();
  }

  GLint uvAttrib = glGetAttribLocation(shaderProgram, "inVertUV");
  if (uvAttrib != -1) {
    void *normOffset = (GLvoid*) offsetof(Vertex, uv);
    glVertexAttribPointer(uvAttrib, 2, GL_FLOAT, GL_FALSE, vertexStride, normOffset);
    glEnableVertexAttribArray(uvAttrib);
    checkErrors();
  }
}

void Mesh::Render(Uniforms uniforms, glm::mat4 parentTransform) {
  auto transformVal = glm::value_ptr(parentTransform);
  glUniformMatrix4fv(uniforms.get(MODEL_TRANS), 1, GL_FALSE, transformVal);

  // load the first texture
  GLint colorUnif = uniforms.get(COLOR);
  if (colorUnif != -1) {
    glUniform3fv(colorUnif, 1, glm::value_ptr(this->color));
  }
  checkErrors();

  // draw the mesh!
  glBindVertexArray(this->vao);
  glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
  checkErrors();

  glBindVertexArray(0);
  checkErrors();
}

Bounds Mesh::GetBounds() {
  Bounds b;
  for (Vertex v : this->vertices) {
    glm::vec3 p = v.position;

    if (p.x < b.minx)
      b.minx = p.x;
    if (p.y < b.miny)
      b.miny = p.y;
    if (p.z < b.minz)
      b.minz = p.z;

    if (p.x > b.minx)
      b.maxx = p.x;
    if (p.y > b.miny)
      b.maxy = p.y;
    if (p.z > b.minz)
      b.maxz = p.z;
  }

  return b;
}
