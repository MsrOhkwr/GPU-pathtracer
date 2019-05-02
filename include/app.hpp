#ifndef APP_HPP_
#define APP_HPP_

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

class App
{
public:
    App();
    ~App();

    void init();
    void loop();
    void save();

private:
    GLFWwindow* window;
    const unsigned int width_screen = 960;
    const unsigned int height_screen = 540;
    const unsigned int k = 1;
    const unsigned int width_texture = 960 * k;
    const unsigned int height_texture = 540 * k;
    const unsigned int accumN = 1;
    const std::string image_back_path = "./input_images/museum_of_ethnography_2k.hdr";
    const std::string vertex_shader_path = "./resources/vertex_shader.glsl";
    const std::string fragment_shader_path = "./resources/fragment_shader.glsl";
    const std::string compute_shader_path = "./resources/compute_shader.glsl";
    const std::string hdri_path = "./output_images/hdri.hdr";
    const std::string ldri_path = "./output_images/ldri.bmp";
    float theta = 0.0f;
    float phi = 0.0f;
    float move_x = 0.0f;
    float move_y = 0.0f;
    float scale = 1.0f;
    int success;
    GLuint texture_input_id;
    GLuint texture_output_id;
    GLuint accumN_id;
    GLuint seed_id;
    GLuint image_back_id;
    GLuint theta_id;
    GLuint phi_id;
    GLuint move_x_id;
    GLuint move_y_id;
    GLuint scale_id;

    GLuint vertex_shader;
    GLuint fragment_shader;
    GLuint compute_shader;

    GLuint shader_program;
    GLuint ray_program;
    
    GLuint VAO;
    GLuint VBO;
};

#endif