#include "Window.hxx"
#include <iostream>
#include <pivot/graphics/Camera.hxx>
#include <pivot/graphics/VulkanApplication.hxx>
#include <pivot/graphics/vk_utils.hxx>

Logger *logger = nullptr;

class Application : public VulkanApplication
{
public:
    void draw(float fElapsedTime) final
    {
        auto &frame = frames[currentFrame];
        uint32_t imageIndex;

        VK_TRY(vkWaitForFences(device, 1, &frame.inFlightFences, VK_TRUE, UINT64_MAX));
        VK_TRY(vkAcquireNextImageKHR(device, swapchain.getSwapchain(), UINT64_MAX, frame.imageAvailableSemaphore,
                                     nullptr, &imageIndex));

        auto &cmd = commandBuffers[imageIndex];

        VK_TRY(vkResetFences(device, 1, &frame.inFlightFences));
        VK_TRY(vkResetCommandBuffer(cmd, 0));
        VkSemaphore waitSemaphores[] = {frame.imageAvailableSemaphore};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        VkSemaphore signalSemaphores[] = {frame.renderFinishedSemaphore};

        VkSubmitInfo submitInfo{
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext = nullptr,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = waitSemaphores,
            .pWaitDstStageMask = waitStages,
            .commandBufferCount = 1,
            .pCommandBuffers = &cmd,
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = signalSemaphores,
        };

        VkCommandBufferBeginInfo beginInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = 0,
            .pInheritanceInfo = nullptr,
        };

        std::array<VkClearValue, 2> clearValues{};
        clearValues.at(0).color = {{0.0f, 0.0f, 0.0f, 0.0f}};
        clearValues.at(1).depthStencil = {1.0f, 0};

        VkDeviceSize offsets = 0;
        VkRenderPassBeginInfo renderPassInfo{
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass = renderPass,
            .framebuffer = swapChainFramebuffers[imageIndex],
            .renderArea =
                {
                    .offset = {0, 0},
                    .extent = swapchain.getSwapchainExtent(),
                },
            .clearValueCount = static_cast<uint32_t>(clearValues.size()),
            .pClearValues = clearValues.data(),
        };
        auto gpuCamera = camera.getGPUCameraData(80, swapchain.getAspectRatio());

        VK_TRY(vkBeginCommandBuffer(cmd, &beginInfo));
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1,
                                &frame.data.objectDescriptor, 0, nullptr);
        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 1, 1, &texturesSet, 0, nullptr);
        vkCmdPushConstants(cmd, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                           sizeof(gpuCamera), &gpuCamera);
        vkCmdBindVertexBuffers(cmd, 0, 1, &vertexBuffers.buffer, &offsets);
        vkCmdBindIndexBuffer(cmd, indicesBuffers.buffer, 0, VK_INDEX_TYPE_UINT32);
        vkCmdBeginRenderPass(cmd, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        {
        }
        vkCmdEndRenderPass(cmd);
        VK_TRY(vkEndCommandBuffer(cmd));
        VK_TRY(vkQueueSubmit(graphicsQueue, 1, &submitInfo, frame.inFlightFences));

        VkPresentInfoKHR presentInfo{
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .pNext = nullptr,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = signalSemaphores,
            .swapchainCount = 1,
            .pSwapchains = &(swapchain.getSwapchain()),
            .pImageIndices = &imageIndex,
            .pResults = nullptr,
        };
        VK_TRY(vkQueuePresentKHR(presentQueue, &presentInfo));
        currentFrame = (currentFrame + 1) % MAX_FRAME_FRAME_IN_FLIGHT;
    }

private:
    Camera camera;
};

int main()
{
    logger = new Logger(std::cout);
    logger->start();

    Window win("Wulkan", 500, 500);
    Application app;

    app.load3DModels({"../assets/viking_room.obj"});
    app.loadTexturess({"../assets/viking_room.png"});
    app.init(win);
    while (!win.shouldClose()) {
        win.pollEvent();
        app.draw(0);
    }

    return 0;
}