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

    struct light_data {
        glm::vec3 diffuse = glm::vec3(1.f);
        glm::vec3 ambient = glm::vec3(1.f);
        glm::vec3 specular = glm::vec3(1.f);
        glm::vec3 position = glm::vec3(1.f);
    };

    struct Material_data {
        float shininess = 32.0f;
        float refract_indice = 0.0f;
        float reflectivity = 0.0f;
        glm::vec3 reflective =glm::vec3(0.f);
        glm::vec3 ambiant = glm::vec3(0.f);
        glm::vec3 diffuse = glm::vec3(0.f);
        glm::vec3 specular = glm::vec3(0.f);
        glm::vec3 color = glm::vec3(0.f);
    };
    struct obj_data {
        std::vector<float> map_vertices;
        std::vector<float> map_colors;
        std::vector<float> textures_coord;
        std::vector<float> normals;
        Material_data material;
    };
    struct textures_array {
        unsigned data[64*4][64];
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