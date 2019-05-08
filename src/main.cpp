#include <iostream>
#include <memory>
#include <glsl.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <opencv2/opencv.hpp>
#include <app.hpp>

int main()
{ 
	if (glfwInit() == GL_FALSE)
	{
		std::cerr << "Failed (" << __FILE__ << " at line " << __LINE__ << ") : " << "Cannot initialize GLFW" << std::endl;
		return EXIT_FAILURE;
	}

	App app;

	try
	{
		app.init();
		app.loop();
		app.save();
	}
	catch(const int& status)
	{
		glfwTerminate();
		return status;
	}
	
	glfwTerminate();
	return EXIT_SUCCESS;
}