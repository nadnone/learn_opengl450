#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>

const int MAX_BONES_WEIGHTS = 2000;
const int MAX_BONES = 100;

struct BoneInfo {

    // index de la matrice final des bones
    int id;

    // taille de la matrice de transformation des vertex de l'espace du model à celui des bones
    glm::mat4 offset;
};

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tengent;
    glm::vec3 Bitangent;

    std::vector<int> m_BoneIDs;
    std::vector<float> m_Weights;

};

struct light_data {
    glm::vec3 diffuse = glm::vec3(1.f);
    glm::vec3 ambient = glm::vec3(.1f);
    glm::vec3 specular = glm::vec3(.5f);
    glm::vec3 position = glm::vec3(1.f);
};
struct textures_array {
    uint8_t data[64][64 * 4];
};

struct Material_data {
    float shininess = 32.0f;
    float refract_indice = 1.42f;
    float reflectivity = 0.0f;
    glm::vec3 reflective = glm::vec3(0.f);
    glm::vec3 ambiant = glm::vec3(0.1f);
    glm::vec3 diffuse = glm::vec3(0.5f);
    glm::vec3 specular = glm::vec3(1.f);
    textures_array texture;
    textures_array texture_lightmap;
};

struct obj_data {
    std::vector<float> map_vertices = std::vector<float>(NULL);
    std::vector<float> map_colors = std::vector<float>(NULL);
    std::vector<float> textures_coord =  std::vector<float>(NULL);
    std::vector<float> normals = std::vector<float>(NULL);

    Material_data material;
    std::vector<Vertex> vertex;
};

struct obj_data_2 {
    Material_data material;
    std::vector<Vertex> vertices;
};


class Misc
{
public:
	Misc();
	~Misc();
    void exitProgram(GLFWwindow *window);

    glm::mat4 convertAssimpMatriceToGLM(const aiMatrix4x4& a);

private:

};

Misc::Misc()
{
}

Misc::~Misc()
{
}

glm::mat4 Misc::convertAssimpMatriceToGLM(const aiMatrix4x4& a)
{
    // a,b,c,d = lignes; 1,2,3,4 = colonnes
    glm::mat4 b;
    b[0][0] = a.a1; b[1][0] = a.a2; b[2][0] = a.a3; b[3][0] = a.a4;
    b[0][1] = a.b1; b[1][1] = a.b2; b[2][1] = a.b3; b[3][1] = a.b4;
    b[0][2] = a.c1; b[1][2] = a.c2; b[2][2] = a.c3; b[3][2] = a.c4;
    b[0][3] = a.d1; b[1][3] = a.d2; b[2][3] = a.d3; b[3][3] = a.d4;
    return b;
}



void Misc::exitProgram(GLFWwindow *window)
{
    // end the program
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(0);
}