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


    // triangle vertices
    float vertices_triangle[] = {
        -1.0f, -1.0f, 0.0f,        -1.0f, 1.0f, 0.0f,        1.0f,  -1.0f, 0.0f, //triangle 1

        1.0f, -1.0f, 0.0f,        -1.0f, 1.0f, 0.0f,        1.0f,  1.0f, 0.0f  //triangle 2


    };


    ObjReader cube("E:/Users/nadir/Documents/GitHub/learn_opengl450/Assets/cube.obj");


    
    // create the buffers
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    unsigned int VBO;
    glGenBuffers(1, &VBO);



    /*
    GLSL vertex shaderand fragment shader
    */

    //vertexshader
    //const char* vertexshaderGLSL = "#version 450 core\nlayout(location = 0) in vec3 aPos;\nvoid main()\n{\ngl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0f);\n}";
    const char* vertexshaderGLSL = "#version 450 core\nlayout(location = 0) in vec3 modelPos;\nuniform mat4 MVP;\nvoid main()\n{\ngl_Position = MVP * vec4(modelPos, 1.0f);\n}";

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
    int deltaTickLoop = 0;
    int translationLoop = 5;
    bool translationLoopB = false;
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        // bind the VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_triangle), vertices_triangle, GL_STATIC_DRAW);


        /*
            Shader Program
        */


        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        /* *********************** */




        /* Matrix transformations */

        glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (640.0f / 480.0f), 0.1f, 100.0f);

        // Camera
        glm::mat4 View = glm::lookAt(glm::vec3(0, 0, -10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));


        glm::mat4 Model = glm::mat4(1.0f);
        Model = glm::rotate(Model, glm::radians((float)deltaTickLoop), glm::vec3(1, 1, 1));

        Model = glm::translate(Model, glm::vec3(translationLoop, 0.0f, 0.0f));

        glm::mat4 ModelViewProjection = Projection * View * Model;
        
        // send to shader
        GLuint MatrixID = glGetUniformLocation(shaderProgram, "MVP");
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, glm::value_ptr(ModelViewProjection));


        /* draw the triangle */
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);


        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();


        // increament delta Tick Loop
        deltaTickLoop++;

        if (deltaTickLoop >= 360) deltaTickLoop = 0;
        


        if (translationLoop < -5) translationLoopB = true;
        else if(translationLoop > 5) translationLoopB = false;

        if (translationLoopB) translationLoop++;
        else translationLoop--;

    }


    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}