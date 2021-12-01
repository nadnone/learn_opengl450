#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string.h>

#include "Misc.h"
#include "MapParser.h"

class ObjImporter
{
public:
	ObjImporter(std::string filename, std::string filename_texture_in, float scale);
	~ObjImporter();


	void draw(glm::mat4 ViewProjection_in, light_data light, glm::vec3 camPos, glm::mat4 Model_in);
	void prepare_to_draw(unsigned shaderProgram);


	glm::mat4 translate(glm::vec3 translation_direction, glm::mat4 Model_in);
	glm::mat4 rotate(float angle, glm::mat4 Model_in, glm::vec3 axe);

private:
	std::string filename_texture;

	unsigned vbo = 0,
		texturecoord_buffer = 0,
		texture_buffer = 0,
		normals_buffer = 0,
		vao = 0,
		shaderProgram = 0,
		image_width = 0,
		image_height = 0;


	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 ViewProjection = glm::mat4(0.0f);

	obj_data mesh_data;
	float coeff = 1.0f,
		shininess = 32.0f;

	void processMesh(aiMesh* mesh, const aiScene* scene);
	void processNode(aiNode* child, const aiScene* scene);

};

void ObjImporter::processMesh(aiMesh *mesh, const aiScene* scene)
{
	std::vector<unsigned> indices;


	for (unsigned i = 0; i < mesh->mNumVertices; i++)
	{
		// vertices 
		mesh_data.map_vertices.push_back(mesh->mVertices[i].x * coeff);
		mesh_data.map_vertices.push_back(mesh->mVertices[i].z * coeff);
		mesh_data.map_vertices.push_back(mesh->mVertices[i].y * coeff);		

		// normals
		mesh_data.normals.push_back(mesh->mNormals[i].x);
		mesh_data.normals.push_back(mesh->mNormals[i].y);
		mesh_data.normals.push_back(mesh->mNormals[i].z);

		// textures
		if (mesh->mTextureCoords[0])
		{
			mesh_data.textures_coord.push_back(mesh->mTextureCoords[0][i].x);
			mesh_data.textures_coord.push_back(mesh->mTextureCoords[0][i].y);
		}
		else
		{
			mesh_data.textures_coord.push_back(0.0f);
			mesh_data.textures_coord.push_back(0.0f);
		}
	

		


	}


}
void ObjImporter::processNode(aiNode* node, const aiScene* scene)
{


	for (unsigned i = 0; i < node->mNumChildren; i++)
	{
		aiNode* child = node->mChildren[i];
		

		for (unsigned j = 0; j < child->mNumMeshes; j++)
		{
			aiMesh* mesh = scene->mMeshes[child->mMeshes[j]];

			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];


			// on ne prend qu'un seul material
			if (j == 0)
			{
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
				
			}

			processMesh(mesh, scene);

		}
		if (child->mNumChildren > 0)
		{
			processNode(child, scene);
		}

	}

}

ObjImporter::ObjImporter(std::string filename, std::string filename_texture_in, float scale)
{

	coeff = scale;

	filename_texture = filename_texture_in;

	// generate objects buffers
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &normals_buffer);
	//glGenBuffers(1, &indices_buffer);
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

ObjImporter::~ObjImporter()
{
	glDeleteTextures(1, &texture_buffer);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &texturecoord_buffer);
	glDeleteBuffers(1, &normals_buffer);
	glDeleteBuffers(1, &vao);
}

void ObjImporter::prepare_to_draw(unsigned int shaderProgram_in)
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



	// bind des vertices
	glBufferData(GL_ARRAY_BUFFER, mesh_data.map_vertices.size() * sizeof(float), mesh_data.map_vertices.data(), GL_STATIC_DRAW);

	// coordonnées
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	/* ****************************************** */

	
	/* TEXTURE */

	// bind the texture coordinates
	glBindBuffer(GL_ARRAY_BUFFER, texturecoord_buffer);
	glBindVertexArray(texturecoord_buffer);

	glBufferData(GL_ARRAY_BUFFER, mesh_data.textures_coord.size() * sizeof(float), mesh_data.textures_coord.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(3);


	// Texture
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, texture_buffer);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, mesh_data.material.texture.data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glGenerateMipmap(GL_TEXTURE_2D);
	

	// Texture lightmap
	/*
	glActiveTexture(GL_TEXTURE1);

	glBindTexture(GL_TEXTURE_2D, texture_buffer);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, mesh_data.material.texture_diffuse.data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glGenerateMipmap(GL_TEXTURE_2D);
	*/


	/* ******************** */


	// bind the normals
	glBindBuffer(GL_ARRAY_BUFFER, normals_buffer);
	glBindVertexArray(normals_buffer);
	glBufferData(GL_ARRAY_BUFFER, mesh_data.normals.size() * sizeof(float), mesh_data.normals.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(2);

	/* *********************** */





	// bind indices
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_buffer);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, my_map_data.map_indices.size() * sizeof(unsigned int), my_map_data.map_indices.data(), GL_STATIC_DRAW);







}

void ObjImporter::draw(glm::mat4 ViewProjection_in, light_data light, glm::vec3 camPos, glm::mat4 Model_in)
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

	// lightmap
	/*
	uniLoc = glGetUniformLocation(shaderProgram, "texture1");

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture_buffer);

	glUniform1i(uniLoc, GL_TEXTURE1);
	*/


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
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)mesh_data.map_vertices.size() / 3);
	//glDrawElements(GL_TRIANGLES, my_map_data.map_indices.size(), GL_UNSIGNED_INT, (void*)0);

	

}
glm::mat4 ObjImporter::translate(glm::vec3 translation_direction, glm::mat4 Model_in)
{
	Model = glm::translate(Model_in, translation_direction);
	return Model;

}

glm::mat4 ObjImporter::rotate(float angle, glm::mat4 Model_in, glm::vec3 axe)
{
	Model = glm::rotate(Model_in, angle, axe);
	return Model;

}
