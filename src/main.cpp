#include <iostream>
#include "vendor/glfw/include/GLFW/glfw3.h"

int main()
{
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(800, 600, "Pochemu", nullptr, nullptr);
    std::cin.get();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
