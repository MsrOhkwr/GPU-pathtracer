#include <iostream>
#include <memory>
#include <thread>
#include <glsl.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <opencv2/opencv.hpp>
#include <app.hpp>
#include <check.hpp>

int main()
{ 
	if (glfwInit() == GL_FALSE)
	{
		std::cerr << "Failed (" << __FILE__ << " at line " << __LINE__ << ") : " << "Cannot initialize GLFW" << std::endl;
		return EXIT_FAILURE;
	}

	constexpr unsigned int nWindow = 1;
	const std::unique_ptr<App[]> appList = std::make_unique<App[]>(nWindow);
	const std::unique_ptr<std::thread[]> thList = std::make_unique<std::thread[]>(nWindow);

	try
	{
		for (unsigned int i = 0; i < nWindow; i++)
		{
			appList[i].init("window" + std::to_string(i));
		}
		for (unsigned int i = 0; i < nWindow; i++)
		{
			thList[i] = std::thread(&App::loop, &appList[i]);
		}
		for (unsigned int i = 0; i < nWindow; i++)
		{
			thList[i].join();
		}
		appList[0].save();
	}
	catch (const appError& e)
	{
		std::cerr << e.message() << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwTerminate();
	return EXIT_SUCCESS;
}