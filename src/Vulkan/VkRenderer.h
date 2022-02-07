#ifndef PONG_VKAPP_RENDERER_H
#define PONG_VKAPP_RENDERER_H

#include "VkSwapChain.h"
#include "VkVirtualFrame.h"
#include "VkPipeline.h"
#include "VkQuad.h"
#include "VkTexture2D.h"
#include "VkDescriptorSetsAllocator.h"
#include "VkSampler.h"

#include <vector>
#include <functional>

namespace Pong::VkApp
{
    struct Renderer
    {
        Renderer(Swapchain& swapchain, size_t virtualFramesCount, const std::array<Img, 4>& images, std::function<std::tuple<int32_t, int32_t>()> getSurfaceSizeCallback);

        void Draw(const std::array<Pong::Quad, 4>& quads);
        void Delete();

        private:
            void CreateRenderPass();
            std::function<std::tuple<int32_t, int32_t>()> FnGetSurfaceSizeCallback;

        private:
            std::vector<VirtualFrame> _virtualFrames;
            Swapchain& _swapchain;
            Pipeline _pipeline;
            VkRenderPass _renderPass;            
            VkImageSubresourceRange _imgSubresourceRange;

            DescriptorSetsAllocator _descSetsAllocator;

            Sampler _sampler;
            std::vector<Texture2D> _textures;
            Quad _quad;
    };
}

#endif 