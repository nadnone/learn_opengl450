#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

class Misc
{
public:
	Misc();
	~Misc();
    void exitProgram(GLFWwindow *window);
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