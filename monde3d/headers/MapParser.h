#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "lodepng.h"

struct map_data {
	std::vector<float> map_vertices;
	std::vector<unsigned int> map_indices;
	std::vector<float> map_colors;
};


class MapParser
{
public:
	MapParser(char * filename);
	~MapParser();
	void draw(unsigned int shaderProgram, glm::mat4 ViewProjection_in);
	void prepare_to_draw(unsigned int shaderProgram);


private:
	map_data my_map_data;
	unsigned int vbo, color_buffer, vao, indices_buffer;
	glm::mat4 Model;
	glm::mat4 ViewProjection;
};

MapParser::MapParser(char * filename)
{
	// gen vbo and vao
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &color_buffer);
	glGenBuffers(1, &indices_buffer);
	glGenVertexArrays(1, &vao);

	ViewProjection = glm::mat4(0.0f);
	Model = glm::mat4(1.0f);




	// decode heightmap
	std::vector<unsigned char> image; 
	unsigned width, height;

	unsigned error = lodepng::decode(image, width, height, filename);

	if (error)
	{
		exit(1);
	}


	for (unsigned w = 0; w < width; w++)
	{
		for (unsigned h = 0; h < height; h++)
		{

			// gestion des couleurs

			float data = image[(w*4) * h];

			if (data < 42.5f)
			{
				my_map_data.map_colors.push_back(36.0f / 255); // couleur de l'eau
				my_map_data.map_colors.push_back(128.0f / 255); // couleur de l'eau
				my_map_data.map_colors.push_back(214.0f / 255); // couleur de l'eau
			}
			else if (data < 84.0f)
			{
				my_map_data.map_colors.push_back(209.0f / 255); // couleur du sable
				my_map_data.map_colors.push_back(206.0f / 255); // couleur du sable
				my_map_data.map_colors.push_back(115.0f / 255); // couleur du sable
			}
			else if (data < 127.5f)
			{
				my_map_data.map_colors.push_back(10.0f / 255); // couleur de l'herbe
				my_map_data.map_colors.push_back(125.0f / 255); // couleur de l'herbe
				my_map_data.map_colors.push_back(18.0f / 255); // couleur de l'herbe
			}
			else if (data < 170.0f)
			{
				my_map_data.map_colors.push_back(1180.f / 255); // couleur de la pierre
				my_map_data.map_colors.push_back(1200.f / 255); // couleur de la pierre
				my_map_data.map_colors.push_back(1170.f / 255); // couleur de la pierre
			}
			else if (data < 200.0f)
			{
				my_map_data.map_colors.push_back(194.0f / 255); // couleur de la neige
				my_map_data.map_colors.push_back(194.0f / 255); // couleur de la neige
				my_map_data.map_colors.push_back(192.0f / 255); // couleur de la neige
			}


			data /= 255.0f;

			// gestions des vertices
			glm::vec2 center_O = glm::vec2(w / 2.0f, h / 2.0f);

			// positions des points des triangles
			float position_carre[9][3] = {

				{center_O.x / 2.0f, 0.0f, center_O.y * 2.0f}, // 1

				{center_O.x, 0.0f, center_O.y * 2.0f},		// 2

				{center_O.x * 2.0f, 0.0f, center_O.y * 2.0f}, // 3

				{center_O.x * 2.0f, 0.0f, center_O.y},		// 4

				{center_O.x * 2.0f, 0.0f, center_O.y / 2.0f}, // 5

				{center_O.x, 0.0f, center_O.y / 2.0f},		// 6

				{center_O.x / 2.0f, 0.0f, center_O.y / 2.0f}, // 7

				{center_O.x / 2.0f, 0.0f, center_O.y},		// 8

				{center_O.x, data, center_O.y},			// 9
			};



			for (unsigned nb = 0; nb < 8; nb++)
			{
				// triangle pt 1
				my_map_data.map_vertices.push_back(position_carre[nb][0]); // x
				my_map_data.map_vertices.push_back(position_carre[nb][1]); // y
				my_map_data.map_vertices.push_back(position_carre[nb][2]); // z

				// triangle pt 2 
				my_map_data.map_vertices.push_back(position_carre[8][0]); // x
				my_map_data.map_vertices.push_back(position_carre[8][1]); // y
				my_map_data.map_vertices.push_back(position_carre[8][2]); // z

				// triangle pt 3
				my_map_data.map_vertices.push_back(position_carre[nb + 1][0]); // x
				my_map_data.map_vertices.push_back(position_carre[nb + 1][1]); // y
				my_map_data.map_vertices.push_back(position_carre[nb + 1][2]); // z
			}




		}

	}

		// TODO revoir les couleurs
		// TODO revoir les coordonées de la map
		// TODO revoir le generateur de map
	
		//printf("x=%i %f\n", x, data);

	}
}

void MapParser::prepare_to_draw(unsigned int shaderProgram)
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
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, my_map_data.map_indices.size() * sizeof(unsigned int), my_map_data.map_indices.data(), GL_STATIC_DRAW);



}

void MapParser::draw(unsigned int shaderProgram, glm::mat4 ViewProjection_in)
{


	ViewProjection = ViewProjection_in;

	glUseProgram(shaderProgram);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo); // vertices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_buffer); // indices



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

MapParser::~MapParser()
{
}