#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string.h>

#include "Misc.h"
#include "MapParser.h"
#include <map>





class ObjAnimated
{
public:
	ObjAnimated(std::string filename, float scale);
	~ObjAnimated();

	void prepare_to_draw(unsigned int shaderProgram, std::string filename_texture);

	void draw(glm::mat4 ViewProjection_in, light_data light, glm::vec3 camPos, glm::mat4 Model_in);

private:
	unsigned vbo = 0,
		texturecoord_buffer = 0,
		texture_buffer = 0,
		normals_buffer = 0,
		bonesIds_buffer = 0,
		weights_buffer = 0,
		vao = 0,
		shaderProgram = 0,
		image_width = 0,
		image_height = 0,
		bones_counter = 0;

	std::map<std::string, BoneInfo> bone_info_map;


	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 ViewProjection = glm::mat4(0.0f);

	obj_data_2 mesh_data;
	float coeff = 1.0f,
		shininess = 32.0f;

	void processMesh(aiMesh* mesh, const aiScene* scene);
	void processNode(aiNode* child, const aiScene* scene);
	void parse_mesh_bones(aiMesh* mesh);
	void setVertexBoneData(Vertex& vertex, int boneID, float weight);
};

void ObjAnimated::prepare_to_draw(unsigned int shaderProgram_in, std::string filename_texture)
{
	/*
		TEXTURES
	*/

	
	std::vector<uint8_t> image_data_texture;

	unsigned error = lodepng::decode(image_data_texture, image_width, image_height, filename_texture);
	if (error)
	{
		printf("Error lodePNG objimporter: %u", error);
	}


	// load image texture

	for (unsigned h = 0; h < image_height; h++)
	{
		for (unsigned w = 0; w < image_width; w++)
		{
			for (uint8_t i = 0; i < 4; i++)
			{
				mesh_data.material.texture.data[h][(w * 4) + i] = image_data_texture[((w * 4) + i) + (h * image_width * 4)];

			}
		}
	}

	
	/* ******************************** */

	/*
		Shader Program
	*/
	shaderProgram = shaderProgram_in;

	glUseProgram(shaderProgram);

	// bind the vao
	glBindVertexArray(vao);
	// bind the VBO 
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	// Model Vertex positions
	glBufferData(GL_ARRAY_BUFFER, mesh_data.vertices.size() * sizeof(Vertex), mesh_data.vertices.data(), GL_STATIC_DRAW);
	glVertexAttribIPointer(0, 4, GL_FLOAT_VEC3, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::Position));
	glEnableVertexAttribArray(0);
	/* ****************************************** */

	// Model Vertex normals
	glBufferData(GL_ARRAY_BUFFER, mesh_data.vertices.size() * sizeof(Vertex), mesh_data.vertices.data(), GL_STATIC_DRAW);
	glVertexAttribIPointer(2, 4, GL_FLOAT_VEC3, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::Normal));
	glEnableVertexAttribArray(2);
	/* ****************************************** */

	// Model Vertex boneIDs
	glBufferData(GL_ARRAY_BUFFER, mesh_data.vertices.size() * sizeof(Vertex), mesh_data.vertices.data(), GL_STATIC_DRAW);
	glVertexAttribIPointer(4, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::m_BoneIDs));
	glEnableVertexAttribArray(4);
	/* ****************************************** */

	// Model Vertex weights
	glBufferData(GL_ARRAY_BUFFER, mesh_data.vertices.size() * sizeof(Vertex), mesh_data.vertices.data(), GL_STATIC_DRAW);
	glVertexAttribIPointer(5, 4, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::m_Weights));
	glEnableVertexAttribArray(5);
	/* ****************************************** */


	/* TEXTURE */
	
	// Model Vertex texcoords
	glBufferData(GL_ARRAY_BUFFER, mesh_data.vertices.size() * sizeof(Vertex), mesh_data.vertices.data(), GL_STATIC_DRAW);
	glVertexAttribIPointer(3, 4, GL_FLOAT_VEC2, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::TexCoords));
	glEnableVertexAttribArray(3);
	/* ****************************************** */


	// Texture
	/*
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, texture_buffer);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, mesh_data.material.texture.data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glGenerateMipmap(GL_TEXTURE_2D);
	*/

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(2);

	/* *********************** */




}


void ObjAnimated::draw(glm::mat4 ViewProjection_in, light_data light, glm::vec3 camPos, glm::mat4 Model_in)
{
	glUseProgram(shaderProgram);

	Model = Model_in;
	ViewProjection = ViewProjection_in;

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo); // vertices



	/* transformation */

	GLuint uniLoc = glGetUniformLocation(shaderProgram, "Model");

	glUniformMatrix4fv(uniLoc, 1, GL_FALSE, glm::value_ptr(Model));

	uniLoc = glGetUniformLocation(shaderProgram, "ViewProjection");

	glUniformMatrix4fv(uniLoc, 1, GL_FALSE, glm::value_ptr(ViewProjection));

	/* *********************  */


	// eye normal
	uniLoc = glGetUniformLocation(shaderProgram, "eyePos");
	glUniform3f(uniLoc, camPos.x, camPos.y, camPos.z);




	/* TEXTURE */

	// texture
	uniLoc = glGetUniformLocation(shaderProgram, "texture0");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_buffer);

	glUniform1i(uniLoc, GL_TEXTURE0);


	/* LUMIERE */


	// diffuse
	uniLoc = glGetUniformLocation(shaderProgram, "light.diffuse");
	glUniform3f(uniLoc, light.diffuse.x, light.diffuse.y, light.diffuse.z);

	// specular
	uniLoc = glGetUniformLocation(shaderProgram, "light.specular");
	glUniform3f(uniLoc, light.specular.x, light.specular.y, light.specular.z);

	// position
	uniLoc = glGetUniformLocation(shaderProgram, "light.position");
	glUniform3f(uniLoc, light.position.x, light.position.y, light.position.z);

	/* ********************** */


	/* PHONG */


	// ambient
	uniLoc = glGetUniformLocation(shaderProgram, "material.ambient");
	glUniform3f(uniLoc, mesh_data.material.ambiant.x, mesh_data.material.ambiant.y, mesh_data.material.ambiant.z);

	// shininess
	uniLoc = glGetUniformLocation(shaderProgram, "material.shininess");
	glUniform1f(uniLoc, mesh_data.material.shininess);

	// specular
	uniLoc = glGetUniformLocation(shaderProgram, "material.specular");
	glUniform3f(uniLoc, mesh_data.material.specular.x, mesh_data.material.specular.y, mesh_data.material.specular.z);

	// diffuse
	uniLoc = glGetUniformLocation(shaderProgram, "material.diffuse");
	glUniform3f(uniLoc, mesh_data.material.diffuse.x, mesh_data.material.diffuse.y, mesh_data.material.diffuse.z);



	// reflective
	uniLoc = glGetUniformLocation(shaderProgram, "material.reflectivity");
	glUniform1f(uniLoc, mesh_data.material.reflectivity);

	// reflectivity
	uniLoc = glGetUniformLocation(shaderProgram, "material.refract_indice");
	glUniform1f(uniLoc, mesh_data.material.refract_indice);



	//draw
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)offsetof(Vertex, Vertex::Position));
	//glDrawElements(GL_TRIANGLES, my_map_data.map_indices.size(), GL_UNSIGNED_INT, (void*)0);
}


void ObjAnimated::setVertexBoneData(Vertex& vertex, int boneID, float weight)
{
	for (int i = 0; i < MAX_BONES_WEIGHTS; i++)
	{
		if (vertex.m_BoneIDs[i] < 0)
		{
			vertex.m_Weights[i] = weight;
			vertex.m_BoneIDs[i] = boneID;
			break;
		}
	}
}

void ObjAnimated::parse_mesh_bones(aiMesh* mesh)
{
	// pour chaque bones
	for (int boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++)
	{
		int boneID = -1;
		std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();


		if (bone_info_map.find(boneName) == bone_info_map.end())
		{
			BoneInfo newboneinfo;
			newboneinfo.id = bones_counter;
			newboneinfo.offset = Misc().convertAssimpMatriceToGLM(mesh->mBones[boneIndex]->mOffsetMatrix);
			bone_info_map[boneName] = newboneinfo;
			boneID = bones_counter++;
		}
		else
		{
			boneID = bone_info_map[boneName].id;
		}
		// on controle si boneID n'est pas égal à son état initial
		assert(boneID != -1);





		auto weights = mesh->mBones[boneIndex]->mWeights;
		int numweights = mesh->mBones[boneIndex]->mNumWeights;

		for (int weight_index = 0; weight_index < numweights; weight_index++)
		{
			int vertexID = weights[weight_index].mVertexId;
			float weight = weights[weight_index].mWeight;

			// on controle si l'id n'est pas supérieur à la taille des vertices
			assert(vertexID <= mesh_data.vertices.size());
			
			setVertexBoneData(mesh_data.vertices[vertexID], boneID, weight);
		}

	}
}

void ObjAnimated::processMesh(aiMesh* mesh, const aiScene* scene)
{

	for (unsigned i = 0; i < mesh->mNumVertices; i++)
	{
	
		Vertex vertex;




		// vertices 
		vertex.Position.x = mesh->mVertices[i].x;
		vertex.Position.y = mesh->mVertices[i].y;
		vertex.Position.z = mesh->mVertices[i].z;


		// normals

		if (mesh->HasNormals())
		{
			vertex.Normal.x = mesh->mNormals[i].x;
			vertex.Normal.y = mesh->mNormals[i].y;
			vertex.Normal.z = mesh->mNormals[i].z;
		}


		// textures
		if (mesh->mTextureCoords[0])
		{
			vertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
			vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
		}
		else
		{
			vertex.TexCoords = glm::vec2(0.0f);
		}



		if (mesh->HasBones())
		{
			// set bone data to default
			for (int bone = 0; bone < mesh->mNumBones; bone++)
			{
				vertex.m_BoneIDs.push_back(-1);
				vertex.m_Weights.push_back(0.0f);
			}
			parse_mesh_bones(mesh);
		}


		mesh_data.vertices.push_back(vertex);




	}


}

void ObjAnimated::processNode(aiNode* node, const aiScene* scene)
{


	for (unsigned i = 0; i < node->mNumChildren; i++)
	{
		aiNode* child = node->mChildren[i];

		int total_vertices = 0;

		for (unsigned j = 0; j < child->mNumMeshes; j++)
		{

			aiMesh* mesh = scene->mMeshes[child->mMeshes[j]];

			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];


			// diffuse
			aiColor3D color;
			material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
			mesh_data.material.diffuse = glm::vec3(color.r, color.g, color.b);

			// specular
			material->Get(AI_MATKEY_COLOR_SPECULAR, color);
			mesh_data.material.specular = glm::vec3(color.r, color.g, color.b);

			// refract indice
			float refract;
			material->Get(AI_MATKEY_REFRACTI, refract);
			mesh_data.material.refract_indice = refract;

			// reflectivity
			float reflectivity;
			material->Get(AI_MATKEY_REFLECTIVITY, reflectivity);
			mesh_data.material.reflectivity = reflectivity;

			// ambiant
			material->Get(AI_MATKEY_COLOR_AMBIENT, color);
			mesh_data.material.ambiant = glm::vec3(color.r, color.g, color.b);

			// shininess
			float shininess;
			material->Get(AI_MATKEY_SHININESS, shininess);
			mesh_data.material.shininess = shininess;

			processMesh(mesh, scene);


		}
		if (child->mNumChildren > 0)
		{
			processNode(child, scene);
		}

	}

}

ObjAnimated::ObjAnimated(std::string filename, float scale)
{
	coeff = scale;

	// generate objects buffers
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &normals_buffer);
	glGenBuffers(1, &bonesIds_buffer);
	glGenBuffers(1, &weights_buffer);
	glGenVertexArrays(1, &vao);
	glGenVertexArrays(1, &texturecoord_buffer);
	glGenTextures(1, &texture_buffer);



	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate);




	if (!scene && !importer.GetErrorString() && !(scene->mNumMeshes > 0))
	{
		printf("Erreur importation: %s\n", importer.GetErrorString());
		exit(1);
	}


	aiNode* node = scene->mRootNode;

	if (!node)
	{
		exit(1);
	}

	//printf("%i\n", node->mNumChildren);

	processNode(node, scene);
}

ObjAnimated::~ObjAnimated()
{
	glDeleteBuffers(1, &vbo);
	glDeleteTextures(1, &texture_buffer);
	glDeleteBuffers(1, &texturecoord_buffer);
	glDeleteBuffers(1, &normals_buffer);
	glDeleteTextures(1, &bonesIds_buffer);
	glDeleteTextures(1, &weights_buffer);
	glDeleteBuffers(1, &vao);
}