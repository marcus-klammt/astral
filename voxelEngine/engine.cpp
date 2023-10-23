#include "engine.h"

Camera camera;

bool showGui;
int flag;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int width, height;


void frameBuffer(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

void render::initalize()
{
	//init glfw and set our open-gl version
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(winSize.x, winSize.y, "voxel engine", NULL, NULL);

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, frameBuffer);
	glfwSetCursorPosCallback(window, mouse_callback);



	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460");

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;
	}

	for (Script scr : scripts)
	{
		scr.script->Start();

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
		glfwGetWindowSize(window, &width, &height);

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		camera.deltaTime = this->deltaTime;
		if (inputClass.isKeyDown(window, GLFW_KEY_ESCAPE))
		{
			onTerminate();
		}
		frame();
	}
	onTerminate();
	return;
}

void render::frame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	if (GetAsyncKeyState(VK_OEM_3) & 0x8000 && flag == 0)
	{
		flag = 1;
		showGui = !showGui;
	}
	else if (GetAsyncKeyState(VK_OEM_3) == 0)
	{
		flag = 0;
	}
	if (showGui)
	{
		camera.enabled = false;

		for (Script scr : scripts)
		{
			scr.script->OnGui();
		}
	}
	else
	{
		camera.enabled = true;
	}

	ImGui::Render();

	glClearColor(0, 0.25f, .4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	camera.handleMovement();
	for (Script scr : scripts)
	{
		scr.script->Update();
	}

	for (Shader shader : shaders)
	{
		shader.setMatrix4x4("view", camera.GetViewMatrix());

		glm::mat4 projection;
		projection = glm::perspective(glm::radians(45.0f), static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.0f);

		shader.setMatrix4x4("projection", projection);

		shader.setVector3("cameraPosition", camera.Position);
	}
	
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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

void render::onTerminate()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	for (Script scr : scripts)
	{
		scr.script->OnClose();
	}
	glfwTerminate();
	exit(0);
}