#pragma once
#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../shader/Shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
using namespace std;

struct Vertex {
  // position
  glm::vec3 Position;
  // normal
  glm::vec3 Normal;
  // texCoords
  glm::vec2 TexCoords;
  // tangent
  glm::vec3 Tangent;
  // bitangent
  glm::vec3 Bitangent;
};

struct Texture {
  unsigned int id;
  string type;
  string path;
  glm::vec3 diffuseColor;
  glm::vec3 ambientColor;
  glm::vec3 specularColor;
  float shininess;
};

class Mesh {
public:
  vector<Vertex> vertices;
  vector<unsigned int> indices;
  vector<Texture> textures;
  unsigned int VAO;

  Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
  {
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;
    setupMesh();
  }

  void Draw(Shader* shader) {
    shader->SetVector3f("light.ambient", glm::vec3(0));
    shader->SetVector3f("light.diffuse", glm::vec3(0));
    shader->SetVector3f("light.specular", glm::vec3(0));
    shader->SetFloat("light.shininess", 32);
    shader->SetInteger("material.specular", 0);
    shader->SetInteger("material.diffuse", 0);
    shader->SetInteger("material.normal", 0);
    shader->SetInteger("material.height", 0);
    bool hasColor = false;
    bool hasTexture = false;
    for (unsigned int i = 0; i < textures.size(); i++) {
      string name = textures[i].type;
      if (name == "color") {
        hasColor = true;
        shader->SetVector3f("light.ambient", textures[i].ambientColor);
        shader->SetVector3f("light.diffuse", textures[i].diffuseColor);
        shader->SetVector3f("light.specular", textures[i].specularColor);
        if (textures[i].shininess != 0) {
          shader->SetFloat("light.shininess", textures[i].shininess);
        }
      } else {
        hasTexture = true;
        glActiveTexture(GL_TEXTURE0 + i);
        shader->SetInteger(name.c_str(), i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
      }
    }
    shader->SetInteger("hasColor", hasColor);
    shader->SetInteger("hasTexture", hasTexture);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
  }

private:
  unsigned int VBO, EBO;

  void setupMesh()
  {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

    glBindVertexArray(0);
  }
};
#endif
