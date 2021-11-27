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

    ObjImporter importerAssimpTest("./Assets/cube.dae", 1.0f);
    importerAssimpTest.prepare_to_draw(shaderProgram);


    // initialisation des Events Claviers
    Input_Event inputs(window);

    // initialisation de la map
    MapParser map((char*)"./Assets/map/grayscale_heightmap.png");
    map.prepare_to_draw(shaderProgram);
    
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

        Misc::light_data lightdata;
        glm::vec3 lightcolor = glm::vec3(249.f / 255, 215.f / 255, 28.f / 255);
        lightdata.position = glm::vec3(0.f, 200.f, 20.f);
        lightdata.diffuse = lightcolor * glm::vec3(1.0f);
        lightdata.ambient = lightcolor * glm::vec3(1.0f);
        lightdata.specular = lightcolor * glm::vec3(1.0f);


        Projection = glm::perspective(glm::radians(45.0f), (1024.0f / 768.0f), 1.0f, 100.0f);

        // Camera
        // 
        // Translation et rotation
        glm::vec3 cam_eye = camera_Pos_Angle.position + camera_Pos_Angle.camFront;
        View = glm::lookAt(camera_Pos_Angle.position, cam_eye, glm::vec3(0.0f, 1.0f, 0.0f));
        /* ********************* */

        if (time > 2)
        {
            printf("X: %f Y: %f Z: %f\n", camera_Pos_Angle.position.x, camera_Pos_Angle.position.y, camera_Pos_Angle.position.z);

        }


        map.draw(Projection * View, lightdata, cam_eye);

        
        glm::mat4 Model = importerAssimpTest.translate(glm::vec3(17.0f, 2.0f, -27.0f), glm::mat4(1.0f));
        //Model = importerAssimpTest.rotate(45.0f * 3.1415 / 180, Model, glm::vec3(1, 0, 0));
   
        importerAssimpTest.draw(Projection * View, lightdata, cam_eye, Model);

        //Model = sphere_2.translate(glm::vec3(0.0f, translationLoop, 0.0f), glm::mat4(1.0f));
        //Model = sphere_2.rotate(deltaTickLoop / 3.1415f / 10.0f, Model, glm::vec3(0, 1, 0));
        //sphere_2.draw(shaderProgram, Projection * View);


        //glm::mat4 Model = cube_3.rotate(deltaTickLoop / 3.1415f / 10.0f, glm::mat4(1.0f), glm::vec3(1, 1, 1));
        //cube_3.draw(shaderProgram, Projection * View);



        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

      



    }





}


