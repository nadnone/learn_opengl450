#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <thread>
#include <chrono>

#include "ObjReader.h"



int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;


    /* which version we use */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);


    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* load GL */
    gladLoadGL();

    // depth testing
    glEnable(GL_DEPTH);


    ObjReader cube("E:/Users/nadir/Documents/GitHub/learn_opengl450/Assets/cube.obj", 0);
    ObjReader cube_2("E:/Users/nadir/Documents/GitHub/learn_opengl450/Assets/cube.obj", 1);
    ObjReader cube_3("E:/Users/nadir/Documents/GitHub/learn_opengl450/Assets/cube.obj", 2);

    /*
    // create the buffers
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    */


    /*
    GLSL vertex shaderand fragment shader
    */

    //vertexshader
    //const char* vertexshaderGLSL = "#version 450 core\nlayout(location = 0) in vec3 aPos;\nvoid main()\n{\ngl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0f);\n}";
    const char* vertexshaderGLSL = "#version 450 core\nlayout(location = 0) in vec3 modelPos;\nuniform mat4 Model;\nuniform mat4 ViewProjection;\nvoid main()\n{\ngl_Position = ViewProjection * Model * vec4(modelPos, 1.0f);\n}";

    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexshaderGLSL, NULL);
    glCompileShader(vertexShader);
    
    const char* fragmentshaderGLSL = "#version 450 core\nout vec4 color;\nvoid main()\n{\ncolor = vec4(0.5568f, 0.2470f, 0.7490f, 1.0f);\n}";
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentshaderGLSL, NULL);
    glCompileShader(fragmentShader);


    /* CHECK SHADER COMPILE ERROR */

    GLint isCompiled = 0;
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
    if(isCompiled == GL_FALSE) 
    {
        GLint maxLength = 0;
        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &errorLog[0]);

        // Provide the infolog in whatever manor you deem best.
        // Exit with failure.
        glDeleteShader(fragmentShader); // Don't leak the shader.
        return 1;
    }

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &errorLog[0]);

        // Provide the infolog in whatever manor you deem best.
        // Exit with failure.
        glDeleteShader(vertexShader); // Don't leak the shader.
        return 1;
    }


    /* ********************************************************** */


    // Attach and link shader

    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    /* ***************************** */



    //glViewport(0, 0, 640, 480);



    /* Loop until the user closes the window */
    float deltaTickLoop = 0;
    float translationLoop = 5;
    bool translationLoopB = false;
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */



        // clear buffer
        glClear(GL_COLOR_BUFFER_BIT);



        /*  transformations matrices */

        glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (640.0f / 480.0f), 0.1f, 100.0f);

        // Camera
        glm::mat4 View = glm::lookAt(glm::vec3(0, 0, -35), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));





        glm::mat4 Model = cube.translate(glm::vec3(translationLoop, 0.0f, 0.0f), glm::mat4(1.0f));
        Model = cube.rotate(deltaTickLoop / 3.1415f / 10.0f, Model, glm::vec3(1, 0, 0));
        cube.draw(shaderProgram, Projection * View);



        Model = cube_2.translate(glm::vec3(0.0f, translationLoop, 0.0f), glm::mat4(1.0f));
        Model = cube_2.rotate(deltaTickLoop / 3.1415f / 10.0f, Model, glm::vec3(0, 1, 0));
        cube_2.draw(shaderProgram, Projection * View);


        Model = cube_3.rotate(deltaTickLoop/3.1415f/10.0f, glm::mat4(1.0f), glm::vec3(1, 1, 1));
        cube_3.draw(shaderProgram, Projection * View);



        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();


        // increament delta Tick Loop
        deltaTickLoop++;

        if (deltaTickLoop >= 360) deltaTickLoop = 0;
        

        
        if (translationLoop < -20) translationLoopB = true;
        else if(translationLoop > 20) translationLoopB = false;

        if (translationLoopB) translationLoop+=0.2f;
        else translationLoop-=0.2f;
        
    }


    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}