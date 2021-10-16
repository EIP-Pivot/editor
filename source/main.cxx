#include <chrono>
#include <iostream>
#include <random>

#include <pivot/graphics/VulkanApplication.hxx>
#include <pivot/graphics/Window.hxx>
#include <pivot/graphics/types/RenderObject.hxx>
#include <pivot/graphics/vk_utils.hxx>

#include <pivot/ecs/Components/Gravity.hxx>
#include <pivot/ecs/Components/RigidBody.hxx>
#include <pivot/ecs/Components/Tag.hxx>

#include <pivot/ecs/Core/Event.hxx>
#include <pivot/ecs/Systems/ControlSystem.hxx>
#include <pivot/ecs/ecs.hxx>

#include <Logger.hpp>

// #include "Scene.hxx"
#include "Systems/PhysicsSystem.hxx"
#include <pivot/ecs/Core/Scene.hxx>
#include <pivot/ecs/Core/SceneManager.hxx>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include "ImGuiCore/ComponentEditor.hxx"
#include "ImGuiCore/Editor.hxx"
#include "ImGuiCore/EntityModule.hxx"
#include "ImGuiCore/ImGuiManager.hxx"
#include "ImGuiCore/SystemsEditor.hxx"

#include "FrameLimiter.hpp"

Logger *logger = nullptr;

class Application : public VulkanApplication
{
public:
    Application(): VulkanApplication(), editor(Editor()), camera(editor.getCamera()){};

    void addRandomObject(std::string object)
    {
        std::array<std::string, 8> textures = {"rouge", "vert", "bleu", "cyan", "orange", "jaune", "blanc", "violet"};
        std::random_device generator;
        std::uniform_real_distribution<float> randPositionY(0.0f, 50.0f);
        std::uniform_real_distribution<float> randPositionXZ(-50.0f, 50.0f);
        std::uniform_real_distribution<float> randRotation(0.0f, 3.0f);
        std::uniform_real_distribution<float> randColor(0.0f, 1.0f);
        std::uniform_real_distribution<float> randGravity(-10.0f, -1.0f);
        std::uniform_real_distribution<float> randVelocityY(10.0f, 200.0f);
        std::uniform_real_distribution<float> randVelocityXZ(-200.0f, 200.0f);
        std::uniform_real_distribution<float> randScale(0.5f, 1.0f);
        std::uniform_int_distribution<int> randTexture(0, textures.size() - 1);
        auto newEntity = entity.addEntity();
        if (newEntity == 1997)
            gSceneManager.getCurrentLevel().GetComponent<Tag>(newEntity).name = "Best Entity = " + std::to_string(newEntity);
        else
            gSceneManager.getCurrentLevel().GetComponent<Tag>(newEntity).name = "Entity " + std::to_string(newEntity);
        componentEditor.addComponent<Gravity>(newEntity, {
                        .force = glm::vec3(0.0f, randGravity(generator), 0.0f),
                    });
        componentEditor.addComponent<RigidBody>(
            newEntity,
            {
                .velocity = glm::vec3(randVelocityXZ(generator), randVelocityY(generator), randVelocityXZ(generator)),
                .acceleration = glm::vec3(0.0f, 0.0f, 0.0f),
            });
        glm::vec3 position = glm::vec3(randPositionXZ(generator), randPositionY(generator), randPositionXZ(generator));
        glm::vec3 rotation = glm::vec3(randRotation(generator), randRotation(generator), randRotation(generator));
        glm::vec3 scale = glm::vec3(randScale(generator));
        componentEditor.addComponent<RenderObject>(newEntity, {
                        .meshID = object,
                        .objectInformation =
                            {
                                .transform = Transform(position, rotation, scale),
                                .textureIndex = textures[randTexture(generator)],
                                .materialIndex = "white",
                            },
                    });
    }

    void DemoScene()
    {
        editor.addScene("Demo");
        systemsEditor.addSystem<PhysicsSystem>();

        std::vector<Entity> entities(2021);

        for (auto &_entity: entities) { addRandomObject("cube"); }
    }

    void loadScene()
    {
        LevelId defaultScene = editor.addScene("Default");
        DemoScene();
        gSceneManager.setCurrentLevelId(defaultScene);
    }

    void init()
    {
        gSceneManager.Init();
        loadScene();

        window.captureCursor(true);
        window.setKeyReleaseCallback(Window::Key::LEFT_ALT, [&](Window &window, const Window::Key key) {
            window.captureCursor(!window.captureCursor());
            bFirstMouse = window.captureCursor();
        });
        window.setKeyReleaseCallback(Window::Key::V, [&](Window &window, const Window::Key key) {
            gSceneManager.getCurrentLevel().switchCamera();
        });

        auto key_lambda_press = [&](Window &window, const Window::Key key) {
            button.set(static_cast<std::size_t>(key));
            Event event(Events::Window::INPUT);
            event.SetParam(Events::Window::Input::INPUT, button);
            gSceneManager.getCurrentLevel().SendEvent(event);
        };
        auto key_lambda_release = [&](Window &window, const Window::Key key) {
            button.reset(static_cast<std::size_t>(key));
            Event event(Events::Window::INPUT);
            event.SetParam(Events::Window::Input::INPUT, button);
            gSceneManager.getCurrentLevel().SendEvent(event);
        };
        // Press action
        window.setKeyPressCallback(Window::Key::W, key_lambda_press);
        window.setKeyPressCallback(Window::Key::S, key_lambda_press);
        window.setKeyPressCallback(Window::Key::D, key_lambda_press);
        window.setKeyPressCallback(Window::Key::A, key_lambda_press);
        window.setKeyPressCallback(Window::Key::SPACE, key_lambda_press);
        window.setKeyPressCallback(Window::Key::LEFT_SHIFT, key_lambda_press);
        // Release action
        window.setKeyReleaseCallback(Window::Key::W, key_lambda_release);
        window.setKeyReleaseCallback(Window::Key::S, key_lambda_release);
        window.setKeyReleaseCallback(Window::Key::D, key_lambda_release);
        window.setKeyReleaseCallback(Window::Key::A, key_lambda_release);
        window.setKeyReleaseCallback(Window::Key::SPACE, key_lambda_release);
        window.setKeyReleaseCallback(Window::Key::LEFT_SHIFT, key_lambda_release);

        window.setMouseMovementCallback([&](Window &window, const glm::dvec2 pos) {
            if (!window.captureCursor()) return;

            if (bFirstMouse) {
                last = pos;
                bFirstMouse = false;
            }
            auto xoffset = pos.x - last.x;
            auto yoffset = last.y - pos.y;

            last = pos;
            ControlSystem::processMouseMovement(camera, glm::dvec2(xoffset, yoffset));
        });
        load3DModels({"../assets/plane.obj", "../assets/cube.obj"});
        loadTextures({"../assets/rouge.png", "../assets/vert.png", "../assets/bleu.png", "../assets/cyan.png",
                      "../assets/orange.png", "../assets/jaune.png", "../assets/blanc.png", "../assets/violet.png"});
    }

    void run()
    {
        float dt = 0.0f;
        float fov = 27.f;
        bool useWindow = true;
        Entity currentEdit = 0;
        gridSize = 100.f;
        this->VulkanApplication::init();
        FrameLimiter<60> fpsLimiter;
        while (!window.shouldClose()) {
            auto startTime = std::chrono::high_resolution_clock::now();
            window.pollEvent();

            imGuiManager.newFrame();

            editor.create();
            if (!editor.getRun())
            {
                editor.setAspectRatio(getAspectRatio());
                entity.create();
                entity.hasSelected() ? componentEditor.create(entity.getEntitySelected()) : componentEditor.create();
                systemsEditor.create();

                if (entity.hasSelected() &&
                    gSceneManager.getCurrentLevel().hasComponent<RenderObject>(entity.getEntitySelected())) {
                    editor.DisplayGuizmo(entity.getEntitySelected());
                }
            }
            else {
                gSceneManager.getCurrentLevel().Update(dt);
            }

            imGuiManager.render();

            draw(componentEditor.getObject(), camera.getGPUCameraData(80.f, getAspectRatio()));
            fpsLimiter.sleep();
            auto stopTime = std::chrono::high_resolution_clock::now();
            dt = std::chrono::duration<float>(stopTime - startTime).count();
        }
    }

public:
    ImGuiManager imGuiManager;
    Editor editor;
    EntityModule entity;
    ComponentEditor componentEditor;
    SystemsEditor systemsEditor;
    Camera &camera;
    glm::dvec2 last;

    bool bFirstMouse = true;
    std::bitset<UINT16_MAX> button;
    int gridSize;
};

int main()
try {
    logger = new Logger(std::cout);
    logger->start();

    Application app;
    app.init();
    app.run();
    return 0;
} catch (std::exception &e) {
    logger->err("THROW") << e.what();
    LOGGER_ENDL;
    return 1;
}