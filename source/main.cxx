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

#include "imgui.h"
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

#include <ImGuizmo.h>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

Logger *logger = nullptr;

template <typename T>
void printImGui(const T &);

template <>
void printImGui(const Tag &tag)
{
    std::cout << tag.name << std::endl;
}

class Application : public VulkanApplication
{
public:
    Application(): VulkanApplication(){};

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

        auto entity = gSceneManager.getCurrentLevel().CreateEntity();

        if (entity == 1997)
            gSceneManager.getCurrentLevel().GetComponent<Tag>(entity).name = "Best Entity = " + std::to_string(entity);
        else
            gSceneManager.getCurrentLevel().GetComponent<Tag>(entity).name = "Entity " + std::to_string(entity);

        gSceneManager.getCurrentLevel().AddComponent<Gravity>(
            entity, {
                        .force = glm::vec3(0.0f, randGravity(generator), 0.0f),
                    });

        gSceneManager.getCurrentLevel().AddComponent<RigidBody>(
            entity,
            {
                .velocity = glm::vec3(randVelocityXZ(generator), randVelocityY(generator), randVelocityXZ(generator)),
                .acceleration = glm::vec3(0.0f, 0.0f, 0.0f),
            });

        glm::vec3 position = glm::vec3(randPositionXZ(generator), randPositionY(generator), randPositionXZ(generator));
        glm::vec3 rotation = glm::vec3(randRotation(generator), randRotation(generator), randRotation(generator));
        glm::vec3 scale = glm::vec3(randScale(generator));

        gSceneManager.getCurrentLevel().AddComponent<RenderObject>(
            entity, {
                        .meshID = object,
                        .objectInformation =
                            {
                                .transform = Transform(position, rotation, scale),
                                .textureIndex = textures[randTexture(generator)],
                                .materialIndex = "white",
                            },
                    });
        if (gSceneManager.getCurrentLevelId() == _scene1)
            objS1.push_back(gSceneManager.getCurrentLevel().GetComponent<RenderObject>(entity));
        else
            objS2.push_back(gSceneManager.getCurrentLevel().GetComponent<RenderObject>(entity));
    }

    void addObject(std::string object)
    {
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 scale = glm::vec3(1.0f);
        auto entity = gSceneManager.getCurrentLevel().CreateEntity();
        gSceneManager.getCurrentLevel().GetComponent<Tag>(entity).name = "Entity " + std::to_string(entity);
        gSceneManager.getCurrentLevel().AddComponent<RenderObject>(
            entity, {
                        .meshID = object,
                        .objectInformation =
                            {
                                .transform = Transform(position, rotation, scale),
                                .textureIndex = "blanc",
                                .materialIndex = "white",
                            },
                    });
        if (gSceneManager.getCurrentLevelId() == _scene1)
            objS1.push_back(gSceneManager.getCurrentLevel().GetComponent<RenderObject>(entity));
        else
            objS2.push_back(gSceneManager.getCurrentLevel().GetComponent<RenderObject>(entity));
    }

    void scene1Init()
    {
        _scene1 = gSceneManager.registerLevel();
        gSceneManager.setCurrentLevelId(_scene1);
        gSceneManager.getCurrentLevel().Init();
        gSceneManager.getCurrentLevel().RegisterComponent<Gravity>();
        gSceneManager.getCurrentLevel().RegisterComponent<RigidBody>();
        gSceneManager.getCurrentLevel().RegisterComponent<Camera>();
        gSceneManager.getCurrentLevel().RegisterComponent<RenderObject>();

        gSceneManager.getCurrentLevel().RegisterSystem<PhysicsSystem>();
        {
            Signature signature;
            signature.set(gSceneManager.getCurrentLevel().GetComponentType<Gravity>());
            signature.set(gSceneManager.getCurrentLevel().GetComponentType<RigidBody>());
            signature.set(gSceneManager.getCurrentLevel().GetComponentType<RenderObject>());
            gSceneManager.getCurrentLevel().SetSystemSignature<PhysicsSystem>(signature);
        }

        auto system = gSceneManager.getCurrentLevel().RegisterSystem<ControlSystem>();
        {
            Signature signature;
            signature.set(gSceneManager.getCurrentLevel().GetComponentType<Camera>());
            gSceneManager.getCurrentLevel().SetSystemSignature<ControlSystem>(signature);
        }

        system->Init();

        std::vector<Entity> entities(2021);

        for (auto &_entity: entities) { addRandomObject("cube"); }

        Entity camera = gSceneManager.getCurrentLevel().CreateEntity();
        gSceneManager.getCurrentLevel().GetComponent<Tag>(camera).name = "Camera";
        gSceneManager.getCurrentLevel().AddComponent<Camera>(camera, Camera(glm::vec3(0, 200, 500)));
        gSceneManager.getCurrentLevel().addCamera(camera);

        addObject("plane");
    }

    void scene2Init()
    {
        _scene2 = gSceneManager.registerLevel();
        gSceneManager.setCurrentLevelId(_scene2);
        gSceneManager.getCurrentLevel().Init();
        gSceneManager.getCurrentLevel().RegisterComponent<Gravity>();
        gSceneManager.getCurrentLevel().RegisterComponent<RigidBody>();
        gSceneManager.getCurrentLevel().RegisterComponent<Camera>();
        gSceneManager.getCurrentLevel().RegisterComponent<RenderObject>();

        auto system = gSceneManager.getCurrentLevel().RegisterSystem<ControlSystem>();
        {
            Signature signature;
            signature.set(gSceneManager.getCurrentLevel().GetComponentType<Camera>());
            gSceneManager.getCurrentLevel().SetSystemSignature<ControlSystem>(signature);
        }

        system->Init();

        Entity camera = gSceneManager.getCurrentLevel().CreateEntity();
        gSceneManager.getCurrentLevel().GetComponent<Tag>(camera).name = "Camera 1";
        gSceneManager.getCurrentLevel().AddComponent<Camera>(camera, Camera(glm::vec3(0, 200, 500)));
        gSceneManager.getCurrentLevel().addCamera(camera);

        Entity camera2 = gSceneManager.getCurrentLevel().CreateEntity();
        gSceneManager.getCurrentLevel().GetComponent<Tag>(camera2).name = "Camera 2";
        gSceneManager.getCurrentLevel().AddComponent<Camera>(camera2, Camera(glm::vec3(0, 200, 500)));
        gSceneManager.getCurrentLevel().addCamera(camera2);
        addObject("plane");
    }

    void init()
    {
        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
        gSceneManager.Init();
        scene1Init();
        scene2Init();
        gSceneManager.setCurrentLevelId(_scene1);
        window.captureCursor(true);
        window.setKeyReleaseCallback(Window::Key::LEFT_ALT, [&](Window &window, const Window::Key key) {
            window.captureCursor(!window.captureCursor());
            bFirstMouse = window.captureCursor();
        });
        window.setKeyReleaseCallback(Window::Key::LEFT_CTRL, [&](Window &window, const Window::Key key) {
            if (gSceneManager.getCurrentLevelId() == _scene1)
                gSceneManager.setCurrentLevelId(_scene2);
            else
                gSceneManager.setCurrentLevelId(_scene1);
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
            ControlSystem::processMouseMovement(gSceneManager.getCurrentLevel().getCamera(),
                                                glm::dvec2(xoffset, yoffset));
        });
        load3DModels({"../assets/plane.obj", "../assets/cube.obj"});
        loadTextures({"../assets/rouge.png", "../assets/vert.png", "../assets/bleu.png", "../assets/cyan.png",
                      "../assets/orange.png", "../assets/jaune.png", "../assets/blanc.png", "../assets/violet.png"});
    }

    void editTransform(const float *cameraView, const float *cameraProjection, bool useWindow, Transform &transform)
    {
        float *matrix = glm::value_ptr(transform.getModelMatrix());
        static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
        static bool useSnap = false;
        static float snap[3] = {1.f, 1.f, 1.f};
        static float bounds[] = {-0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f};
        static float boundsSnap[] = {0.1f, 0.1f, 0.1f};
        static bool boundSizing = false;
        static bool boundSizingSnap = false;

        if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
            mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
            mCurrentGizmoOperation = ImGuizmo::ROTATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
            mCurrentGizmoOperation = ImGuizmo::SCALE;

        float matrixTranslation[3], matrixRotation[3], matrixScale[3];
        ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation, matrixRotation, matrixScale);
        ImGui::InputFloat3("Tr", matrixTranslation);
        ImGui::InputFloat3("Rt", matrixRotation);
        ImGui::InputFloat3("Sc", matrixScale);
        ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix);
        ImGuiIO &io = ImGui::GetIO();
        ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
        // ImGuizmo::DrawGrid(cameraView, cameraProjection, identityMatrix, gridSize);
        ImGuizmo::Manipulate(cameraView, cameraProjection, mCurrentGizmoOperation, mCurrentGizmoMode, matrix, NULL,
                             NULL, NULL, NULL);
    }

    void run()
    {
        float dt = 0.0f;
        float fov = 27.f;
        bool useWindow = true;
        bool toggle = false;
        Entity currentEdit = 0;
        gridSize = 100.f;
        this->VulkanApplication::init();

        while (!window.shouldClose()) {
            window.pollEvent();

            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // Guizmo
            ImGuiIO &io = ImGui::GetIO();
            ImGuizmo::BeginFrame();

            ImGui::SetNextWindowPos(ImVec2(10, 10));
            ImGui::SetNextWindowSize(ImVec2(320, 200));
            ImGui::Begin("Editor");
            if (ImGui::RadioButton("Full view", !useWindow)) useWindow = false;
            ImGui::SameLine();
            if (ImGui::RadioButton("Window", useWindow)) useWindow = true;
            ImGui::SliderInt("Grid size", &gridSize, 100, 1000);

            ImGui::Text("X: %f Y: %f", io.MousePos.x, io.MousePos.y);
            if (ImGuizmo::IsUsing()) {
                ImGui::Text("Using gizmo");
            } else {
                ImGui::Text(ImGuizmo::IsOver() ? "Over gizmo" : "");
                ImGui::SameLine();
                ImGui::Text(ImGuizmo::IsOver(ImGuizmo::TRANSLATE) ? "Over translate gizmo" : "");
                ImGui::SameLine();
                ImGui::Text(ImGuizmo::IsOver(ImGuizmo::ROTATE) ? "Over rotate gizmo" : "");
                ImGui::SameLine();
                ImGui::Text(ImGuizmo::IsOver(ImGuizmo::SCALE) ? "Over scale gizmo" : "");
            }
            ImGui::Separator();
            ImGui::Text("3d model");
            if (ImGui::BeginTabBar("Models")) {
                if (ImGui::TabItemButton("plane")) { addObject("plane"); }
                if (ImGui::TabItemButton("cube")) { addRandomObject("cube"); }
            }
            ImGui::EndTabBar();
            ImGui::Separator();

            ImGui::Text("Scene");
            if (ImGui::BeginTabBar("Scene")) {
                if (ImGui::BeginTabItem("Scene 1")) {
                    gSceneManager.setCurrentLevelId(_scene1);
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Scene 2")) {
                    gSceneManager.setCurrentLevelId(_scene2);
                    ImGui::EndTabItem();
                }
            }
            ImGui::EndTabBar();
            ImGui::Separator();
            const float *view = glm::value_ptr(gSceneManager.getCurrentLevel().getCamera().getView());
            const float *projection = glm::value_ptr(
                gSceneManager.getCurrentLevel().getCamera().getProjection(80.0f, getAspectRatio(), 0.1f));
            if (gSceneManager.getCurrentLevel().getLivingEntityCount() > 0 &&
                gSceneManager.getCurrentLevel()
                    .getSignature(currentEdit)
                    .test(gSceneManager.getCurrentLevel().GetComponentType<RenderObject>()))
                editTransform(view, projection, useWindow,
                              gSceneManager.getCurrentLevel()
                                  .GetComponent<RenderObject>(currentEdit)
                                  .objectInformation.transform);
            ImGui::End();
            // Guizmo

            ImGui::SetNextWindowPos(ImVec2(10, 220));
            ImGui::SetNextWindowSize(ImVec2(320, 150));
            ImGui::Begin("Entity");
            for (Entity entity = 0; entity < gSceneManager.getCurrentLevel().getLivingEntityCount(); entity++) {
                // Component manager: getComponents(Entity entity);
                // printImGui(gSceneManager.getCurrentLevel().GetComponent<Tag>(i));
                if (ImGui::TreeNode(gSceneManager.getCurrentLevel().GetComponent<Tag>(entity).name.c_str())) {
                    ImGui::TreePop();
                    currentEdit = entity;
                    ImGui::Separator();
                }
            }
            ImGui::End();

            ImGui::SetNextWindowPos(ImVec2(10, 340));
            ImGui::SetNextWindowSize(ImVec2(320, 150));

            ImGui::Begin("Stats");
            ImGui::Checkbox("Systems", &toggle);
            ImGui::Text("Fps: %.1f", ImGui::GetIO().Framerate);
            ImGui::Text("ms/frame %.3f", 1000.0f / ImGui::GetIO().Framerate);
            ImGui::End();

            if (toggle) { gSceneManager.getCurrentLevel().Update(dt); }
            auto startTime = std::chrono::high_resolution_clock::now();

            ImGui::Render();
            draw(gSceneManager.getCurrentLevelId() == _scene1 ? objS1 : objS2,
                 gSceneManager.getCurrentLevel().getCamera().getGPUCameraData(80.f, getAspectRatio()));
            auto stopTime = std::chrono::high_resolution_clock::now();
            dt = std::chrono::duration<float>(stopTime - startTime).count();
        }
    }

public:
    std::vector<std::reference_wrapper<const RenderObject>> objS1;
    std::vector<std::reference_wrapper<const RenderObject>> objS2;
    ImGuizmo::OPERATION mCurrentGizmoOperation;
    const float identityMatrix[16] = {1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f};
    LevelId _scene1;
    LevelId _scene2;
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