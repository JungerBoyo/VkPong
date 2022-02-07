#include "VkPipeline.h"
#include "VkShader.h"
#include "VkMain.h"
#include "VkQuad.h"

#include "ErrorLog.h"

#include <array>

void Pong::VkApp::Pipeline::Set(VkRenderPass renderPass, std::string_view vShPath, std::string_view fShPath, const std::vector<VkDescriptorSetLayout>& descSetLayouts,
                                const std::vector<VkPushConstantRange>& pushConstantRange)
{
    auto vshModule = ShaderModule(vShPath);
    auto fshModule = ShaderModule(fShPath);

    enum : uint32_t { vSh, fSh };
    std::array<VkPipelineShaderStageCreateInfo, 2> shadersInfo{{{}, {}}};
    shadersInfo[vSh].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shadersInfo[vSh].pNext = nullptr;
    shadersInfo[vSh].flags = 0;
    shadersInfo[vSh].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shadersInfo[vSh].module = vshModule;
    shadersInfo[vSh].pName = "main";
    shadersInfo[vSh].pSpecializationInfo = nullptr;

    shadersInfo[fSh].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shadersInfo[fSh].pNext = nullptr;
    shadersInfo[fSh].flags = 0;
    shadersInfo[fSh].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shadersInfo[fSh].module = fshModule;
    shadersInfo[fSh].pName = "main";
    shadersInfo[fSh].pSpecializationInfo = nullptr;

    VkPipelineVertexInputStateCreateInfo vertexInputStateInfo{};
    vertexInputStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputStateInfo.pNext = nullptr;
    vertexInputStateInfo.flags = 0;
    vertexInputStateInfo.vertexBindingDescriptionCount = 1;
    vertexInputStateInfo.pVertexBindingDescriptions = &Quad::BindingDesc;
    vertexInputStateInfo.vertexAttributeDescriptionCount = Quad::AttribDesc.size();
    vertexInputStateInfo.pVertexAttributeDescriptions = Quad::AttribDesc.data();

    VkPipelineInputAssemblyStateCreateInfo assemblyStateInfo{};
    assemblyStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    assemblyStateInfo.pNext = nullptr;
    assemblyStateInfo.flags = 0;
    assemblyStateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    assemblyStateInfo.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewportInfo{};
    viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportInfo.pNext = nullptr;
    viewportInfo.flags = 0;
    viewportInfo.pScissors = nullptr;
    viewportInfo.pViewports = nullptr;
    viewportInfo.scissorCount = 1;
    viewportInfo.viewportCount = 1;
    
    std::array<VkDynamicState, 2> dynamicStates{VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
    dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateInfo.pNext = nullptr;
    dynamicStateInfo.flags = 0;
    dynamicStateInfo.dynamicStateCount = dynamicStates.size();
    dynamicStateInfo.pDynamicStates = dynamicStates.data();
   
    VkPipelineRasterizationStateCreateInfo rasterInfo{};
    rasterInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterInfo.pNext = nullptr;
    rasterInfo.flags = 0;
    rasterInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterInfo.lineWidth = 1.f;
    rasterInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterInfo.depthBiasEnable = VK_FALSE;
    rasterInfo.depthClampEnable = VK_FALSE;
    rasterInfo.depthBiasConstantFactor = .0f;
    rasterInfo.depthBiasClamp = .0f;
    rasterInfo.depthBiasSlopeFactor = .0f;

    VkPipelineMultisampleStateCreateInfo multisamplingInfo{};
    multisamplingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisamplingInfo.pNext = nullptr;
    multisamplingInfo.flags = 0;
    multisamplingInfo.sampleShadingEnable = VK_FALSE;
    multisamplingInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisamplingInfo.minSampleShading = 1.f;
    multisamplingInfo.pSampleMask = nullptr;
    multisamplingInfo.alphaToCoverageEnable = VK_FALSE;
    multisamplingInfo.alphaToOneEnable = VK_FALSE;

    VkPipelineDepthStencilStateCreateInfo depthStencilStateInfo{};
    depthStencilStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilStateInfo.depthTestEnable = VK_FALSE;
    depthStencilStateInfo.depthWriteEnable = VK_FALSE;
    depthStencilStateInfo.depthCompareOp = VK_COMPARE_OP_ALWAYS;
    depthStencilStateInfo.depthBoundsTestEnable = VK_FALSE;
    depthStencilStateInfo.minDepthBounds = .0f;
    depthStencilStateInfo.maxDepthBounds = 1.f;
    depthStencilStateInfo.stencilTestEnable = VK_FALSE;
    depthStencilStateInfo.front = {};
    depthStencilStateInfo.back = {};

    VkPipelineColorBlendAttachmentState blendAttchState{};
    blendAttchState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    blendAttchState.colorBlendOp = VK_BLEND_OP_ADD;
    blendAttchState.alphaBlendOp = VK_BLEND_OP_ADD;
    blendAttchState.blendEnable = VK_TRUE;
    blendAttchState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    blendAttchState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    blendAttchState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    blendAttchState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;

    VkPipelineColorBlendStateCreateInfo blendStateInfo{};
    blendStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    blendStateInfo.logicOpEnable = VK_FALSE;
    blendStateInfo.logicOp = VK_LOGIC_OP_COPY;
    blendStateInfo.attachmentCount = 1;
    blendStateInfo.pAttachments = &blendAttchState;
    blendStateInfo.blendConstants[0] = 0.f;
    blendStateInfo.blendConstants[1] = 0.f;
    blendStateInfo.blendConstants[2] = 0.f;
    blendStateInfo.blendConstants[3] = 0.f;
    
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.pNext = nullptr;
    pipelineLayoutInfo.flags = 0;
    pipelineLayoutInfo.pushConstantRangeCount = pushConstantRange.size();
    pipelineLayoutInfo.pPushConstantRanges = pushConstantRange.data();
    pipelineLayoutInfo.setLayoutCount = descSetLayouts.size();
    pipelineLayoutInfo.pSetLayouts = descSetLayouts.data();

    if(vkCreatePipelineLayout(VkCore::Device(), &pipelineLayoutInfo, nullptr, &_pipelineLayout) != VK_SUCCESS)
    {
        LOG("failed to create pipeline layout, pipeline won't be created");
    }

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.pNext = nullptr;
    pipelineInfo.flags = 0;
    pipelineInfo.stageCount = shadersInfo.size();
    pipelineInfo.pStages = shadersInfo.data();
    pipelineInfo.pVertexInputState = &vertexInputStateInfo;
    pipelineInfo.pInputAssemblyState = &assemblyStateInfo;
    pipelineInfo.pTessellationState = nullptr;
    pipelineInfo.pViewportState = &viewportInfo;
    pipelineInfo.pRasterizationState = &rasterInfo;
    pipelineInfo.pMultisampleState = &multisamplingInfo;
    pipelineInfo.pDepthStencilState = &depthStencilStateInfo;
    pipelineInfo.pColorBlendState = &blendStateInfo;
    pipelineInfo.pDynamicState = &dynamicStateInfo;
    pipelineInfo.layout = _pipelineLayout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = -1;

    if(vkCreateGraphicsPipelines(VkCore::Device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &_pipeline) != VK_SUCCESS)
    {
        LOG("failed to create pipeline");
    }
}   

void Pong::VkApp::Pipeline::Delete() const 
{
    vkDestroyPipelineLayout(VkCore::Device(), _pipelineLayout, nullptr);
    vkDestroyPipeline(VkCore::Device(), _pipeline, nullptr);    
}
