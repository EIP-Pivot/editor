#include "Window.hxx"
#include <stdexcept>

Window::Window(std::string n, unsigned w, unsigned h): windowName(n)
{
    extent = {
        .width = w,
        .height = h,
    };
    initWindow();
}

Window::~Window()
{
    if (window != nullptr) glfwDestroyWindow(window);
    glfwTerminate();
}

void Window::createSurface(const VkInstance &instance, VkSurfaceKHR *surface)
{
    if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create surface");
    }
}

void Window::initWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    // glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(extent.width, extent.height, windowName.c_str(), nullptr, nullptr);
}

void Window::setKeyCallback(GLFWkeyfun &&f) { glfwSetKeyCallback(window, f); }
void Window::setCursorPosCallback(GLFWcursorposfun &&f) { glfwSetCursorPosCallback(window, f); }
void Window::setResizeCallback(void(&&f)(GLFWwindow *, int, int)) { glfwSetFramebufferSizeCallback(window, f); }
void Window::unsetKeyCallback()
{
    glfwSetKeyCallback(window, [](GLFWwindow *, int, int, int, int) {});
}
void Window::unsetCursorPosCallback()
{
    glfwSetCursorPosCallback(window, [](GLFWwindow *, double, double) {});
}

void Window::captureCursor(bool capture)
{
    if (capture) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void Window::setUserPointer(void *ptr) { glfwSetWindowUserPointer(window, ptr); }

void Window::setTitle(const std::string &t)
{
    windowName = t;
    glfwSetWindowTitle(window, windowName.c_str());
}

std::vector<const char *> Window::getWindowRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtentsions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char *> extensions(glfwExtentsions, glfwExtentsions + glfwExtensionCount);
    return extensions;
}