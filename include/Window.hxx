#pragma once

#include <GLFW/glfw3.h>
#include <functional>
#include <pivot/graphics/interface/IWindow.hxx>
#include <string>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

class Window : public IWindow
{
public:
    Window(std::string, unsigned, unsigned);
    Window(Window &) = delete;
    Window(const Window &) = delete;
    virtual ~Window();
    GLFWwindow *getWindow() { return window; }
    inline bool shouldClose() const { return glfwWindowShouldClose(window); }
    inline void pollEvent() { glfwPollEvents(); }
    void createSurface(const VkInstance &, VkSurfaceKHR *);
    inline bool isKeyPressed(unsigned key) const { return glfwGetKey(this->window, key) == GLFW_PRESS; }

    void setKeyCallback(GLFWkeyfun &&f);
    void setCursorPosCallback(GLFWcursorposfun &&f);
    void setResizeCallback(void(&&)(GLFWwindow *, int, int));

    void unsetKeyCallback();
    void unsetCursorPosCallback();
    void captureCursor(bool);
    void setUserPointer(void *ptr);
    void setTitle(const std::string &t);
    inline const std::string &getTitle() const { return windowName; }

    const VkExtent2D &getSize() final { return extent; }
    std::vector<const char *> getWindowRequiredExtensions() final;

private:
    void initWindow();

private:
    VkExtent2D extent;
    std::string windowName;
    GLFWwindow *window = nullptr;
};
