#include "VkMain.h"
#include "VkRenderer.h"
#include "Window.h"
#include "Game.h"
#include "GameStartup.h"
#include "Timer.h"

#include <fmt/format.h>

using namespace Pong;

int main()
{
    auto glfwContext = Window::InitGLFW();

    Window win(800, 600);

    auto instanceExts = win.getVkExtensions();
    instanceExts.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    VkApp::VkCore::CreateInstance(instanceExts);
    VkApp::VkCore::SetSurface(win.getVkSurface(VkApp::VkCore::Instance()));

    std::vector<const char*> deviceExts{VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    VkApp::VkCore::PickPhysicalDevice(deviceExts);
    VkApp::VkCore::CreateDevice(deviceExts);
    
    auto[wpx, hpx] = win.getResolutionPx();
    VkApp::Swapchain swapchain(wpx, hpx, VkApp::QuerySwapChainDetailes(VkApp::VkCore::PhysicalDevice(), VkApp::VkCore::Surface()));
    
    Pong::GameStartupData data("mod/mod.json");

    Pong::Game pong(data.lhsPlayerHorizontalPos, data.lhsPlayerSpeed, data.lhsPlayerScaling, 
                    data.rhsPlayerHorizontalPos, data.rhsPlayerSpeed, data.rhsPlayerScaling,
                    data.ballSpeed, data.ballScaling);

    VkApp::Renderer renderer(swapchain, data.framesInFlight, data.textures, [&win]{ return win.getResolutionPx(); });
    
    win.SetKeyCallback([&pong](int32_t keyCode){ pong.ControlCallback(keyCode); });

    auto time{0.};
    while(!win.isClosed())    
    {
        Timer timer(&time);
        Window::PollEvents();
       
        pong.Update(time);
        renderer.Draw(pong.getObjectsTransforms());
    }

    vkDeviceWaitIdle(VkApp::VkCore::Device());
    renderer.Delete();
    swapchain.Delete();
    VkApp::VkCore::Uninit();
}