#include "mesh.h"

Mesh::Mesh(const vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures) {
  this->vertices = vertices;
  this->indices = indices;
  this->textures = textures;

  glGenVertexArrays(1, &this->vao);
  glGenBuffers(1, &this->vbo);
  glGenBuffers(1, &this->ebo);
  
  glBindVertexArray(this->vao);
  glBindBuffer(GL_ARRAY_BUFFER, this->vbo);

  glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), 
	       &this->vertices[0], GL_STATIC_DRAW);  

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), 
	       &this->indices[0], GL_STATIC_DRAW);

  // positions
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
			(GLvoid*) 0);
  // normals
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
			(GLvoid*) offsetof(Vertex, normal));
  // uv
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
			(GLvoid*) offsetof(Vertex, uv));

  glBindVertexArray(0);
}

void
Mesh::Draw(const Uniforms &uniforms) {
  // load the first texture
  GLuint textureIndex = this->textures[0].index;
  glUniform1i(uniforms.texture, textureIndex);
  
  // draw the mesh!
  glBindVertexArray(this->vao);
  glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}
