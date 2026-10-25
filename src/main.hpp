#pragma once

#include <filesystem>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

int main();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process_input(GLFWwindow *window);
std::string read_file(std::filesystem::path const& path);