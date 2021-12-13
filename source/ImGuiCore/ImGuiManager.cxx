#include "ImGuiCore/ImGuiManager.hxx"
#include <pivot/ecs/Core/SceneManager.hxx>

extern SceneManager gSceneManager;

static std::vector<ImTextureID> updateImages(pivot::graphics::VulkanApplication &app)
{
    std::vector<ImTextureID> text;
    for (auto &imageView: app.getViewportSwapchain().getSwapchainImageViews()) {
        text.push_back(
            ImGui_ImplVulkan_AddTexture(app.getViewportSampler(), imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));
    }
    return text;
}
ImGuiManager::ImGuiManager(pivot::graphics::VulkanApplication &app): app(app) {}

void ImGuiManager::init() {}
void ImGuiManager::newFrame()
{
    if (text.empty()) text = updateImages(app.get());
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
            text = updateImages(app.get());
            bIsResizing = false;
        }
        ImGui::Image(text.at(app.get().getCurrentFrame()), viewportSize);
    }
    ImGui::End();
}

void ImGuiManager::render() { ImGui::Render(); }
