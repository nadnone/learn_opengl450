#include <vector>
#include <chrono>

#include "Shader_texture.h"
#include "Shader_map.h"

#include "GameLoop.h"

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
    {
        printf("Init glfw error\n");
        return 1;
    }


    /* which version we use */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1024, 768, "Hello World", NULL, NULL);


    if (!window)
    {
        printf("Window creating error\n");
        glfwTerminate();
        return 1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* load GL */
    gladLoadGL();

    // depth testing
    glEnable(GL_DEPTH_TEST);


    
    //Get the shader program id
    unsigned shaderProgram[2];


    Shader_texture shader_tex = Shader_texture();
    shaderProgram[0] = shader_tex.getShaderProgramID();

    Shader_map shader_map = Shader_map();
    shaderProgram[1] = shader_map.getShaderProgramID();




    // run the game loop
    GameLoop gameloop = GameLoop();
    gameloop.run(window, shaderProgram);



    Misc().exitProgram(window);
}