#pragma once
// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;
// GL Includes
#include <GL/glew.h> // Contains all the necessery OpenGL includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "lodepng.h"
#include <../assimp/Importer.hpp>
#include <../assimp/scene.h>
#include <../assimp/postprocess.h>

#include "Mesh.h"

GLint TextureFromFile(const char* path, string directory);

class Model
{
public:
	/*  Functions   */
	// Constructor, expects a filepath to a 3D model.
	Model(GLchar* path, bool loadTangent)
	{
		this->loadModel(path);
		this->loadTangent = loadTangent;
	}

	// Draws the model, and thus all its meshes
	void Draw(Shader shader)
	{
		//if (!instanced)
		//	for (GLuint i = 0; i < this->meshes.size(); i++)
		//	{
		//		this->meshes[i].Draw(shader);
		//	}
		//else
			for (GLuint i = 0; i < this->meshes.size(); i++)
				this->meshes[i].Draw(shader);
	}
	vector<Mesh> meshes;

private:
	/*  Model Data  */
	string directory;
	vector<Texture> textures_loaded;	// Stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
	bool loadTangent;

	/*  Functions   */
	// Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
	void loadModel(string path)
	{
		// Read file via ASSIMP
		Assimp::Importer importer;
		unsigned int flags;
		if (loadTangent)
			flags = aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace;
		else
			flags = aiProcess_Triangulate | aiProcess_FlipUVs;
		const aiScene* scene = importer.ReadFile(path, flags);//By setting aiProcess_Triangulate we tell Assimp that if the model does not (entirely) consist of triangles it should transform all the model's primitive shapes to triangles. The aiPorcess_FlipUVs flips the texture coordinates on the y-axis where necessary during processing

		// Check for errors
		if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
		{
			cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
			return;
		}
		// Retrieve the directory path of the filepath
		this->directory = path.substr(0, path.find_last_of('/'));

		// Process ASSIMP's root node recursively
		this->processNode(scene->mRootNode, scene);
	}

	// Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	void processNode(aiNode* node, const aiScene* scene)
	{
		// Process all the node's meshes (if any) Process each mesh located at the current node
		for (GLuint i = 0; i < node->mNumMeshes; i++)//check each of the node's mesh indices
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];//The node object only contains indices to index the actual objects in the scene. The scene contains all the data, node is just to keep stuff organized (like relations between nodes). Retrieve the corresponding mesh by indexing the scene's mMeshes array
			this->meshes.push_back(this->processMesh(mesh, scene));//returned mesh is then passed to the processMesh function that returns a Mesh object that we can store in the meshes list/vector
		}
		// Then do the same for each of its children. After we've processed all of the meshes (if any) we then recursively process each of the children nodes
		for (GLuint i = 0; i < node->mNumChildren; i++)//Once all the meshes have been processed we iterate through all of the node's children 
		{
			this->processNode(node->mChildren[i], scene);//and call the same processNode function for each of the node's children. Once a node no longer has any children the function stops executing
		}
	}

	Mesh processMesh(aiMesh* mesh, const aiScene* scene)
	{
		std::vector<Vertex> vertices;
		std::vector<GLuint> indices;
		std::vector<Texture> textures;

		//Process vertices. Walk through each of the mesh's vertices
		for (GLuint i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			glm::vec3 vector;// We declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
			// Positions
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.Position = vector;
			// Normals
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
			// Texture Coordinates
			if (mesh->mTextureCoords[0]) // Does the mesh contain texture coordinates?
			{
				glm::vec2 vec;
				//Assimp allows a model to have up to 8 different texture coordinates per vertex which we're not going to use so we only care about the first set of texture coordinates
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;
			}
			else
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);
			if (loadTangent)
			{
				//Tangent
				vector.x = mesh->mTangents[i].x;
				vector.y = mesh->mTangents[i].y;
				vector.z = mesh->mTangents[i].z;
				vertex.Tangents = vector;
			}
			vertices.push_back(vertex);
		}

		//Process indices
		/*Assimp's interface defined each mesh having an array of faces where each face represents a single primitive, which in our case (due to the aiProcess_Triangulate option) are always triangles. A face contains the indices that define which vertices we need to draw in what order for each primitive so if we iterate over all the faces and store all the face's indices in the indices vector we're all set:*/
		for (GLuint i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (GLuint j = 0; j < face.mNumIndices; j++)// Retrieve all indices of the face and store them in the indices vector
			{
				indices.push_back(face.mIndices[j]);
			}
		}

		//Process material
		/*Just like with nodes, a mesh only contains an index to a material object and to retrieve the actual material of a mesh we need to index the scene's mMaterials array*/
		if (mesh->mMaterialIndex >= 0) //to check if the mesh actually contains a material or not
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex]; //retrieve the aiMaterial object from the scene's mMaterials array
			// We assume a convention for sampler names in the shaders. Each diffuse texture should be named
			// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
			// Same applies to other texture as the following list summarizes:
			// Diffuse: texture_diffuseN
			// Specular: texture_specularN
			// Normal: texture_normalN

			// 1. Diffuse maps
			std::vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse"); //retrieve the textures from the material
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			// 2. Specular maps
			std::vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");//retrieve the textures from the material
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
			// 3. Reflection maps (Note that ASSIMP doesn't load reflection maps properly from wavefront objects, so we'll cheat a little by defining the reflection maps as ambient maps in the .obj file, which ASSIMP is able to load)
			//std::vector<Texture> reflectionMaps = this->loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_reflection");//retrieve the textures from the material
			//textures.insert(textures.end(), reflectionMaps.begin(), reflectionMaps.end());
			if (loadTangent)
			{
				// 4. Normal map
				std::vector<Texture> normalMaps = this->loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");//retrieve the textures from the material
				textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
			}
		}

		// Return a mesh object created from the extracted mesh data
		return Mesh(vertices, indices, textures, this->loadTangent);
	}

	// Checks all material textures of a given type and loads the textures if they're not loaded yet.
	// The required info is returned as a Texture struct.
	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
	{
		vector<Texture> textures;
		for (GLuint i = 0; i < mat->GetTextureCount(type); i++)//check the amount of textures stored in the material via its GetTextureCount function that expects one of the texture types we've given
		{
			aiString str;
			mat->GetTexture(type, i, &str);//retrieve each of the texture's file locations. Store the result in an aiString
			GLboolean skip = false;

			for (GLuint j = 0; j < textures_loaded.size(); j++)
			{
				if (textures_loaded[j].path == str)// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
				{
					textures.push_back(textures_loaded[j]);
					skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)
					break;
				}
			}
			if (!skip)// If texture hasn't been loaded already, load it
			{
				Texture texture;
				texture.id = TextureFromFile(str.C_Str(), this->directory); //loads a texture (with SOIL) for us and returns the texture's ID
				texture.type = typeName;
				texture.path = str;
				textures.push_back(texture);
				this->textures_loaded.push_back(texture); //Add to loaded textures. Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
			}
		}
		return textures;
	}
};




GLint TextureFromFile(const char* path, string directory)
{
	//Generate texture ID and load texture data 
	string filename = string(path);
	filename = directory + '/' + filename;
	GLuint textureID;
	glGenTextures(1, &textureID);
	std::vector<unsigned char> image; //the raw pixels
	unsigned width, height;

	//decode
	unsigned error = lodepng::decode(image, width, height, filename);

	//if there's an error, display it
	if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	image.clear();
	return textureID;
}