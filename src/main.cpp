#include <inttypes.h>
#include <renderer.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <stdio.h>
#include <windowing.h>

renderer_t renderer;
winf_t wininfo = {};
GraphBuilder builder = {};

typedef struct
{
    VkDeviceAddress address;
} pushConstants;

typedef struct
{
    float position[3];
    float color[3];
} vertex;

vertex verts[4] = {
    {{-0.5f, 0.5f, 1.0f}, {1.0f, 1.0f, 1.0f}},
    {{-0.5f, -0.5f, 1.0f}, {1.0f, 1.0f, 1.0f}},
    {{0.5f, 0.5f, 1.0f}, {1.0f, 1.0f, 1.0f}},
    {{0.5f, -0.5f, 1.0f}, {1.0f, 1.0f, 1.0f}},
};

int ImageIndex = 0;
int FrameIndex = 0;
int Index = 0;
void loop()
{
    FrameIndex++;
    Index = FrameIndex % FRAMECOUNT;

    drawRenderer(&renderer, Index);
}

void init()
{
    renderer.meshHandler.vertexSize = sizeof(vertex);
    initRenderer(&renderer);

    uint64_t Len = 0;
    uint32_t *Shader = NULL;

    readShaderSPRV("./shaders/2d.spv", &Len, &Shader);

    graphicsPipeline pl = {};

    pl.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    pl.colorBlending = VK_TRUE;
    pl.logicOpEnable = VK_FALSE;
    pl.reasterizerDiscardEnable = VK_FALSE;
    pl.polyMode = VK_POLYGON_MODE_FILL;
    pl.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    pl.primitiveRestartEnable = VK_FALSE;
    pl.depthBiasEnable = VK_FALSE;
    pl.depthTestEnable = VK_FALSE;
    pl.depthClampEnable = VK_FALSE;
    pl.depthClipEnable = VK_FALSE;
    pl.stencilTestEnable = VK_FALSE;
    pl.alphaToCoverageEnable = VK_FALSE;
    pl.rastSampleCount = VK_SAMPLE_COUNT_1_BIT;
    pl.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    pl.cullMode = VK_CULL_MODE_NONE;
    pl.colorBlendEq = (VkColorBlendEquationEXT){
        VK_BLEND_FACTOR_SRC_ALPHA,
        VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        VK_BLEND_OP_ADD,
        VK_BLEND_FACTOR_ONE,
        VK_BLEND_FACTOR_ZERO,
        VK_BLEND_OP_ADD,
    };
    pl.depthBoundsEnable = VK_FALSE;
    pl.alphaToOneEnable = VK_TRUE;
    pl.sampleMask = UINT32_MAX;

    setShaderSLSPRV(renderer.vkCore, &pl, Shader, Len);

    uint32_t indices[6] = {0, 1, 2, 2, 3, 1};

    Mesh triangle = createMesh(renderer, 4, verts, 6, indices, 1);
    submitMesh(triangle, &renderer);

    setPushConstantRange(&pl, sizeof(pushConstants), SHADER_STAGE_VERTEX);

    int texWidth, texHeight, texChannels;
    stbi_uc *img = stbi_load("assets/birb.png", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    Texture birby = createTextureImage(renderer.vkCore, texWidth, texHeight);
    BufferCreateInfo bci = {
        texWidth * texHeight * 4,
        BUFFER_USAGE_TRANSFER_SRC,
        CPU_ONLY,
    };
    Buffer buf;
    createBuffer(renderer.vkCore, bci, &buf);
    pushDataToBuffer(img, texWidth * texHeight * 4, buf, 0);
    copyDataToTextureImage(renderer.vkCore, &birby, &buf, texWidth, texHeight);
    destroyBuffer(buf, renderer.vkCore);
    stbi_image_free(img);
    write_textureDescriptorSet(renderer.vkCore, birby.imgview, renderer.vkCore.linearSampler, 0);

    createPipelineLayout(renderer.vkCore, &pl);
    RenderPass scenePass = sceneDraw(&renderer);
    scenePass.gPl = pl;
    VkOffset2D offSet = {0, 0};
    scenePass.drawArea = (drawArea){
        &offSet,
        &renderer.vkCore.extent,
    };
    addPass(&builder, &scenePass);
    renderer.rg = &builder;
}
int main(void)
{
    wininfo.name = (char *)"Thing";
    wininfo.w = 1920;
    wininfo.h = 1080;
    launch_window(wininfo, &renderer, loop, init);

    destroyRenderer(&renderer);

    glfwDestroyWindow(renderer.vkCore.window);
    glfwTerminate();
}