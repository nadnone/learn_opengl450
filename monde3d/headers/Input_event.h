#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Misc.h"


struct mouse_keyboard {
	glm::vec3 position = glm::vec3(17.0f, 2.0f, -27.0f);
	glm::vec3 direction = glm::vec3(0.0f);
	glm::vec2 angle = glm::vec2(0.0f);	
	glm::vec3 camFront = glm::vec3(0.0f);
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

	mouse_keyboard input_return;

	const float SPEED = 0.25f;
	const float SENSIVITY = 0.1f;

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

	// Mouvement de la souris 
	double x = 0.0f, y = 0.0f;
	glfwGetCursorPos(window, &x, &y);

	input_return.angle.x += (float)x * SENSIVITY;
	input_return.angle.y += (float)y * SENSIVITY;


	// max angle
	if (input_return.angle.y < -70.0f)
	{
		input_return.angle.y = -70.0f;
	}
	if (input_return.angle.y > 70.0f)
	{
		input_return.angle.y = 70.0f;
	}

	// calcule de la direction
	input_return.direction.x = cos(glm::radians(input_return.angle.x)) * cos(glm::radians(input_return.angle.y));
	input_return.direction.y = -sin(glm::radians(input_return.angle.y));
	input_return.direction.z = sin(glm::radians(input_return.angle.x)) * cos(glm::radians(input_return.angle.y));
	input_return.camFront = glm::normalize(input_return.direction);

	glfwSetCursorPos(window, 0, 0);
	/* ********************** */



	if (glfwGetKey(window, forward) == GLFW_PRESS)
	{
		input_return.position += SPEED * input_return.camFront;
	}
	else if (glfwGetKey(window, backward) == GLFW_PRESS)
	{
		input_return.position -= SPEED * input_return.camFront;
	}
	else if (glfwGetKey(window, right) == GLFW_PRESS)
	{
		input_return.position += SPEED * glm::normalize(glm::cross(input_return.camFront, glm::vec3(0.0f, 1.0f, 0.0f)));
	}
	else if (glfwGetKey(window, left) == GLFW_PRESS)
	{
		input_return.position -= SPEED * glm::normalize(glm::cross(input_return.camFront, glm::vec3(0.0f, 1.0f, 0.0f)));
	}



	// quitter le programme
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		Misc().exitProgram(window);
	}
	/* **************** */






	return input_return;
}

Input_Event::~Input_Event()
{
}



