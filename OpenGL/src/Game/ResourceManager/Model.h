#pragma once
#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../../lib/stb_image/stb_image.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "../shader/Shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

unsigned int TextureFromMyFile(const char* path, const string& directory, bool gamma = false);

class Model
{
public:
  /*  Model Data */
  vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
  vector<Mesh> meshes;
  string directory;
  bool gammaCorrection;

  /*  Functions   */
  // constructor, expects a filepath to a 3D model.
  Model(bool gamma = true) : gammaCorrection(gamma){
  }


  // draws the model, and thus all its meshes
  void Draw(Shader* shader)
  {
    for (unsigned int i = 0; i < meshes.size(); i++)
      meshes[i].Draw(shader);
  }

  // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
  void LoadModel(string const& path)
  {
    // read file via ASSIMP
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
      cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
      return;
    }
    // retrieve the directory path of the filepath
    directory = path.substr(0, path.find_last_of('/'));

    // process ASSIMP's root node recursively
    processNode(scene->mRootNode, scene);
  }

private:
  // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
  void processNode(aiNode * node, const aiScene * scene)
  {
    // process each mesh located at the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
      // the node object only contains indices to index the actual objects in the scene. 
      // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
      aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
      meshes.push_back(processMesh(mesh, scene));
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
      processNode(node->mChildren[i], scene);
    }

  }

  Mesh processMesh(aiMesh * mesh, const aiScene * scene) {
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;

    // 读取顶点数据
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
      Vertex vertex;
      glm::vec3 vector;
      // positions
      vector.x = mesh->mVertices[i].x;
      vector.y = mesh->mVertices[i].y;
      vector.z = mesh->mVertices[i].z;
      vertex.Position = vector;
      // normals
      if (mesh->mNormals != nullptr) {
        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        vertex.Normal = vector;
      }
      // texture coordinates
      if (mesh->mTextureCoords[0]) {
        glm::vec2 vec;
        vec.x = mesh->mTextureCoords[0][i].x;
        vec.y = mesh->mTextureCoords[0][i].y;
        vertex.TexCoords = vec;
      } else {
        vertex.TexCoords = glm::vec2(0.0f, 0.0f);
      }
      // tangent
      if (mesh->mTangents != nullptr) {
        vector.x = mesh->mTangents[i].x;
        vector.y = mesh->mTangents[i].y;
        vector.z = mesh->mTangents[i].z;
        vertex.Tangent = vector;
      }
      // bitangent
      if (mesh->mBitangents != nullptr) {
        vector.x = mesh->mBitangents[i].x;
        vector.y = mesh->mBitangents[i].y;
        vector.z = mesh->mBitangents[i].z;
        vertex.Bitangent = vector;
      }
      vertices.push_back(vertex);
    }
    // 读取面索引数据
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
      aiFace face = mesh->mFaces[i];
      for (unsigned int j = 0; j < face.mNumIndices; j++)
        indices.push_back(face.mIndices[j]);
    }
    // 读取材质信息
    if (mesh->mMaterialIndex >= 0) {
      // process materials
      aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
      // 加载颜色
      textures.push_back(loadMaterialColor(material));
      // 加载贴图
      vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "material.diffuse");
      textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

      vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "material.specular");
      textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

      std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "material.normal");
      textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

      std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "material.height");
      textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
    }

    return Mesh(vertices, indices, textures);
  }


 Texture loadMaterialColor(aiMaterial* mat) {
    aiColor3D color(1.0f, 1.0f, 1.0f);
    Texture texture;
    texture.type = "color";
    mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
    texture.diffuseColor = glm::vec3(color.r, color.g, color.b);
    color = aiColor3D(1.0f, 1.0f, 1.0f);
    mat->Get(AI_MATKEY_COLOR_AMBIENT, color);
    texture.ambientColor = glm::vec3(color.r, color.g, color.b);
    color = aiColor3D(1.0f, 1.0f, 1.0f);
    mat->Get(AI_MATKEY_COLOR_SPECULAR, color);
    texture.specularColor = glm::vec3(color.r, color.g, color.b);
    float shininess = 32;
    mat->Get(AI_MATKEY_SHININESS, shininess);
    texture.shininess = shininess;
    return texture;
  }

  // checks all material textures of a given type and loads the textures if they're not loaded yet.
  // the required info is returned as a Texture struct.
  vector<Texture> loadMaterialTextures(aiMaterial * mat, aiTextureType type, string typeName)
  {
    vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
      aiString str;
      mat->GetTexture(type, i, &str);
      // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
      bool skip = false;
      for (unsigned int j = 0; j < textures_loaded.size(); j++)
      {
        if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
        {
          textures.push_back(textures_loaded[j]);
          skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
          break;
        }
      }
      if (!skip)
      {   // if texture hasn't been loaded already, load it
        Texture texture;
        texture.id = TextureFromMyFile(str.C_Str(), this->directory);
        texture.type = typeName;
        texture.path = str.C_Str();
        textures.push_back(texture);
        textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
      }
    }
    return textures;
  }
};

#endif
