#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <thread>
#include <chrono>


#include "Input_event.h"

#include "MapParser.h"

#include "ObjReader.h"
#include "ObjImporter.h"


class GameLoop
{
public:
    GameLoop();
    ~GameLoop();
    void run(GLFWwindow* window_in, unsigned int shaderProgram_in);

private:
    float deltaTickLoop = 0;
    float translationLoop = 5;
    bool translationLoopB = false;

    glm::mat4 Projection = glm::mat4(0.0f);
    glm::mat4 View = glm::mat4(0.0f);

    unsigned int shaderProgram = 0;

    double lastmovementTime = 0;

    mouse_keyboard camera_Pos_Angle;

    const float MIN_FPS = 1.0f / 60.0f;

    GLFWwindow* window = NULL;

};

GameLoop::GameLoop()
{

}
GameLoop::~GameLoop()
{
}

void GameLoop::run(GLFWwindow* window_in, unsigned int shaderProgram_in)
{
    window = window_in;
    shaderProgram = shaderProgram_in;

    ObjImporter importerAssimpTest("./Assets/cookie.dae", 1.0f);
    //ObjReader cube("./Assets/cube.obj");   
    importerAssimpTest.prepare_to_draw(shaderProgram);

    //ObjReader sphere_2("./Assets/sphere.obj");
    //ObjReader cube_3("./Assets/cube.obj");

    // initialisation des Events Claviers
    Input_Event inputs(window);

    // initialisation de la map
    MapParser map((char*)"./Assets/map/grayscale_heightmap.png");
    map.prepare_to_draw(shaderProgram);

    float ambiance = 0.1f;

    while (!glfwWindowShouldClose(window))
    {

      

        /* Render here */

        // clear buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Limit movement time
        double time = glfwGetTime();

        if ((time - lastmovementTime) >= MIN_FPS)
        {
            // INPUT EVENTS
            camera_Pos_Angle = inputs.getMovement();
            /* ********************* */
        }
        
        lastmovementTime = time;



         /*  transformations matrices */

        Projection = glm::perspective(glm::radians(45.0f), (1024.0f / 768.0f), 1.0f, 100.0f);

        // Camera
        // 
        // Translation et rotation
        View = glm::lookAt(camera_Pos_Angle.position, camera_Pos_Angle.position + camera_Pos_Angle.camFront, glm::vec3(0.0f, 1.0f, 0.0f));
        /* ********************* */


        map.draw(Projection * View);

        
        //glm::mat4 Model = cube.translate(glm::vec3(translationLoop, 0.0f, 5.0f), glm::mat4(1.0f));
        //Model = cube.rotate(deltaTickLoop / 3.1415f / 10.0f, Model, glm::vec3(1, 0, 0));
   
        importerAssimpTest.draw(Projection * View, ambiance+=0.005f);
        if (ambiance > 1.0f) ambiance = 0.0f;

        //Model = sphere_2.translate(glm::vec3(0.0f, translationLoop, 0.0f), glm::mat4(1.0f));
        //Model = sphere_2.rotate(deltaTickLoop / 3.1415f / 10.0f, Model, glm::vec3(0, 1, 0));
        //sphere_2.draw(shaderProgram, Projection * View);


        //glm::mat4 Model = cube_3.rotate(deltaTickLoop / 3.1415f / 10.0f, glm::mat4(1.0f), glm::vec3(1, 1, 1));
        //cube_3.draw(shaderProgram, Projection * View);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

        // increament delta Tick Loop
        deltaTickLoop++;

        if (deltaTickLoop >= 360) deltaTickLoop = 0;



        if (translationLoop < -5) translationLoopB = true;
        else if (translationLoop > 5) translationLoopB = false;

        if (translationLoopB) translationLoop += 0.2f;
        else translationLoop -= 0.2f;





    }





}


