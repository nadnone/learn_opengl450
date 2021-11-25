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


    struct Material_data {
        float shininess = 0.0f;
        glm::vec3 ambiant = glm::vec3(1.0f);
        glm::vec3 diffuse = glm::vec3(0.0f);
        glm::vec3 specular = glm::vec3(0.0f);
    };
    
    struct obj_data {
        std::vector<float> map_vertices;
        std::vector<unsigned int> map_indices;
        std::vector<float> map_colors;
        std::vector<float> textures;
        std::vector<float> normals;
        Material_data material;
    };

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