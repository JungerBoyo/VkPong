#ifndef WINDOW_H
#define WINDOW_H

#include "vulkan/vulkan.h"

#include <cinttypes>
#include <functional>
#include <vector>

namespace Pong
{
    struct WindowContext
    {
        WindowContext();
        ~WindowContext();
    };

    class Window
    {
        struct WinData
        {
            bool minimized{false};
            std::function<void(int32_t)> keyCallback;
        };

        public:
            static WindowContext InitGLFW();

            Window(int32_t w, int32_t h);

            void SetKeyCallback(std::function<void(int32_t)> callback);
            
            std::tuple<int32_t, int32_t> getResolutionPx() const;
            bool isMinimized() const;
            bool isClosed() const;
            
            static void PollEvents();

            static std::vector<const char*> getVkExtensions();
            VkSurfaceKHR getVkSurface(VkInstance) const;
        
            ~Window();

        private:
            WinData _winData{};
            void* /*GLFWwindow*/ _nativeWin{nullptr};
    };
}

#endif