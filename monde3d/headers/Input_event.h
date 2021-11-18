#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Misc.h"


struct mouse_keyboard {
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 10.0f);
	glm::vec2 angle = glm::vec2(0.0f);
};

class Input_Event
{
public:
	Input_Event(GLFWwindow* window);
	~Input_Event();

	mouse_keyboard getMovement();
private:

	int forward = GLFW_KEY_W;
	int backward = GLFW_KEY_S;
	int right = GLFW_KEY_D;
	int left = GLFW_KEY_A;

	glm::vec3 direction = glm::vec3(0.0f);

	mouse_keyboard input_return;

	const float SPEED = 1.0f/4.0f;
	const float SENSIBILITY = 1.0f/4.0f;

	GLFWwindow* window = NULL;

};

Input_Event::Input_Event(GLFWwindow * window_in)
{
	window = window_in;
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPos(window, 0, 0);
}

mouse_keyboard Input_Event::getMovement()
{
	if (glfwGetKey(window, forward) == GLFW_PRESS)
	{
		direction.z = -1;
	}
	else if (glfwGetKey(window, backward) == GLFW_PRESS)
	{
		direction.z = 1;
	}
	else if (glfwGetKey(window, right) == GLFW_PRESS)
	{
		direction.x = 1;
	}
	else if (glfwGetKey(window, left) == GLFW_PRESS)
	{
		direction.x = -1;

	}


	// quitter le programme
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		Misc().exitProgram(window);
	}
	/* **************** */



	// Mouvement de la souris 
	double x = 0.0f, y = 0.0f;
	glfwGetCursorPos(window, &x, &y);

	// reverse axes
	input_return.angle.x += y * (3.1415f / 180.0f) * SENSIBILITY;
	input_return.angle.y += x * (3.1415f / 180.0f) * SENSIBILITY;

	//printf("after: %f %f\n", input_return.angle.x, input_return.angle.y);

	glfwSetCursorPos(window, 0, 0);
	/* ********************** */


	// on calcule la vitesse gravité etc..
	input_return.position.z += direction.z * SPEED;
	input_return.position.x += direction.x * SPEED;

	direction = glm::vec3(0.0f);

	return input_return;
}

Input_Event::~Input_Event()
{
}



