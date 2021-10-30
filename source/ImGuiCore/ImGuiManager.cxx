#include "ImGuiCore/ImGuiManager.hxx"
#include <pivot/ecs/Core/SceneManager.hxx>

extern SceneManager gSceneManager;

void ImGuiManager::newFrame(VulkanApplication &app)
{
    static auto image = ImGui_ImplVulkan_AddTexture(app.textureSampler, app.loadedTextures.at("greystone").imageView,
                                                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    static auto image2 = ImGui_ImplVulkan_AddTexture(app.textureSampler, app.loadedTextures.at("rouge").imageView,
                                                     VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();

    ImGui::Begin("h");
    ImGui::Image(image, {800, 800});
    ImGui::Image(image2, {800, 800});
    ImGui::End();
}

void ImGuiManager::render() { ImGui::Render(); }
