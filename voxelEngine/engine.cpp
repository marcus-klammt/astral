#include "engine.h"

Camera camera;

float deltaTime = 0.0f;	
float lastFrame = 0.0f;

void frameBuffer(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

void render::initalize()
{
	//init glfw and set our open-gl version
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(800, 600, "voxel engine", NULL, NULL);

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, frameBuffer);
	glfwSetCursorPosCallback(window, mouse_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;
	}

	for (Script script : scripts)
	{
		script.script->Start();
	}

	glEnable(GL_DEPTH_TEST);
	camera = Camera();
	camera.rendering = this;

	renderLoop();
}

void render::renderLoop()
{
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		camera.deltaTime = this->deltaTime;
		if (inputClass.isKeyDown(window, GLFW_KEY_ESCAPE))
		{
			glfwTerminate();
			exit(0);
		}
		frame();
	}

	glfwTerminate();
	return;
}

void render::frame()
{
	glClearColor(0, 0.25f, .4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	camera.handleMovement();
	for (Script script : scripts)
	{
		script.script->Update();
	}

	for (Shader shader : shaders)
	{
		shader.setMatrix4x4("view", camera.GetViewMatrix());
	}

	glfwPollEvents();
	glfwSwapBuffers(window);
}


bool input::isKeyDown(GLFWwindow* window, int key)
{
	if (glfwGetKey(window, key))
	{
		return true;
	}
	return false;
}

void frameBuffer(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	camera.handleMouse(xposIn, yposIn);
}