#version 450

layout(location = 0) out vec4 fragment;

layout(set = 0, binding = 0) uniform sampler texSampler;
layout(set = 0, binding = 1) uniform texture2D[4] textures;

layout(location = 0) in vec2 vTexCoord;
layout(location = 1) in flat uint vInstanceID;

void main()
{
    //fragment = vec4(0.5, 0.12, 0.424, 1.0);
    fragment = texture(sampler2D(textures[vInstanceID], texSampler), vTexCoord);
}
