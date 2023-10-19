#pragma once
#include "defs.h"
#include "shaders.h"
#include <vector>

class input 
{
public:
	input()
	{

	}

	bool isKeyDown(GLFWwindow* window, int key);
};

class Script;

class render
{
public:

	render()
	{
		inputClass = input();
	}
	std::vector<Script> scripts;
	std::vector<Shader> shaders;

	input inputClass;
	GLFWwindow* window;

	float deltaTime;
	void initalize();
	void renderLoop();
	void frame();
	void UpdateCamera();
};

class Script 
{
public:

	Script* script;
	render* rendering;
	Script() {

	}
	Script(render* render)
	{
		rendering = render;
	}
	virtual void Update() {};
	virtual void Start() {}
	int index = 0;

};

class Camera
{
public:
	Camera()
	{

	}

	render* rendering;

	float lastX = 400, lastY = 300;
	float Yaw = -90.0f;
	float Pitch;
	float speed = 2.5f;
	float deltaTime;
	float MouseSensitivity = 0.1f;

	bool firstMouse = true;

	glm::vec3 Position;
	glm::vec3 Front = glm::vec3(0,0,-1);
	glm::vec3 Up = glm::vec3(0,1,0);
	glm::vec3 Right;
	glm::vec3 WorldUp = glm::vec3(0,1,0);

	glm::mat4 view;
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(Position, Position + Front, Up);
	}

	void updateCamera(float xoffset, float yoffset)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw += xoffset;
		Pitch += yoffset;
		if (Pitch > 89.0f)
			Pitch = 89.0f;
		if (Pitch < -89.0f)
			Pitch = -89.0f;
		updateVectors();
	}

	void updateVectors()
	{
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		Right = glm::normalize(glm::cross(Front, WorldUp));
		Up = glm::normalize(glm::cross(Right, Front));

	}

	void handleMouse(double xPosIn, double yPosIn)
	{
		float xpos = static_cast<float>(xPosIn);
		float ypos = static_cast<float>(yPosIn);

		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}
		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos;

		lastX = xpos;
		lastY = ypos;

		updateCamera(xoffset, yoffset);
	}

	void handleMovement()
	{
		float velocity = speed * deltaTime;

		if (this->rendering->inputClass.isKeyDown(rendering->window, GLFW_KEY_W))
		{
			Position += Front * velocity;
		}
		if (this->rendering->inputClass.isKeyDown(rendering->window, GLFW_KEY_S))
		{
			Position -= Front * velocity;
		}
		if (this->rendering->inputClass.isKeyDown(rendering->window, GLFW_KEY_A))
		{
			Position -= Right * velocity;
		}
		if (this->rendering->inputClass.isKeyDown(rendering->window, GLFW_KEY_D))
		{
			Position += Right * velocity;
		}

		if (this->rendering->inputClass.isKeyDown(rendering->window, GLFW_KEY_LEFT_SHIFT))
		{
			speed = 5;
		}
		else
		{
			speed = 2.5f;
		}
	}
};

