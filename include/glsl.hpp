#ifndef GLSL_H_
#define GLSL_H_

#include <glad/glad.h>
#include <GLFW/glfw3.h>

void framebuffer_size_callback(GLFWwindow* window, const int width, const int height);
void scroll_callback(GLFWwindow* window, const double x, const double y);
int readShaderSource(const GLuint shader, const char *filename);
void printShaderInfoLog(const GLuint shader);
void printProgramInfoLog(const GLuint program);
void processInput(GLFWwindow *window);

#endif