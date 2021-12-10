#include "ImGuiCore/ImGuiManager.hxx"
#include <pivot/ecs/Core/SceneManager.hxx>

extern SceneManager gSceneManager;

ImGuiManager::ImGuiManager(pivot::graphics::VulkanApplication &app): app(app) {}

void ImGuiManager::init()
{
    for (auto [image, imageView, _]: app.get().getViewportSwapchain().getImages()) {
        text.push_back(ImGui_ImplVulkan_AddTexture(app.get().getViewportSampler(), imageView,
                                                   VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));
    }
}
void ImGuiManager::newFrame()
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();

    if (ImGui::Begin("Render")) {
        auto windowSize = ImGui::GetContentRegionAvail();
        if (windowSize.y != viewportSize.y || windowSize.x != viewportSize.x) {
            bIsResizing = true;
            std::swap(windowSize, viewportSize);
        } else if (bIsResizing) {
            app.get().recreateViewport(vk::Extent2D{
                .width = static_cast<uint32_t>(viewportSize.x),
                .height = static_cast<uint32_t>(viewportSize.y),
            });
            text.clear();
            init();
            bIsResizing = false;
        }
        ImGui::Image(text.at(app.get().getCurrentFrame()), viewportSize);
    }
    ImGui::End();
}

void ImGuiManager::render() { ImGui::Render(); }
