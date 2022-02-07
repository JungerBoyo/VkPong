#ifndef PONG_IMG_H
#define PONG_IMG_H

#include <vector>
#include <string_view>

namespace Pong
{
    using ubyte = unsigned char;

    struct Img
    {
        Img() = default;
        void Load(std::string_view path);

        std::vector<ubyte> pixels;
        int32_t width;
        int32_t height;
        int32_t channels{4};
    };
}

#endif