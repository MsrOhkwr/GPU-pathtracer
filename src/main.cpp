#include <iostream>
#include <memory>
#include <glsl.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <opencv2/opencv.hpp>
#include <app.hpp>

int main()
{ 
    App app;

    app.init();
    app.loop();
    app.save();

    return EXIT_SUCCESS;
}