cmake_minimum_required(VERSION 3.15.0)
project(VulkanPong)

set(CMAKE_CXX_STANDARD 20)

include_directories(
    src/ 
    src/Vulkan 
    src/models 
    src/Vulkan/Core 
    src/Window 
    src/Logging 
    src/Game
    res/ConstexprMap
    res/Timer
    res/nlohmannjson
)

add_subdirectory(res/ConstexprMap)
add_subdirectory(res/Timer)
add_subdirectory(res/nlohmannjson)

add_executable(${PROJECT_NAME} 
    src/main.cpp
    
    src/Window/Window.h
    src/Window/Window.cpp

    src/Logging/ErrorLog.h

    src/models/Quad.h
    src/models/Img.h
    src/models/Img.cpp

    src/Logging/VkDebug.h
    src/Logging/VkDebug.cpp

    src/Game/Game.h
    src/Game/Game.cpp
    src/Game/Math.h
    src/Game/Math.cpp
    src/Game/GameStartup.h
    src/Game/GameStartup.cpp

    src/Vulkan/Core/VkMain.h
    src/Vulkan/Core/VkMain.cpp
    src/Vulkan/Core/VkQueues.cpp
    src/Vulkan/Core/ExtFunctions.h
    src/Vulkan/Core/ExtFunctions.cpp
    src/Vulkan/VkSwapChain.h
    src/Vulkan/VkSwapChain.cpp
    src/Vulkan/VkCommands.h
    src/Vulkan/VkCommands.cpp
    src/Vulkan/VkRenderer.h
    src/Vulkan/VkRenderer.cpp
    src/Vulkan/VkState.h
    src/Vulkan/VkState.cpp
    src/Vulkan/VkFramebuffer.h
    src/Vulkan/VkFramebuffer.cpp
    src/Vulkan/VkShader.h
    src/Vulkan/VkShader.cpp
    src/Vulkan/VkPipeline.h
    src/Vulkan/VkPipeline.cpp
    src/Vulkan/VkQuad.h
    src/Vulkan/VkQuad.cpp
    src/Vulkan/VkMemoryAllocator.h
    src/Vulkan/VkMemoryAllocator.cpp
    src/Vulkan/VkVirtualFrame.h
    src/Vulkan/VkVirtualFrame.cpp
    src/Vulkan/VkUtils.h
    src/Vulkan/VkDescriptorSetsAllocator.h
    src/Vulkan/VkDescriptorSetsAllocator.cpp
    src/Vulkan/VkTexture2D.h
    src/Vulkan/VkTexture2D.cpp
    src/Vulkan/VkSampler.h
    src/Vulkan/VkSampler.cpp
)

target_link_libraries(${PROJECT_NAME} 
    Timer
    nl_json
    ConstexprMap
    fmt
    glfw3 X11 Xxf86vm Xrandr pthread Xi Xinerama Xcursor
    vulkan dl
)