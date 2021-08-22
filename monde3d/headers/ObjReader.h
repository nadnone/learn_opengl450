#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <glm/glm.hpp>

class ObjReader
{
public:
	ObjReader(const char * filename);
	~ObjReader();

private:
	std::vector<glm::vec3> data_vertices_coord;
};

ObjReader::ObjReader(const char * filename)
{
	std::fstream ObjFile(filename);

	std::string line;


	// get the vertices coordinates
	glm::vec3 vertices;

	// get the faces
	std::vector<std::vector<int>> faces_vertices_lines;
	int faces_vertices[2];

	while (!ObjFile.eof())
	{
		std::getline(ObjFile, line);
		if (line[0] == 'v' && line[1] == ' ')
		{
			sscanf_s(line.c_str(), "v %f %f %f", &vertices.x, &vertices.y, &vertices.z);
		}

		if (line[0] == 'f' && line[1] == ' ')
		{

			// count and split number of spaced blocks
			int spaced_block_nbrs = 0;
			bool cutOrder = false;
			std::string block_cache;
			std::vector<std::string> blocks;
			for (int i = 2; i < line.length(); i++)
			{
				if (line[i] == ' ' || i == line.length()+1)
				{
					spaced_block_nbrs++;
					blocks.push_back(block_cache);
					block_cache = "";
				}
				if(line[i] != ' ') block_cache += line[i];
			}

			// Le programme ne veut pas récuperer la dernière face de la ligne

			for (int i = 0; i < blocks.size(); i++)
			{
				sscanf_s((char*)blocks[i].c_str(), "%d/%d/%d", &faces_vertices[0], &faces_vertices[1], &faces_vertices[2]);
				faces_vertices_lines.push_back(std::vector<int>{ faces_vertices[0], faces_vertices[1], faces_vertices[2] });

			}


		}
	}

	ObjFile.close();



 	data_vertices_coord.push_back(vertices);

	
}

ObjReader::~ObjReader()
{
}