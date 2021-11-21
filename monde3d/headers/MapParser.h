#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "lodepng.h"
#include "Misc.h"


class MapParser
{
public:
	MapParser(char * filename);
	~MapParser();
	void draw(glm::mat4 ViewProjection_in);
	void prepare_to_draw(unsigned int shaderProgram_in);


private:
	Misc::obj_data my_obj_data;
	unsigned int vbo = 0,
		color_buffer = 0,
		vao = 0,
		shaderProgram = 0;
	glm::mat4 Model;
	glm::mat4 ViewProjection;
	const float COEFF = 0.1f;
};

MapParser::MapParser(char* filename)
{
	// gen vbo and vao
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &color_buffer);
	//glGenBuffers(1, &indices_buffer);
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

	std::vector<float> z;

	for (unsigned i = 0; i < image.size(); i++)
	{
		z.push_back((image[i] / 255.0f) * 4.0f);
	}


	for (int h = 0; h <= (int)height - 2; h++)
	{
		for (int w = 0; w <= (int)width - 2; w++)
		{




			// gestions des vertices

			int x_pixel = w, y_pixel = h; // int car indice de pixel dans la map
			const int RGBA_nb = 4;
			float y_pos = h * 1.0f, x_pos = w * 1.0f; // positions flotante pour une utilisation en tant que coordonnée

			// on centre l'objet dans le monde
			x_pos -= (width / 2);
			y_pos -= (height / 2) - 200;

			// on multiplie les positions par un coefficien pour dominuer leur taille
			x_pos *= COEFF; 
			y_pos *= COEFF;

			// positions des points des deux triangles (par rapport à mon croquis)
			float position_carre[4][3] = {
				// 0 = 1
				{
					x_pos,
					z[ (x_pixel * RGBA_nb) + (y_pixel * width * RGBA_nb)],
					y_pos
				},					
				// 1 = 2
				{ 
					x_pos + COEFF,
					z[( (1 + x_pixel) * RGBA_nb) + (y_pixel * width * RGBA_nb)],
					y_pos
				},
				// 2 = 4
				{ 
					x_pos, 
					z[( x_pixel * RGBA_nb) + ((1 + y_pixel) * width * RGBA_nb)],
					COEFF + y_pos
				},
				// 3 = 5
				{ 
					COEFF + x_pos,
					z[( (1 + x_pixel) * RGBA_nb) + ((1 + y_pixel) * width * RGBA_nb)],
					COEFF + y_pos
				}	
			};

			unsigned positions_colors[4] = {
									// 0
									(x_pixel * RGBA_nb) + (y_pixel * width * RGBA_nb),
									// 1
									((1 + x_pixel) * RGBA_nb) + (y_pixel * width * RGBA_nb),
									// 2
									(x_pixel * RGBA_nb) + ((1 + y_pixel) * width * RGBA_nb),
									// 3
									((1 + x_pixel) * RGBA_nb) + ((1 + y_pixel) * width * RGBA_nb)
			};

			// ordre des positions pour former les deux triangles
			unsigned order[6] = {
								0, 2, 1,
								2, 1, 3
			};

			// draw square
			for (unsigned nb = 0; nb < 6; nb++) // nombre d'ordre ( quel positions va dans quel ordre )
			{

				for (unsigned pt = 0; pt < 3; pt++) // pour chaque axe (x, y, z)
				{
					// triangles par rapport à l'ordre de position définis
					my_obj_data.map_vertices.push_back(position_carre[ order[nb] ][pt]); // xyz
							
				}

				// gestion des couleurs
				float data = image[positions_colors[order[nb]]];


				if (data < 42.5f)
				{
					my_obj_data.map_colors.push_back(36.0f / 255); // couleur de l'eau
					my_obj_data.map_colors.push_back(128.0f / 255); // couleur de l'eau
					my_obj_data.map_colors.push_back(214.0f / 255); // couleur de l'eau
				}
				else if (data < 84.0f)
				{
					my_obj_data.map_colors.push_back(209.0f / 255); // couleur du sable
					my_obj_data.map_colors.push_back(206.0f / 255); // couleur du sable
					my_obj_data.map_colors.push_back(115.0f / 255); // couleur du sable
				}
				else if (data < 127.5f)
				{
					my_obj_data.map_colors.push_back(10.0f / 255); // couleur de l'herbe
					my_obj_data.map_colors.push_back(125.0f / 255); // couleur de l'herbe
					my_obj_data.map_colors.push_back(18.0f / 255); // couleur de l'herbe
				}
				else if (data < 170.0f)
				{
					my_obj_data.map_colors.push_back(1180.f / 255); // couleur de la pierre
					my_obj_data.map_colors.push_back(1200.f / 255); // couleur de la pierre
					my_obj_data.map_colors.push_back(1170.f / 255); // couleur de la pierre
				}
				else if (data < 255.0f)
				{
					my_obj_data.map_colors.push_back(194.0f / 255); // couleur de la neige
					my_obj_data.map_colors.push_back(194.0f / 255); // couleur de la neige
					my_obj_data.map_colors.push_back(192.0f / 255); // couleur de la neige
				}
			}
		}

	}

}
void MapParser::prepare_to_draw(unsigned int shaderProgram_in) 
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
	glBufferData(GL_ARRAY_BUFFER, my_obj_data.map_vertices.size() * sizeof(float), my_obj_data.map_vertices.data(), GL_STATIC_DRAW);

	// coordonnées
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	/* ****************************************** */

	// bind the couelur
	
	glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
	glBindVertexArray(color_buffer);
	// bind des couleurs
	glBufferData(GL_ARRAY_BUFFER, my_obj_data.map_colors.size() * sizeof(float), my_obj_data.map_colors.data(), GL_STATIC_DRAW);


	// couleurs
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);
	
	/* *********************** */

	// bind indices
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_buffer);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, my_map_data.map_indices.size() * sizeof(unsigned int), my_map_data.map_indices.data(), GL_STATIC_DRAW);



}

void MapParser::draw(glm::mat4 ViewProjection_in)
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
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)my_obj_data.map_vertices.size() / 3);
	//glDrawElements(GL_TRIANGLES, my_map_data.map_indices.size(), GL_UNSIGNED_INT, (void*)0);

}

MapParser::~MapParser()
{
}