#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Input_Event
{
public:
	Input_Event(GLFWwindow* window);
	~Input_Event();

	glm::vec3 getMovement();
private:

	int forward = GLFW_KEY_W;
	int backward = GLFW_KEY_S;
	int right = GLFW_KEY_D;
	int left = GLFW_KEY_A;

	glm::vec3 direction = glm::vec3(0.0f, 0.0f, 0.0f);

	const float SPEED = 1.2f;
	glm::vec3 velocity = glm::vec3(0.0f,0.0f,10.0f);

	GLFWwindow* window = NULL;

};

Input_Event::Input_Event(GLFWwindow * window_in)
{
	window = window_in;
}

glm::vec3 Input_Event::getMovement()
{
	if (glfwGetKey(window, forward) == GLFW_PRESS)
	{
		direction.z = 1;
	}
	else if (glfwGetKey(window, backward) == GLFW_PRESS)
	{
		direction.z = -1;
	}
	else if (glfwGetKey(window, right) == GLFW_PRESS)
	{
		direction.y = 1;
	}
	else if (glfwGetKey(window, left) == GLFW_PRESS)
	{
		direction.y = -1;

	}


	// on calcule la vitesse gravité etc..
	velocity.z += direction.z * SPEED;
	velocity.x += direction.x * SPEED; // à recoir

	direction = glm::vec3(0.0f);

	return velocity;
}

Input_Event::~Input_Event()
{
}



