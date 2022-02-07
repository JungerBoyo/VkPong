#include "Img.h"
#include "ErrorLog.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

void Pong::Img::Load(std::string_view path) 
{
    ubyte* ptr = stbi_load(path.data(), &width, &height, &channels, STBI_rgb_alpha);

    if(!ptr)
    {
        LOG_ARGS("loading of img at {} failed", path);
    }

    pixels.resize(width * height * 4);
    memcpy(pixels.data(), ptr, pixels.size());

    stbi_image_free(ptr);
}
