#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Misc
{
public:
	Misc();
	~Misc();
    void exitProgram(GLFWwindow *window);

    struct obj_data {
        std::vector<float> map_vertices;
        std::vector<unsigned int> map_indices;
        std::vector<float> map_colors;
        std::vector<float> textures;
        std::vector<float> normals;
    };

    //std::vector<std::vector<obj_data>> Mesh;

private:

};

Misc::Misc()
{
}

Misc::~Misc()
{
}

void Misc::exitProgram(GLFWwindow *window)
{
    // end the program
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(0);
}