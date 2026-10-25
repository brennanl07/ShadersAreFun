#include "main.hpp"

#include "./glprogram/shader.hpp"
#include <fstream>

int main()
{
    std::cout << "ShadersAreFun starting!" << std::endl;
    if (!glfwInit()) {
        std::cout << "GLFW Failed to initialize!\n";
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif


    GLFWwindow* window = glfwCreateWindow(800, 600, "Shaders Are Fun", NULL, NULL);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, 800, 600);

    // unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // std::string vertexShaderSource = read_file("./assets/shaders/shader.vert");
    // const char* vertexShaderSourceC = vertexShaderSource.c_str();
    // glShaderSource(vertexShader, 1, &vertexShaderSourceC, NULL);

    // unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    // std::string fragmentShaderSource = read_file("./assets/shaders/shader.frag");
    // const char* fragmentShaderSourceC = fragmentShaderSource.c_str();
    // glShaderSource(fragmentShader, 1, &fragmentShaderSourceC, NULL);

    // int success;
    // char infoLog[512];

    // glCompileShader(vertexShader);
    // glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    // if(!success)
    // {
    //     glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    //     std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    // }

    // glCompileShader(fragmentShader);
    // glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    // if(!success)
    // {
    //     glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    //     std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    // }

    // int32_t infoLogLength;
    // glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &infoLogLength);
    // std::cout << infoLogLength << std::endl;

    // unsigned int shaderProgram = glCreateProgram();
    // glAttachShader(shaderProgram, vertexShader);
    // glAttachShader(shaderProgram, fragmentShader);
    // glLinkProgram(shaderProgram);

    // glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    // if(!success) {
    //     glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    //     std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
    // }

    // glDeleteShader(vertexShader);
    // glDeleteShader(fragmentShader);

    PipelineShader pipelineShader = PipelineShader("./assets/shaders/shader.vert", "./assets/shaders/shader.frag");

    float vertices[] = {
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };

    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    unsigned int VBO;
    glGenBuffers(1, &VBO);

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    while (!glfwWindowShouldClose(window))
    {
        process_input(window);

        glClearColor(0.2, 0.15, 0.4, 1.0f);
        // glClearColor(1,1,1, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // glUseProgram(shaderProgram);
        pipelineShader.get_shader().use();
        pipelineShader.get_shader().set_uniform("ourColor", glm::vec3(1.0, 1.0, 0.1));
        glBindVertexArray(VAO);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();    
    }

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* _, int width, int height)
{
    glViewport(0, 0, width, height);
}

void process_input(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

std::string read_file(std::filesystem::path const& path)
{
    constexpr auto read_size = std::size_t(4096);
    auto stream = std::ifstream(path);
    stream.exceptions(std::ios_base::badbit);

    if (!stream) {
        throw std::ios_base::failure("file at path " + path.string() + " does not exist");
    }
    
    auto out = std::string();
    auto buf = std::string(read_size, '\0');
    while (stream.read(& buf[0], read_size)) {
        out.append(buf, 0, stream.gcount());
    }
    out.append(buf, 0, stream.gcount());
    return out;
}