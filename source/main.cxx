#include <iostream>
#include <pivot/graphics/Camera.hxx>
#include <pivot/graphics/VulkanApplication.hxx>
#include <pivot/graphics/Window.hxx>
#include <pivot/graphics/vk_utils.hxx>

#include <Logger.hpp>

#include "Scene.hxx"

Logger *logger = nullptr;

class Application : public VulkanApplication
{
public:
    Application(): VulkanApplication(), camera(glm::vec3(0, 10, 0)){};
    void init()
    {
        window.captureCursor(true);
        window.setKeyEventCallback(Window::Key::LEFT_ALT,
                                   [&](Window &window, const Window::Key key, const Window::KeyAction action) {
                                       if (action == Window::KeyAction::Pressed) {
                                           window.captureCursor(!window.captureCursor());
                                           bFirstMouse = true;
                                       }
                                   });
        window.setMouseMovementCallback([&](Window &window, const glm::dvec2 pos) {
            if (!window.captureCursor()) return;
            if (bFirstMouse) {
                last = pos;
                bFirstMouse = false;
            }
            auto offset = glm::dvec2(pos.x - last.x, last.y - pos.y);
            last = pos;
            camera.processMouseMovement(offset.x, offset.y);
        });
        scene.obj.push_back(RenderObject{
            .meshID = "viking_room",
            .objectInformation =
                {
                    .transform =
                        {
                            .translation = glm::vec3(-10.0f, 2.f, 0.0f),
                            .rotation = glm::vec3(0.f),
                            .scale = glm::vec3(4.f),
                        },
                    .textureIndex = "viking_room",
                    .materialIndex = "white",
                },
        });
        scene.obj.push_back({
            .meshID = "plane",
            .objectInformation =
                {
                    .transform =
                        {
                            .translation = glm::vec3(0.0f, 0.0f, 0.0f),
                            .rotation = glm::vec3(0, 0, 0),
                            .scale = glm::vec3(1.0f),
                        },
                    .textureIndex = "greystone",
                    .materialIndex = "white",

                },
        });
        load3DModels({"../assets/viking_room.obj", "../assets/plane.obj"});
        loadTextures({"../assets/viking_room.png", "../assets/greystone.png"});
    }

    void run()
    {
        this->VulkanApplication::init();
        while (!window.shouldClose()) {
            window.pollEvent();
            if (window.captureCursor()) {
                if (window.isKeyPressed(Window::Key::W)) camera.processKeyboard(Camera::FORWARD);
                if (window.isKeyPressed(Window::Key::S)) camera.processKeyboard(Camera::BACKWARD);
                if (window.isKeyPressed(Window::Key::D)) camera.processKeyboard(Camera::RIGHT);
                if (window.isKeyPressed(Window::Key::A)) camera.processKeyboard(Camera::LEFT);
                if (window.isKeyPressed(Window::Key::SPACE)) camera.processKeyboard(Camera::UP);
                if (window.isKeyPressed(Window::Key::LEFT_SHIFT)) camera.processKeyboard(Camera::DOWN);
            }
            draw(scene, camera);
        }
    }

public:
    glm::dvec2 last;

    bool bFirstMouse = true;
    Scene scene;
    Camera camera;
};

int main()
{
    logger = new Logger(std::cout);
    logger->start();

    Application app;
    app.init();
    app.run();
    return 0;
}
