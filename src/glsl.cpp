#include <iostream>
#include <fstream>
#include <cmath>
#include <memory>
#include <glsl.hpp>

void framebuffer_size_callback(GLFWwindow* window, const int width, const int height)
{
	if (width / 16 < height / 9)
	{
		glViewport(0, (height - 9 * width / 16) / 2, width, 9 * width / 16);
	}
	else
	{
		glViewport((width - 16 * height / 9) / 2, 0, 16 * height / 9, height);
	}
}

static float MouseWheelScale = 0.05f;
float scale = 1.0f;
bool scrolled = false;
void scroll_callback(GLFWwindow* window, const double x, const double y)
{
	scale = fmaxf(0.0f, scale - MouseWheelScale * (float)y);
	scrolled = true;
}

int readShaderSource(const GLuint shader, const std::string filename)
{
	std::ifstream ifs(filename);
	if (ifs.fail())
	{
		std::cerr << "Failed (" << __FILE__ << " at line " << __LINE__ << ") : " << "Cannot open file" << filename << std::endl;
		return GL_FALSE;
	}

	ifs.seekg(0L, std::ios_base::end);

	const int length = ifs.tellg();

	std::unique_ptr<const char[]> source;
	try
	{
		source = std::make_unique<const char[]>(length);
	}
	catch(std::bad_alloc&)
	{
		std::cerr << "Failed (" << __FILE__ << " at line " << __LINE__ << ") : " << "Cannot allocate memory" << std::endl;
		return GL_FALSE;
	}

	ifs.seekg(0L, std::ios_base::beg);

	int ret = ifs.read((char*)source.get(), static_cast<std::streamsize>(length)) ? GL_TRUE : GL_FALSE;

	if (ret == GL_FALSE)
	{
		std::cerr << "Failed (" << __FILE__ << " at line " << __LINE__ << ") : " << "Cannot read file: " << filename << std::endl;
	}
	else
	{
		try
		{
			glShaderSource(shader, 1, std::make_unique<const char* const>(source.get()).get(), &length);
		}
		catch(std::bad_alloc&)
		{
			std::cerr << "Failed (" << __FILE__ << " at line " << __LINE__ << ") : " << "Cannot allocate memory" << std::endl;
			return GL_FALSE;
		}
	}

	return GL_TRUE;
}

void printShaderInfoLog(const GLuint shader)
{
	int buffSize;

	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &buffSize);

	if (buffSize > 1) 
	{
		std::unique_ptr<char[]> infoLog;
		try
		{
			infoLog = std::make_unique<char[]>(buffSize);
		}
		catch(std::bad_alloc&)
		{
			std::cerr << "Failed (" << __FILE__ << " at line " << __LINE__ << ") : " << "Cannot allocate InfoLog buffer" << std::endl;
			return;
		}
		glGetShaderInfoLog(shader, buffSize, nullptr, infoLog.get());
		std::cerr << "InfoLog:" << std::endl
				  << infoLog.get() << std::endl;
	}

	return;
}

void printProgramInfoLog(const GLuint program)
{
	int buffSize;
	
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &buffSize);

	if (buffSize > 1) 
	{
		std::unique_ptr<char[]> infoLog;
		try
		{
			infoLog = std::make_unique<char[]>(buffSize);
		}
		catch(std::bad_alloc&)
		{
			std::cerr << "Failed (" << __FILE__ << " at line " << __LINE__ << ") : " << "Cannot allocate InfoLog buffer" << std::endl;
			return;
		}
		glGetShaderInfoLog(program, buffSize, nullptr, infoLog.get());
		std::cerr << "InfoLog:" << std::endl
				  << infoLog.get() << std::endl;
	}

	return;
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}