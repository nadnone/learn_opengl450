#include <vector>
#include <chrono>

#include "Shader.h"
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
    Shader_Compilation shader_compiler = Shader_Compilation();
    unsigned int shaderProgram = shader_compiler.getShaderProgramID();



    //glViewport(0, 0, 640, 480);


    // run the game loop
    GameLoop gameloop = GameLoop();
    gameloop.run(window, shaderProgram);



    Misc().exitProgram(window);
}