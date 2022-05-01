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
    void run(GLFWwindow* window_in, unsigned shaderProgram_in[2]);

private:

    glm::mat4 Projection = glm::mat4(0.0f);
    glm::mat4 View = glm::mat4(0.0f);

    float lastmovementTime = 0;

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

void GameLoop::run(GLFWwindow* window_in, unsigned shaderProgram[2])
{
    window = window_in;


    /* 
        chargement de l'horloge
    */

    ObjImporter cadran_clock("./Assets/cadran_clock.dae", "./Assets/textures/cadran_clock_texture.png", 6.f);
    cadran_clock.prepare_to_draw(shaderProgram[0]);

    ObjImporter small_stick_clock("./Assets/small_stick_clock.dae", "./Assets/textures/cube_texture.png", .25f);
    small_stick_clock.prepare_to_draw(shaderProgram[0]);

    ObjImporter big_stick_clock("./Assets/big_stick_clock.dae", "./Assets/textures/cube_texture.png", .25f);
    big_stick_clock.prepare_to_draw(shaderProgram[0]);
    
    ObjImporter sec_stick_clock("./Assets/seconds_stick_clock.dae", "./Assets/textures/TextureCookie.png", 0.25f);
    sec_stick_clock.prepare_to_draw(shaderProgram[0]);


    // --------------------------


    // initialisation des events Claviers
    Input_Event inputs(window);

    // initialisation de la map
    MapParser map("./Assets/map/grayscale_heightmap.png");
    map.prepare_to_draw(shaderProgram[1]);


    while (!glfwWindowShouldClose(window))
    {

        /* Render here */


        // clear buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Limit movement time
        float time = glfwGetTime();

        if ((time - lastmovementTime) >= MIN_FPS)
        {
            // INPUT EVENTS
            camera_Pos_Angle = inputs.getMovement();
            /* ********************* */
        }
        
        lastmovementTime = time;
        
         /*  transformations matrices */

        light_data lightdata;

        lightdata.position = glm::vec3(0.f, 0.f, 5.f);


        Projection = glm::perspective(glm::radians(45.0f), (1024.0f / 768.0f), 1.0f, 100.0f);

        // Camera
        // 
        // Translation et rotation
        glm::vec3 cam_eye = camera_Pos_Angle.position + camera_Pos_Angle.camFront;
        View = glm::lookAt(camera_Pos_Angle.position, cam_eye, glm::vec3(0.0f, 1.0f, 0.0f));
        /* ********************* */




       // map.draw(Projection * View, lightdata, cam_eye);



        /*
            Horloge
        */

        glm::mat4 Model = glm::mat4(1.0f);


        Model = cadran_clock.translate(glm::vec3(0.f, 6.f, 0.f), Model);
        Model = cadran_clock.rotate(90.f * 3.1415f / 180.f, Model, glm::vec3(0.f, 0.f, 1.f));
        cadran_clock.draw(Projection * View, lightdata, cam_eye, Model);


        Model = small_stick_clock.translate(glm::vec3(0.f, 0.f, 0.f), Model);
        Model = small_stick_clock.rotate(90.f * 3.1415f / 180.f, Model, glm::vec3(0.f, 0.f, 1.f));
        Model = small_stick_clock.rotate(3.1415f, Model, glm::vec3(1.f, 0.f, 0.f));

        //small_stick_clock.draw(Projection * View, lightdata, cam_eye, Model);

        /*
            spin hours stick
        
        */

        int64_t timestamp = (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() + (2*3600)) % 43200;
        
        float v_angular = -(2 * 3.1415f) / 43200;
        v_angular *= timestamp;


        // cos = adj / hyp; adj = hyp * cos
        // sin = opp / hyp; opp = hyp * sin

        float x_stick = glm::cos(v_angular);
        float y_stick = glm::sin(v_angular);

        Model = small_stick_clock.translate(glm::vec3(0.f, x_stick, y_stick), Model);
        Model = small_stick_clock.rotate(v_angular, Model, glm::vec3(1.f, 0.f, 0.f));

        small_stick_clock.draw(Projection * View, lightdata, cam_eye, Model);


        printf("%i\n", timestamp);

        
        // ********************


        Model = big_stick_clock.rotate(-v_angular, Model, glm::vec3(1.f, 0.f, 0.f));
        Model = big_stick_clock.translate(glm::vec3(0.f, -x_stick, -y_stick), Model);
        //big_stick_clock.draw(Projection * View, lightdata, cam_eye, Model);

        // -----------------------
        /*
            spin minutes stick
        */
        v_angular = -(2 * 3.1415f) / 3600;
        v_angular *= timestamp;

        x_stick = glm::cos(v_angular);
        y_stick = glm::sin(v_angular);

        Model = big_stick_clock.translate(glm::vec3(0.f, x_stick, y_stick), Model);
        Model = big_stick_clock.rotate(v_angular, Model, glm::vec3(1.f, 0.f, 0.f));

        big_stick_clock.draw(Projection * View, lightdata, cam_eye, Model);
        
        // -----------------------
 

        Model = sec_stick_clock.rotate(-v_angular, Model, glm::vec3(1.f, 0.f, 0.f));
        Model = sec_stick_clock.translate(glm::vec3(0.f, -x_stick, -y_stick), Model);
        //sec_stick_clock.draw(Projection* View, lightdata, cam_eye, Model);

        /*
            spin seconds stick
        */
        v_angular = -(2 * 3.1415f) / 60;
        v_angular *= timestamp;

        x_stick = glm::cos(v_angular);
        y_stick = glm::sin(v_angular);

        Model = sec_stick_clock.translate(glm::vec3(0.f, x_stick, y_stick), Model);
        Model = sec_stick_clock.rotate(v_angular, Model, glm::vec3(1.f, 0.f, 0.f));

        sec_stick_clock.draw(Projection* View, lightdata, cam_eye, Model);

        // -----------------------


        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

      



    }





}


