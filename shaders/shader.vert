#version 450

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexCoord;

struct Transform
{
    vec2 translation;
    vec2 scaling;
    mat2 rotation;
};

layout(push_constant) uniform INSTANCE_TRANSFORMS
{
    Transform data[4];
}transforms;


layout(location = 0) out vec2 vTexCoord;
layout(location = 1) out flat uint vInstanceID;

void main()
{
    vTexCoord = inTexCoord;
    vInstanceID = gl_InstanceIndex;

    Transform transform = transforms.data[gl_InstanceIndex];

    gl_Position = vec4(transform.scaling*(transform.rotation*inPosition) + transform.translation, 0, 1.0);
}