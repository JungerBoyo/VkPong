#include "Window.h"
#include "ErrorLog.h"
#include "VkState.h"

#include <GLFW/glfw3.h>

#define WINDATA auto* data = (WinData*)(glfwGetWindowUserPointer(win))
#define GLFW(win) (GLFWwindow*)(win)


Pong::WindowContext::WindowContext() 
{
    if(!glfwInit())
    {
        LOG("Failed to init glfw3");
    }
}

Pong::WindowContext::~WindowContext() 
{
    glfwTerminate();    
}

Pong::WindowContext Pong::Window::InitGLFW() 
{
   return WindowContext();
}

Pong::Window::Window(int32_t w, int32_t h) 
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    _nativeWin = glfwCreateWindow(w, h, "", nullptr, nullptr);

    glfwSetWindowUserPointer(GLFW(_nativeWin), &_winData);

    glfwSetWindowIconifyCallback(GLFW(_nativeWin), [](GLFWwindow* win, int32_t ic)
    {
        WINDATA;
        data->minimized = static_cast<bool>(ic);
    });

    glfwSetFramebufferSizeCallback(GLFW(_nativeWin),[](GLFWwindow* win, int32_t w, int32_t h)
    {
        VkApp::WindowResizedFlag = true;
    });
}

void Pong::Window::SetKeyCallback(std::function<void(int32_t)> callback)
{
    _winData.keyCallback = callback;

    glfwSetKeyCallback(GLFW(_nativeWin), [](GLFWwindow* win, int32_t keyCode, int32_t, int32_t, int32_t)
    {
        WINDATA;
        data->keyCallback(keyCode);
    });
}

std::tuple<int32_t, int32_t> Pong::Window::getResolutionPx() const 
{
    int32_t w, h;
    glfwGetFramebufferSize(GLFW(_nativeWin), &w, &h);

    return {w, h};
}

bool Pong::Window::isMinimized() const 
{
    return _winData.minimized;
}

bool Pong::Window::isClosed() const 
{
    return glfwWindowShouldClose(GLFW(_nativeWin));
}

std::vector<const char*> Pong::Window::getVkExtensions()
{
    uint32_t extensionCount{0};
    const char** extensionsPP;

    extensionsPP = glfwGetRequiredInstanceExtensions(&extensionCount);
    std::vector<const char*> extensions(extensionCount);
    for(size_t i{0}; i<extensionCount; ++i)
        extensions[i] = extensionsPP[i];

    return extensions;
}

VkSurfaceKHR Pong::Window::getVkSurface(VkInstance instance) const 
{
    VkSurfaceKHR surface;
    if(glfwCreateWindowSurface(instance, GLFW(_nativeWin), nullptr, &surface) != VK_SUCCESS)
    {
        LOG("failed to create window surface");
        return VK_NULL_HANDLE;
    }
    
    return surface;
}

void Pong::Window::PollEvents() 
{
    glfwPollEvents();
}

Pong::Window::~Window() 
{
    glfwDestroyWindow(GLFW(_nativeWin));
}

