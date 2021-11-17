#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class ObjReader
{
public:
	ObjReader(const char * filename);
	~ObjReader();

	glm::mat4 translate(glm::vec3 translation_direction, glm::mat4 Model_in);
	glm::mat4 rotate(float angle, glm::mat4 Model_in, glm::vec3 axe);

	std::vector<float> getVertices();
	void draw(unsigned int shaderProgram, glm::mat4 ViewProjection_in);

private:
	std::vector<glm::vec3> data_vertices_coord;
	std::vector<std::vector<std::vector<int>>> faces_data;
	std::vector<float> vertices_vector_total;
	unsigned int vbo, vao;
	glm::mat4 Model;
	glm::mat4 ViewProjection;
};






ObjReader::ObjReader(const char * filename)
{
	ViewProjection = glm::mat4(0.0f);
	Model = glm::mat4(1.0f);

	glGenBuffers(1, &vbo);
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

			vertices_vector_total.push_back(data_vertices_coord[(faces_data[i][j][0] - (int)1)].x);
			vertices_vector_total.push_back(data_vertices_coord[(faces_data[i][j][0] - (int)1)].y);
			vertices_vector_total.push_back(data_vertices_coord[(faces_data[i][j][0] - (int)1)].z);

		}
	}
}

std::vector<float> ObjReader::getVertices()
{

	return vertices_vector_total;
}







void ObjReader::draw(unsigned int shaderProgram, glm::mat4 ViewProjection_in)
{
	ViewProjection = ViewProjection_in;

	// bind the vao
	glBindVertexArray(vao);


	// bind the VBO
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices_vector_total.size() * sizeof(float), vertices_vector_total.data(), GL_STATIC_DRAW);


	/*
		Shader Program
	*/


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	/* *********************** */




	/* transformation */


	glUseProgram(shaderProgram);

	GLuint MatrixID = glGetUniformLocation(shaderProgram, "Model");

	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, glm::value_ptr(Model));

	MatrixID = glGetUniformLocation(shaderProgram, "ViewProjection");

	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, glm::value_ptr(ViewProjection));

	/* *********************  */


	//draw
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertices_vector_total.size() / 3);

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