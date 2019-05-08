#ifndef GLSL_HPP_
#define GLSL_HPP_

#include <glad/glad.h>
#include <GLFW/glfw3.h>

void framebuffer_size_callback(GLFWwindow* window, const int width, const int height);
void scroll_callback(GLFWwindow* window, const double x, const double y);
void window_focus_callback(GLFWwindow* window, const int focused);
int readShaderSource(const GLuint shader, const std::string filename);
void printShaderInfoLog(const GLuint shader);
void printProgramInfoLog(const GLuint program);
void processInput(GLFWwindow *window);

#endif