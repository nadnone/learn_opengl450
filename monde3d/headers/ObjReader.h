/*
Fichier obsolète, peut être supprimé
*/
#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Misc.h"

class ObjReader
{
public:
	ObjReader(const char * filename);
	~ObjReader();

	glm::mat4 translate(glm::vec3 translation_direction, glm::mat4 Model_in);
	glm::mat4 rotate(float angle, glm::mat4 Model_in, glm::vec3 axe);
	void draw(unsigned int shaderProgram, glm::mat4 ViewProjection_in);
	void prepare_to_draw(unsigned int shaderProgram);

private:
	std::vector<glm::vec3> data_vertices_coord;
	std::vector<std::vector<std::vector<int>>> faces_data;
	Misc::obj_data my_map_data;
	unsigned int vbo, color_buffer, vao, indices_buffer;
	glm::mat4 Model;
	glm::mat4 ViewProjection;
};






ObjReader::ObjReader(const char * filename)
{
	ViewProjection = glm::mat4(0.0f);
	Model = glm::mat4(1.0f);

	// gen vbo and vao
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &color_buffer);
	//glGenBuffers(1, &indices_buffer);
	glGenVertexArrays(1, &vao);

	std::fstream ObjFile(filename);

	std::string line;
	glm::vec3 vertices;


	// get the vertices coordinates

	// get the faces
	std::vector<std::vector<int>> faces_vertices_lines;
	int block_split_data[3] = {0,0,0};

	while (!ObjFile.eof())
	{
		std::getline(ObjFile, line);
		if (line[0] == 'v' && line[1] == ' ')
		{
			sscanf_s(line.c_str(), "v %f %f %f", &vertices.x, &vertices.y, &vertices.z);
			data_vertices_coord.push_back(vertices);
		}

		if (line[0] == 'f' && line[1] == ' ')
		{

			// count and split number of spaced blocks
			bool cutOrder = false;
			std::string block_cache;
			std::vector<std::string> blocks;
			for (int i = 2; i < line.length()+1; i++)
			{
				if (line[i] == ' ' || i == line.length())
				{
					blocks.push_back(block_cache);
					block_cache = "";
				}
				if(line[i] != ' ') block_cache += line[i];
			}

			// push faces data
			for (int i = 0; i < blocks.size(); i++)
			{
				sscanf_s((char*)blocks[i].c_str(), "%d/%d/%d", &block_split_data[0], &block_split_data[1], &block_split_data[2]);
				faces_vertices_lines.push_back(std::vector<int>{ block_split_data[0], block_split_data[1], block_split_data[2] });

			}
			
			faces_data.push_back(faces_vertices_lines);
			faces_vertices_lines.clear();
		}
	}
	ObjFile.close();


	/*
		PARSE DATA
	*/

	for (int i = 0; i < faces_data.size(); i++)
	{
		for (int j = 0; j < faces_data[i].size(); j++)
		{

			my_map_data.map_vertices.push_back(data_vertices_coord[(faces_data[i][j][0] - (int)1)].x);
			my_map_data.map_vertices.push_back(data_vertices_coord[(faces_data[i][j][0] - (int)1)].y);
			my_map_data.map_vertices.push_back(data_vertices_coord[(faces_data[i][j][0] - (int)1)].z);


			// TODO remplacer par couleur de l'objet
			my_map_data.map_colors.push_back(0.0f);
			my_map_data.map_colors.push_back(0.0f);
			my_map_data.map_colors.push_back(255.0f);



		}
	}

}



void ObjReader::prepare_to_draw(unsigned int shaderProgram)
{

	glUseProgram(shaderProgram);

	// bind the vao
	glBindVertexArray(vao);





	/*
		Shader Program
	*/

	// bind the VBO 
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// bind des vertices
	glBufferData(GL_ARRAY_BUFFER, my_map_data.map_vertices.size() * sizeof(float), my_map_data.map_vertices.data(), GL_STATIC_DRAW);

	// coordonnées
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	/* ****************************************** */

	// bind the couelur

	glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
	glBindVertexArray(color_buffer);
	// bind des couleurs
	glBufferData(GL_ARRAY_BUFFER, my_map_data.map_colors.size() * sizeof(float), my_map_data.map_colors.data(), GL_STATIC_DRAW);


	// couleurs
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);

	/* *********************** */

	// bind indices
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_buffer);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, my_map_data.map_indices.size() * sizeof(unsigned int), my_map_data.map_indices.data(), GL_STATIC_DRAW);



}

void ObjReader::draw(unsigned int shaderProgram, glm::mat4 ViewProjection_in)
{

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


	//draw
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)my_map_data.map_vertices.size() / 3);
	//glDrawElements(GL_TRIANGLES, my_map_data.map_indices.size(), GL_UNSIGNED_INT, (void*)0);

}


glm::mat4 ObjReader::translate(glm::vec3 translation_direction, glm::mat4 Model_in)
{
	Model = glm::translate(Model_in, translation_direction);
	return Model;

}

glm::mat4 ObjReader::rotate(float angle, glm::mat4 Model_in, glm::vec3 axe)
{
	Model = glm::rotate(Model_in, angle, axe);
	return Model;

}


ObjReader::~ObjReader()
{
}