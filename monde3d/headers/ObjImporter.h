#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Misc.h"
#include "MapParser.h"

class ObjImporter
{
public:
	ObjImporter(const char * filename, float scale);
	~ObjImporter();
	void draw(glm::mat4 ViewProjection_in, Misc::light_data light, glm::vec3 camPos, glm::mat4 Model_in);
	void prepare_to_draw(unsigned int shaderProgram);
	glm::mat4 translate(glm::vec3 translation_direction, glm::mat4 Model_in);
	glm::mat4 rotate(float angle, glm::mat4 Model_in, glm::vec3 axe);

private:
	unsigned int vbo = 0,
		color_buffer = 0,
		normals_buffer = 0,
		vao = 0,
		shaderProgram = 0;
	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 ViewProjection = glm::mat4(0.0f);

	Misc::obj_data mesh_data;
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

		// couleur 
		
		mesh_data.map_colors.push_back(95.0f / 255);
		mesh_data.map_colors.push_back(67.0f / 255);
		mesh_data.map_colors.push_back(54.0f / 255);
		

		// normals
		mesh_data.normals.push_back(mesh->mNormals[i].x);
		mesh_data.normals.push_back(mesh->mNormals[i].y);
		mesh_data.normals.push_back(mesh->mNormals[i].z);

		// textures
		if (mesh->mTextureCoords[0])
		{
			mesh_data.textures.push_back(mesh->mTextureCoords[0][1].x);
			mesh_data.textures.push_back(mesh->mTextureCoords[0][1].y);
		}
		else
		{
			mesh_data.textures.push_back(0.0f);
			mesh_data.textures.push_back(0.0f);
		}


	}


	// process indices
	for (unsigned i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for (unsigned j = 0; j < face.mNumIndices; j++)
		{
			mesh_data.map_indices.push_back(face.mIndices[j]);
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
				material->Get(AI_MATKEY_REFLECTIVITY, refract);
				mesh_data.material.refract_indice = refract;

				// reflective
				material->Get(AI_MATKEY_COLOR_REFLECTIVE, color);
				mesh_data.material.reflective = glm::vec3(color.r, color.g, color.b);

				// ambiant
				material->Get(AI_MATKEY_COLOR_AMBIENT, color);
				mesh_data.material.ambiant = glm::vec3(color.r, color.g, color.b);

				// shininess
				float shininess;
				material->Get(AI_MATKEY_SHININESS, shininess);
				mesh_data.material.shininess = shininess;

				// color
				material->Get(AI_MATKEY_BASE_COLOR, color);
				mesh_data.material.color = glm::vec3(color.r, color.g, color.b);
				
				// TODO récupérer la couleur du material
			}

			processMesh(mesh, scene);

		}
		if (child->mNumChildren > 0)
		{
			processNode(child, scene);
		}

	}

}


ObjImporter::ObjImporter(const char * filename, float scale)
{

	coeff = scale;

	// gen vbo and vao
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &color_buffer);
	glGenBuffers(1, &normals_buffer);
	//glGenBuffers(1, &indices_buffer);
	glGenVertexArrays(1, &vao);


	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate);




	if (!scene && !importer.GetErrorString() && !scene->mNumMeshes > 0)
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
}

void ObjImporter::prepare_to_draw(unsigned int shaderProgram_in)
{


	shaderProgram = shaderProgram_in;

	glUseProgram(shaderProgram);

	// bind the vao
	glBindVertexArray(vao);





	/*
		Shader Program
	*/

	// bind the VBO 
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// bind des vertices
	glBufferData(GL_ARRAY_BUFFER, mesh_data.map_vertices.size() * sizeof(float), mesh_data.map_vertices.data(), GL_STATIC_DRAW);

	// coordonnées
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	/* ****************************************** */

	// bind the couleur
	/*
	glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
	glBindVertexArray(color_buffer);
	glBufferData(GL_ARRAY_BUFFER, mesh_data.map_colors.size() * sizeof(float), mesh_data.map_colors.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);
	*/
	/* *********************** */



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

void ObjImporter::draw(glm::mat4 ViewProjection_in, Misc::light_data light, glm::vec3 camPos, glm::mat4 Model_in)
{

	Model = Model_in;
	ViewProjection = ViewProjection_in;
	
	glUseProgram(shaderProgram);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo); // vertices
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_buffer); // indices

	
	/* transformation */

	GLuint MatrixID = glGetUniformLocation(shaderProgram, "Model");

	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, glm::value_ptr(Model));

	MatrixID = glGetUniformLocation(shaderProgram, "ViewProjection");

	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, glm::value_ptr(ViewProjection));

	/* *********************  */


	// eye normal
	MatrixID = glGetUniformLocation(shaderProgram, "eyePos");
	glUniform3f(MatrixID, camPos.x, camPos.y, camPos.z);



	/* LUMIERE */

	// diffuse
	MatrixID = glGetUniformLocation(shaderProgram, "light.diffuse");
	glUniform3f(MatrixID, light.diffuse.x, light.diffuse.y, light.diffuse.z);

	// ambient
	MatrixID = glGetUniformLocation(shaderProgram, "light.ambient");
	glUniform3f(MatrixID, light.ambient.x, light.ambient.y, light.ambient.z);

	// specular
	MatrixID = glGetUniformLocation(shaderProgram, "light.specular");
	glUniform3f(MatrixID, light.specular.x, light.specular.y, light.specular.z);

	// position
	MatrixID = glGetUniformLocation(shaderProgram, "light.position");
	glUniform3f(MatrixID, light.position.x, light.position.y, light.position.z);

	/* ********************** */


	/* PHONG */

	// ambient
	MatrixID = glGetUniformLocation(shaderProgram, "material.ambient");
	glUniform3f(MatrixID, mesh_data.material.ambiant.x, mesh_data.material.ambiant.y, mesh_data.material.ambiant.z);

	// shininess
	MatrixID = glGetUniformLocation(shaderProgram, "material.shininess");
	glUniform1f(MatrixID, mesh_data.material.shininess);

	// specular
	MatrixID = glGetUniformLocation(shaderProgram, "material.specular");
	glUniform3f(MatrixID, mesh_data.material.specular.x, mesh_data.material.specular.y, mesh_data.material.specular.z);

	// diffuse
	MatrixID = glGetUniformLocation(shaderProgram, "material.diffuse");
	glUniform3f(MatrixID, mesh_data.material.diffuse.x, mesh_data.material.diffuse.y, mesh_data.material.diffuse.z);

	// reflective
	MatrixID = glGetUniformLocation(shaderProgram, "material.reflective");
	glUniform3f(MatrixID, mesh_data.material.reflective.x, mesh_data.material.reflective.y, mesh_data.material.reflective.z);

	// reflectivity
	MatrixID = glGetUniformLocation(shaderProgram, "material.refract_indice");
	glUniform1f(MatrixID, mesh_data.material.refract_indice);

	// color
	MatrixID = glGetUniformLocation(shaderProgram, "material.color");
	glUniform3f(MatrixID, mesh_data.material.color.x, mesh_data.material.color.y, mesh_data.material.color.z);
	/* *********************  */


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
