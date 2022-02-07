#include "VkRenderer.h"
#include "ErrorLog.h"
#include "VkState.h"
#include "VkMain.h"
#include "VkUtils.h"

#include <array>

Pong::VkApp::Renderer::Renderer(Swapchain& swapchain, size_t virtualFramesCount, const std::array<Img, 4>& images, std::function<std::tuple<int32_t, int32_t>()> getSurfaceSizeCallback) 
    : _sampler(VK_FILTER_NEAREST),
      _swapchain(swapchain), FnGetSurfaceSizeCallback(getSurfaceSizeCallback),
      _descSetsAllocator({{VK_DESCRIPTOR_TYPE_SAMPLER, 1}, {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 4}}, 1)
{
    for(const Img& img : images)
        _textures.emplace_back(img, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);

    CreateRenderPass();

    auto[vpW, vpH] = FnGetSurfaceSizeCallback();
    _virtualFrames.reserve(virtualFramesCount);
    for(size_t i{0}; i<virtualFramesCount; ++i)
        _virtualFrames.emplace_back(_renderPass, vpW, vpH, Swapchain::surfaceFormat.format, swapchain.getImage(0), VK_IMAGE_ASPECT_COLOR_BIT);


    enum : uint32_t { sampler, textures };
    std::vector<VkDescriptorSetLayoutBinding> descSetLayoutBindings(2, {{}});
    descSetLayoutBindings[sampler].binding = 0;
    descSetLayoutBindings[sampler].descriptorCount = 1;
    descSetLayoutBindings[sampler].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
    descSetLayoutBindings[sampler].pImmutableSamplers = nullptr;
    descSetLayoutBindings[sampler].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    descSetLayoutBindings[textures].binding = 1;
    descSetLayoutBindings[textures].descriptorCount = 4;
    descSetLayoutBindings[textures].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    descSetLayoutBindings[textures].pImmutableSamplers = nullptr;
    descSetLayoutBindings[textures].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
  
    _descSetsAllocator.Allocate(descSetLayoutBindings);

    std::vector<VkDescriptorImageInfo> samplerInfo{{_sampler, VK_NULL_HANDLE, {}}};
    std::vector<VkDescriptorImageInfo> imgInfos{_textures[0].Info(), _textures[1].Info(), _textures[2].Info(), _textures[3].Info()};

    _descSetsAllocator.Update({0, 0}, {0, 1}, {VK_DESCRIPTOR_TYPE_SAMPLER, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE}, {&samplerInfo, &imgInfos});

    VkPushConstantRange pushConstantRange{};
    pushConstantRange.size = 4 * Pong::Quad::transform_data_size;
    pushConstantRange.offset = 0;
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    _pipeline.Set(_renderPass, "shaders/bin/vert.spv", "shaders/bin/frag.spv", _descSetsAllocator.getLayouts(), {pushConstantRange});

    _imgSubresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    _imgSubresourceRange.baseArrayLayer = 0;
    _imgSubresourceRange.layerCount = 1;
    _imgSubresourceRange.baseMipLevel = 0;
    _imgSubresourceRange.levelCount = 1;
}

/*
    There are 4 quads, each has texture. 1 quad = background. 
    3 quads => push constants with transforms and textures. 


*/

void Pong::VkApp::Renderer::CreateRenderPass()
{
    /*
        Render pass
            - clear on load => implicit image layout transition (presentation to transfer?)
            - attachment layout transition through dependecies

    */
    VkAttachmentDescription colorAttch;
    colorAttch.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttch.flags = 0;
    colorAttch.format = Swapchain::surfaceFormat.format;
    colorAttch.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttch.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    colorAttch.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttch.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttch.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttch.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    VkAttachmentReference colorAttchRef;
    colorAttchRef.attachment = 0;
    colorAttchRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpassDesc{};
    subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDesc.flags = 0;
    subpassDesc.inputAttachmentCount = 0;
    subpassDesc.pInputAttachments = nullptr;
    subpassDesc.preserveAttachmentCount = 0;
    subpassDesc.pPreserveAttachments = nullptr;
    subpassDesc.pResolveAttachments = nullptr;
    subpassDesc.colorAttachmentCount = 1;
    subpassDesc.pColorAttachments = &colorAttchRef;
    subpassDesc.pDepthStencilAttachment = nullptr;

    VkSubpassDependency srcDependency{};
    srcDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    srcDependency.dstSubpass = 0;
    srcDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    srcDependency.srcAccessMask = 0;
    srcDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    srcDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    srcDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    /// this is gruaranteed by implemetation to exist implicitly
    //VkSubpassDependency dstDependency{};
    //dstDependency.srcSubpass = 0;
    //dstDependency.dstSubpass = VK_SUBPASS_EXTERNAL;
    //dstDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    //dstDependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    //dstDependency.dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    //dstDependency.dstAccessMask = 0;
    //dstDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    VkRenderPassCreateInfo renderpassInfo{};
    renderpassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderpassInfo.pNext = nullptr;
    renderpassInfo.flags = 0;
    renderpassInfo.subpassCount = 1;
    renderpassInfo.pSubpasses = &subpassDesc;
    renderpassInfo.dependencyCount = 1;
    renderpassInfo.pDependencies = &srcDependency;
    renderpassInfo.attachmentCount = 1;
    renderpassInfo.pAttachments = &colorAttch;
 
    if(vkCreateRenderPass(VkCore::Device(), &renderpassInfo, nullptr, &_renderPass) != VK_SUCCESS)
    {
        LOG("failed to create renderpass");
    }
}

void Pong::VkApp::Renderer::Delete() 
{   
    for(const auto& vFrame : _virtualFrames)
        vFrame.Delete();

    _quad.Delete();

    for(const auto& tex : _textures)
        tex.Delete();
   
    _sampler.Delete();

    _descSetsAllocator.Delete();
    _pipeline.Delete();
    vkDestroyRenderPass(VkCore::Device(),_renderPass, nullptr);
}

void Pong::VkApp::Renderer::Draw(const std::array<Pong::Quad, 4>& quads)
{
    static size_t virutalFrameIndex = 0;
    auto& vFrame = _virtualFrames[virutalFrameIndex];

    virutalFrameIndex = (virutalFrameIndex + 1) % _virtualFrames.size();

    vkWaitForFences(VkCore::Device(), 1, &vFrame.cmdBufferSubmissionFence, VK_TRUE, UINT64_MAX);
    vkResetFences(VkCore::Device(), 1, &vFrame.cmdBufferSubmissionFence);
    vFrame.cmdAlloc.ResetCommandBuffers();

    uint32_t imgIndex;
    auto result = vkAcquireNextImageKHR(VkCore::Device(), _swapchain.get(), UINT64_MAX, vFrame.imgAquiredSemaphore, VK_NULL_HANDLE, &imgIndex);

    if(result == VK_ERROR_OUT_OF_DATE_KHR || result != VK_SUCCESS)
    {
        auto[W, H] = FnGetSurfaceSizeCallback();

        vkDeviceWaitIdle(VkCore::Device());
        _swapchain.Recreate(W, H);

        vkAcquireNextImageKHR(VkCore::Device(), _swapchain.get(), UINT64_MAX, vFrame.imgAquiredSemaphore, VK_NULL_HANDLE, &imgIndex);
    }

    auto schExtent = _swapchain.getExtent();
    auto schImg = _swapchain.getImage(imgIndex);
    vFrame.fbo.Resize(_renderPass, schImg, schExtent.width, schExtent.height);

    VkCommandBuffer cmdBuffer;
    {
        auto scope = vFrame.cmdAlloc.RecordBuffer(0, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
        cmdBuffer = scope.getRecordedCmdBuffer();

        bool queueSame = VkCore::GraphicsQueueIndex() == VkCore::PresentationQueueIndex();
        if(!queueSame)
        {
            VkImageMemoryBarrier imgBarrierPresentToGraphics{};
            imgBarrierPresentToGraphics.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            imgBarrierPresentToGraphics.subresourceRange = _imgSubresourceRange;
            imgBarrierPresentToGraphics.image = schImg;
            imgBarrierPresentToGraphics.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            imgBarrierPresentToGraphics.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            imgBarrierPresentToGraphics.srcQueueFamilyIndex = VkCore::PresentationQueueIndex();
            imgBarrierPresentToGraphics.dstQueueFamilyIndex = VkCore::GraphicsQueueIndex();
            imgBarrierPresentToGraphics.srcAccessMask = 0; // because access is controlled by semaphore
            imgBarrierPresentToGraphics.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

            vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 
                                 VK_DEPENDENCY_BY_REGION_BIT, 0, nullptr, 0, nullptr, 1, &imgBarrierPresentToGraphics);
        }


        VkClearValue clearValue{{.4f, .12f, .123f, 1.f}};
        VkRenderPassBeginInfo renderPassBeginInfo{VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
        renderPassBeginInfo.clearValueCount = 1;
        renderPassBeginInfo.pClearValues = &clearValue;
        renderPassBeginInfo.renderArea.extent = schExtent;
        renderPassBeginInfo.renderArea.offset = {0,0};
        renderPassBeginInfo.renderPass = _renderPass;
        renderPassBeginInfo.framebuffer = vFrame.fbo.get();

        vkCmdBeginRenderPass(cmdBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);

        auto[viewport, scissor] = CreateViewportScissor(schExtent.width, schExtent.height);

        vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);
        vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);

        auto vbo = _quad.vbo();
        VkDeviceSize offset{0};
        vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &vbo, &offset);
   
        const auto& descSets = _descSetsAllocator.getSets();
        vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline.getLayout(), 0, descSets.size(), descSets.data(), 0, nullptr);

        vkCmdPushConstants(cmdBuffer, _pipeline.getLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, 4 * Pong::Quad::transform_data_size, quads.data());

        vkCmdDraw(cmdBuffer, Pong::Quad::vertices.size(), 4, 0, 0);

        vkCmdEndRenderPass(cmdBuffer);

        if(!queueSame)
        {
            VkImageMemoryBarrier imgBarrierGraphicsToPresent{};
            imgBarrierGraphicsToPresent.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            imgBarrierGraphicsToPresent.subresourceRange = _imgSubresourceRange;
            imgBarrierGraphicsToPresent.image = schImg;
            imgBarrierGraphicsToPresent.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            imgBarrierGraphicsToPresent.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            imgBarrierGraphicsToPresent.srcQueueFamilyIndex = VkCore::GraphicsQueueIndex();
            imgBarrierGraphicsToPresent.dstQueueFamilyIndex = VkCore::PresentationQueueIndex();
            imgBarrierGraphicsToPresent.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            imgBarrierGraphicsToPresent.dstAccessMask = 0; // because of sync with semaphore 

            vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, /*semaphore sync so we dont want any further blocking*/
                                 VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,  VK_DEPENDENCY_BY_REGION_BIT, 0, nullptr, 0, nullptr, 1, &imgBarrierGraphicsToPresent);
        }
    }


    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = nullptr;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &vFrame.imgAquiredSemaphore;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &vFrame.renderFinishedSemaphore;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdBuffer;

    VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    submitInfo.pWaitDstStageMask = &waitDstStageMask;

    if(vkQueueSubmit(VkCore::GraphicsQueue(), 1, &submitInfo, vFrame.cmdBufferSubmissionFence) != VK_SUCCESS)
    {
        LOG("failed to clear image from swapchain");
        return;
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = nullptr;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &vFrame.renderFinishedSemaphore;
    presentInfo.swapchainCount = 1;

    auto nativeSwapchain = _swapchain.get();
    presentInfo.pSwapchains = &nativeSwapchain;
    presentInfo.pImageIndices = &imgIndex;
    presentInfo.pResults = nullptr;

    result = vkQueuePresentKHR(VkCore::PresentationQueue(), &presentInfo);

    if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || WindowResizedFlag || result != VK_SUCCESS)
    {
        WindowResizedFlag = false;
        auto[W, H] = FnGetSurfaceSizeCallback();

        vkDeviceWaitIdle(VkCore::Device());
        _swapchain.Recreate(W, H);
    }
}


