#include <Windows.h>
#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"

#define GLEW_STATIC
#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#define GLM_FORCE_RADIANS
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> //glm::value_ptr

#include "Utils/logfile.h"
#include "game.h"
#include "Input/Mouse.h"

#include <cstdlib>


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "config_file.h"



//Globals
extern int global_height = 720;
int global_width = 1280;
bool keys[4] = { false };

Mouse global_mouse;

static void error_callback(int error, const char* description)
{
	std::cerr << description;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	glfwGetCursorPos(window, &global_mouse.x, &global_mouse.y);

	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		global_mouse.left_btn.register_input(action == GLFW_PRESS);
	}
	if (button == GLFW_MOUSE_BUTTON_MIDDLE)
	{
		global_mouse.middle_btn.register_input(action == GLFW_PRESS);
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		global_mouse.right_btn.register_input(action == GLFW_PRESS);
	}
}



static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if ((key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q) && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_1)
	{
		keys[0] = (action == GLFW_PRESS);
	}
	if (key == GLFW_KEY_2)
	{
		keys[1] = (action == GLFW_PRESS);
	}
	if (key == GLFW_KEY_3)
	{
		keys[2] = (action == GLFW_PRESS);
	}
	if (key == GLFW_KEY_4)
	{
		keys[3] = (action == GLFW_PRESS);
	}
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	global_width = width;
	global_height = height;
}










#if USE_CONSOLE
int main(int argc, char* argv[])
#else
INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, INT nCmdShow)
#endif
{
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
		exit(EXIT_FAILURE);

	
#if DISPLAY_FULLSCREEN
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	glfwWindowHint(GLFW_DECORATED, 0);

	GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Box Rigid Body Physics", NULL, NULL);
	glViewport(0, 0, mode->width, mode->height);
#else
	GLFWwindow* window = glfwCreateWindow(global_width, global_height, "Box Rigid Body Physics", NULL, NULL);
#endif



	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	
	
	//Activate V-sync
	glfwSwapInterval(1);

	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS);
	
	glClearColor(0.0f, 0.0f, 0.5f, 1.0f);

	//Setup IMGUI
	ImGui::CreateContext();
	ImGui_ImplGlfwGL3_Init(window, true);

	ImGui::StyleColorsDark();

	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);


	#if FPS_TIMED
	int FPS = 0;
	double previousTime = glfwGetTime();
	int frameCount = 0;
	#endif

	initialize_game();

	while (!glfwWindowShouldClose(window))
	{
		#if FPS_TIMED
		// Measure speed
		double currentTime = glfwGetTime();
		frameCount++;
		// If a second has passed.
		if (currentTime - previousTime >= 1.0)
		{
			// Display the frame count here any way you want.
			//displayFPS(frameCount);
			DEBUG_LOG(frameCount << "\n");
			FPS = frameCount;

			frameCount = 0;
			previousTime = currentTime;
		}
		#endif

		// update other events like input handling 
		glfwPollEvents();

		// clear the drawing surface
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Update Game here
		if(update_game(global_mouse)) break;
		
		//Reder game here
		render_game();

		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);

	}
	glfwTerminate();

	return 0;
}