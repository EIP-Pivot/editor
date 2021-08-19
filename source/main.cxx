#include "Window.hxx"
#include <iostream>
#include <pivot/graphics/Camera.hxx>
#include <pivot/graphics/VulkanApplication.hxx>
#include <pivot/graphics/vk_utils.hxx>

#include <Logger.hpp>

#include "Scene.hxx"

Logger *logger = nullptr;

class Application : public VulkanApplication
{
public:
    Application(): VulkanApplication(), camera(glm::vec3(0, 10, 0)){};

    static void keyboard_callback(GLFWwindow *win, int key, int, int action, int)
    {
        auto *eng = (Application *)glfwGetWindowUserPointer(win);

        switch (action) {
            case GLFW_PRESS: {
                switch (key) {
                    case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(win, true); break;
                    case GLFW_KEY_LEFT_ALT: {
                        if (eng->bInteractWithUi) {
                            glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                            eng->bInteractWithUi = false;
                            eng->bFirstMouse = true;
                        } else {
                            glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                            eng->bInteractWithUi = true;
                        }
                    } break;
                    default: break;
                }
            } break;
            default: break;
        }
    }

    static void cursor_callback(GLFWwindow *win, double xpos, double ypos)
    {
        auto *eng = (Application *)glfwGetWindowUserPointer(win);
        if (eng->bInteractWithUi) return;

        if (eng->bFirstMouse) {
            eng->lastX = xpos;
            eng->lastY = ypos;
            eng->bFirstMouse = false;
        }
        auto xoffset = xpos - eng->lastX;
        auto yoffset = eng->lastY - ypos;

        eng->lastX = xpos;
        eng->lastY = ypos;
        eng->camera.processMouseMovement(xoffset, yoffset);
    }

public:
    float lastX;
    float lastY;

    bool bInteractWithUi = false;
    bool bFirstMouse = true;
    Scene scene;
    Camera camera;
};

int main()
{
    logger = new Logger(std::cout);
    logger->start();

    Window win("Wulkan", 800, 600);
    Application app;

    win.setUserPointer(&app);
    win.captureCursor(true);
    win.setCursorPosCallback(Application::cursor_callback);
    win.setKeyCallback(Application::keyboard_callback);
    app.scene.obj.push_back(RenderObject{
        .meshID = "viking_room",
        .objectInformation =
            {
                .transform =
                    {
                        .translation = glm::vec3(-10.0f, 2.f, 0.0f),
                        .rotation = glm::vec3(0.f),
                        .scale = glm::vec3(4.f),
                    },
                .textureIndex = 0,
            },
    });
    app.scene.obj.push_back({
        .meshID = "plane",
        .objectInformation =
            {
                .transform =
                    {
                        .translation = glm::vec3(0.0f, 0.0f, 0.0f),
                        .rotation = glm::vec3(0, 0, 0),
                        .scale = glm::vec3(1.0f),
                    },
                .textureIndex = 1,
            },
    });
    app.load3DModels({"../assets/viking_room.obj", "../assets/plane.obj"});
    app.loadTexturess({"../assets/viking_room.png", "../assets/greystone.png"});
    app.init(win);
    while (!win.shouldClose()) {
        win.pollEvent();
        if (!app.bInteractWithUi) {
            if (win.isKeyPressed(GLFW_KEY_W)) app.camera.processKeyboard(Camera::FORWARD);
            if (win.isKeyPressed(GLFW_KEY_S)) app.camera.processKeyboard(Camera::BACKWARD);
            if (win.isKeyPressed(GLFW_KEY_D)) app.camera.processKeyboard(Camera::RIGHT);
            if (win.isKeyPressed(GLFW_KEY_A)) app.camera.processKeyboard(Camera::LEFT);
            if (win.isKeyPressed(GLFW_KEY_SPACE)) app.camera.processKeyboard(Camera::UP);
            if (win.isKeyPressed(GLFW_KEY_LEFT_SHIFT)) app.camera.processKeyboard(Camera::DOWN);
        }
        app.draw(app.scene, app.camera, 0);
    }
    return 0;
}
