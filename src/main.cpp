#include <iostream>
#include <memory>
#include <glsl.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <opencv2/opencv.hpp>

int main()
{ 
    if (glfwInit() == GL_FALSE)
    {
        std::cerr << "Failed (" << __FILE__ << " at line " << __LINE__ << ") : " << "Cannot initialize GLFW" << std::endl;
        std::exit(1);
    }
    std::atexit(glfwTerminate);

    constexpr unsigned int width_screen = 960;
    constexpr unsigned int height_screen = 540;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    GLFWwindow* window = glfwCreateWindow(width_screen, height_screen, "OpenGL", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cerr << "Failed (" << __FILE__ << " at line " << __LINE__ << ") : " << "Cannot open a window" << std::endl;
        std::exit(1);
    }
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwMakeContextCurrent(window);

    if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == GL_FALSE)
    {
        std::cerr << "Failed (" << __FILE__ << " at line " << __LINE__ << ") : " << "Cannot initialize GLAD" << std::endl;
        glfwDestroyWindow(window);
        std::exit(1);
    }
        
    int success;

    constexpr unsigned int k = 1;
    constexpr unsigned int width_texture = 960 * k;
    constexpr unsigned int height_texture = 540 * k;

    // input texture
    GLuint texture_input;
    glGenTextures(1, &texture_input);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_input);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width_texture, height_texture, 0, GL_RGBA, GL_FLOAT, nullptr);
    glBindImageTexture(0, texture_input, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

    // output texture
    GLuint texture_output;
    glGenTextures(1, &texture_output);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture_output);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width_texture, height_texture, 0, GL_RGBA, GL_FLOAT, nullptr);
    glBindImageTexture(1, texture_output, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    // accumN
    constexpr unsigned int accumN = 1;
    GLuint accumN_id;
    glGenBuffers(1, &accumN_id);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, accumN_id);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(unsigned int), &accumN, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, accumN_id);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // seed
    cv::Mat img_seed(cv::Size(width_texture, height_texture), CV_32FC4);
    for (unsigned int i = 0; i < width_texture; i++)
    {
        for (unsigned int j = 0; j < height_texture; j++)
        {
            img_seed.at<cv::Vec4f>(j, i) = cv::Vec4f(
                static_cast<float>(rand()) / RAND_MAX,
                static_cast<float>(rand()) / RAND_MAX,
                static_cast<float>(rand()) / RAND_MAX,
                static_cast<float>(rand()) / RAND_MAX);
        }
    }
    GLuint seed;
    glGenTextures(1, &seed);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, seed);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width_texture, height_texture, 0, GL_RGBA, GL_FLOAT, img_seed.data);
    glBindImageTexture(2, seed, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32UI);

    // hdri
    GLuint hdri;
    glGenTextures(1, &hdri);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, hdri);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    cv::Mat img_back = cv::imread("./input_images/museum_of_ethnography_2k.hdr", -1);

    cv::cvtColor(img_back, img_back, cv::COLOR_BGRA2RGBA);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, img_back.cols, img_back.rows, 0, GL_RGBA, GL_FLOAT, img_back.data);
    glBindImageTexture(3, hdri, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);

    // theta
	float theta = 0.0f;
    GLuint theta_id;
	glGenBuffers(1, &theta_id);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, theta_id);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float), &theta, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, theta_id);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 1);

    // phi
    float phi = 0.0f;
	GLuint phi_id;
	glGenBuffers(1, &phi_id);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, phi_id);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float), &phi, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, phi_id);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 2);

    // move_x
    float move_x = 0.0f;
    GLuint move_x_id;
    glGenBuffers(1, &move_x_id);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, move_x_id);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float), &move_x, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, move_x_id);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 3);

    // move_y
    float move_y = 0.0f;
    GLuint move_y_id;
    glGenBuffers(1, &move_y_id);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, move_y_id);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float), &move_y, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, move_y_id);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 4);

    // scale
    extern float scale;
    GLuint scale_id;
    glGenBuffers(1, &scale_id);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, scale_id);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float), &scale, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, scale_id);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 5);

    // vertex shader
    GLuint vertex_shader = glad_glCreateShader(GL_VERTEX_SHADER);
    if (readShaderSource(vertex_shader, "./resources/vertex_shader.glsl") == GL_FALSE)
    {
        std::cerr << "Failed (" << __FILE__ << " at line " << __LINE__ << ") : " << "Cannot read vertex shader" << std::endl;
        glfwDestroyWindow(window);
        std::exit(1);
    }
    glCompileShader(vertex_shader);
    // check for shader compile errors
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    printShaderInfoLog(vertex_shader);
    if (success == GL_FALSE)
    {
        std::cerr << "Failed (" << __FILE__ << " at line " << __LINE__ << ") : " << "Compile error in vertex_shader" << std::endl;
        glfwDestroyWindow(window);
        std::exit(1);
    }

    // fragment shader
    GLuint fragment_shader = glad_glCreateShader(GL_FRAGMENT_SHADER);
    if (readShaderSource(fragment_shader, "./resources/fragment_shader.glsl") == GL_FALSE)
    {
        std::cerr << "Failed (" << __FILE__ << " at line " << __LINE__ << ") : " << "Cannot read fragment shader" << std::endl;
        glfwDestroyWindow(window);
        std::exit(1);
    }
    glCompileShader(fragment_shader);
    // check for shader compile errors
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    printShaderInfoLog(fragment_shader);
    if (success == GL_FALSE)
    {
        std::cerr << "Failed (" << __FILE__ << " at line " << __LINE__ << ") : " << "Compile error in fragment_shader" << std::endl;
        glfwDestroyWindow(window);
        std::exit(1);
    }

    // compute shader
    GLuint compute_shader = glad_glCreateShader(GL_COMPUTE_SHADER);
    if (readShaderSource(compute_shader, "./resources/compute_shader.glsl") == GL_FALSE)
    {
        std::cerr << "Failed (" << __FILE__ << " at line " << __LINE__ << ") : " << "Cannot read fragment shader" << std::endl;
        glfwDestroyWindow(window);
        std::exit(1);
    }
    glCompileShader(compute_shader);
    // check for shader compile errors
    glGetShaderiv(compute_shader, GL_COMPILE_STATUS, &success);
    printShaderInfoLog(compute_shader);
    if (success == GL_FALSE)
    {
        std::cerr << "Failed (" << __FILE__ << " at line " << __LINE__ << ") : " << "Compile error in compute shader" << std::endl;
        glfwDestroyWindow(window);
        std::exit(1);
    }

    // link shaders
    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    // check fo linking errors
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    printProgramInfoLog(shader_program);
    if (success == GL_FALSE)
    {
        std::cerr << "Failed (" << __FILE__ << " at line " << __LINE__ << ") : " << "Linking error in shader_program" << std::endl;
        glfwDestroyWindow(window);
        std::exit(1);
    }

    GLuint ray_program = glCreateProgram();
    glAttachShader(ray_program, compute_shader);
    glLinkProgram(ray_program);
    glGetProgramiv(ray_program, GL_LINK_STATUS, &success);
    printProgramInfoLog(ray_program);
    if (success == GL_FALSE)
    {
        std::cerr << "Failed (" << __FILE__ << " at line " << __LINE__ << ") : " << "Linking error in ray_program" << std::endl;
        glfwDestroyWindow(window);
        std::exit(1);
    }

    // vertices
    constexpr std::array<float, 15> vertices =
    {
         1,  1, 0,
         1, -1, 0,
        -1, -1, 0,
        -1,  1, 0,
         1,  1, 0,
    };

    GLuint VAO;
    GLuint VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.begin(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    bool clicked1 = false;
    bool clicked2 = false;
    extern bool scrolled;
    float theta_tmp = theta;
    float phi_tmp = phi;
    float move_x_tmp = move_x;
    float move_y_tmp = move_y;
    double px_prev_1, py_prev_1, px_prev_2, py_prev_2;
	double px_next_1, py_next_1, px_next_2, py_next_2;
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    while (glfwWindowShouldClose(window) == GL_FALSE)
    {
        processInput(window);

        glClear(GL_COLOR_BUFFER_BIT);

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1))
        {
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, accumN_id);
            glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(unsigned int), &accumN_id, GL_DYNAMIC_COPY);

            if (clicked1 == false)
            {
                glfwGetCursorPos(window, &px_prev_1, &py_prev_1);
            }
            glfwGetCursorPos(window, &px_next_1, &py_next_1);
			theta = theta_tmp + (px_next_1 - px_prev_1) * 0.01f;
			phi = phi_tmp + (py_next_1 - py_prev_1) * 0.01f;
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, theta_id);
			glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float), &theta, GL_DYNAMIC_COPY);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, phi_id);
			glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float), &phi, GL_DYNAMIC_COPY);

			clicked1 = true;
        }
        else
        {
            clicked1 = false;
            theta_tmp = theta;
            phi_tmp = phi;
        }

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2))
        {
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, accumN_id);
            glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(unsigned int), &accumN_id, GL_DYNAMIC_COPY);

            if (clicked2 == false)
            {
                glfwGetCursorPos(window, &px_prev_2, &py_prev_2);
            }
            glfwGetCursorPos(window, &px_next_2, &py_next_2);
            move_x = move_x_tmp - float(px_next_2 - px_prev_2) * 0.05f;
			move_y = move_y_tmp + float(py_next_2 - py_prev_2) * 0.05f;
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, move_x_id);
			glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float), &move_x, GL_DYNAMIC_COPY);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, move_y_id);
			glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float), &move_y, GL_DYNAMIC_COPY);

			clicked2 = true;
		}
		else
		{
			clicked2 = false;
			move_x_tmp = move_x;
			move_y_tmp = move_y;
		}

        if (scrolled)
		{
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, accumN_id);
			glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(unsigned int), &accumN_id, GL_DYNAMIC_COPY);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, scale_id);
			glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float), &scale, GL_DYNAMIC_COPY);
			scrolled = false;
		}
        
        if (glfwGetKey(window, GLFW_KEY_H))
        {
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, accumN_id);
            glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(unsigned int), &accumN, GL_DYNAMIC_COPY);

            theta = theta_tmp = 0.0f;
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, theta_id);
            glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float), &theta, GL_DYNAMIC_COPY);

            phi = phi_tmp = 0.0f;
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, phi_id);
            glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float), &phi, GL_DYNAMIC_COPY);

            move_x = move_x_tmp = 0.0f;
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, move_x_id);
            glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float), &move_x, GL_DYNAMIC_COPY);

            move_y = move_y_tmp = 0.0f;
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, move_y_id);
            glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float), &move_y, GL_DYNAMIC_COPY);

            scale = 1.0f;
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, scale_id);
            glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float), &scale, GL_DYNAMIC_COPY);
        }

        glUseProgram(ray_program);
        glDispatchCompute(width_texture, height_texture, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        glUseProgram(shader_program);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 5);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    try
    {
        glBindTexture(GL_TEXTURE_2D, texture_input);
        const std::unique_ptr<float[]> gl_texture_bytes_f = std::make_unique<float[]>(width_texture * height_texture * 3);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_BGR, GL_FLOAT, gl_texture_bytes_f.get());
        cv::Mat HDRI(height_texture, width_texture, CV_32FC3, gl_texture_bytes_f.get());
        cv::flip(HDRI, HDRI, 0);
        cv::imwrite("./output_images/hdri.hdr", HDRI);

        glBindTexture(GL_TEXTURE_2D, texture_output);
        const std::unique_ptr<unsigned char[]> gl_texture_bytes_b = std::make_unique<unsigned char[]>(width_texture * height_texture * 3);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_BGR, GL_UNSIGNED_BYTE, gl_texture_bytes_b.get());
        cv::Mat LDRI(height_texture, width_texture, CV_8UC3, gl_texture_bytes_b.get());
        cv::flip(LDRI, LDRI, 0);
        cv::imwrite("./output_images/ldri.bmp", LDRI);
    }
    catch(...)
    {
        std::cerr << "Failed (" << __FILE__ << " at line " << __LINE__ << ") : " << "Cannot save file" << std::endl;
        glfwDestroyWindow(window);
        std::exit(1);
    }

    glfwDestroyWindow(window);

    return 0;
}