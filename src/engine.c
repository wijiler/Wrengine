
#include <engine.h>
#include <stdio.h>

void runSystems()
{
    for (uint64_t i = 0; i < WRECS.systemCount; i++)
    {
        WRESystem *sys = getSystem(i);
        if (sys->active)
        {
            sys->function(
                sys->componentIDs,
                sys->touchCount,
                sys->data,
                sys->datacount);
        }
    }
}

static void error_callback(int error, const char *description)
{
    fprintf(stderr, "GLFW ERROR:%i, %s\n", error, description);
}

void initializePipelines(WREngine *engine)
{
    uint64_t Len = 0;
    uint32_t *Shader = NULL;

    readShaderSPRV("./shaders/sprite.spv", &Len, &Shader);
    engine->spritePipeline.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    engine->spritePipeline.colorBlending = VK_TRUE;
    engine->spritePipeline.logicOpEnable = VK_FALSE;
    engine->spritePipeline.reasterizerDiscardEnable = VK_FALSE;
    engine->spritePipeline.polyMode = VK_POLYGON_MODE_FILL;
    engine->spritePipeline.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    engine->spritePipeline.primitiveRestartEnable = VK_FALSE;
    engine->spritePipeline.depthBiasEnable = VK_FALSE;
    engine->spritePipeline.depthTestEnable = VK_FALSE;
    engine->spritePipeline.depthClampEnable = VK_FALSE;
    engine->spritePipeline.depthClipEnable = VK_FALSE;
    engine->spritePipeline.stencilTestEnable = VK_FALSE;
    engine->spritePipeline.alphaToCoverageEnable = VK_FALSE;
    engine->spritePipeline.rastSampleCount = VK_SAMPLE_COUNT_1_BIT;
    engine->spritePipeline.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    engine->spritePipeline.cullMode = VK_CULL_MODE_NONE;
    engine->spritePipeline.colorBlendEq = (VkColorBlendEquationEXT){
        VK_BLEND_FACTOR_SRC_ALPHA,
        VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        VK_BLEND_OP_ADD,
        VK_BLEND_FACTOR_ONE,
        VK_BLEND_FACTOR_ZERO,
        VK_BLEND_OP_ADD,
    };
    engine->spritePipeline.depthBoundsEnable = VK_FALSE;
    engine->spritePipeline.alphaToOneEnable = VK_TRUE;
    engine->spritePipeline.sampleMask = UINT32_MAX;
    setPushConstantRange(&engine->spritePipeline, sizeof(VkDeviceAddress), SHADER_STAGE_VERTEX);
    createPipelineLayout(engine->Renderer.vkCore, &engine->spritePipeline);
    setShaderSLSPRV(engine->Renderer.vkCore, &engine->spritePipeline, Shader, Len);
}

void launchEngine(WREngine *engine)
{
    glfwVulkanSupported();
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    engine->Renderer.vkCore.window = glfwCreateWindow(1920, 1080, "Wrengine", NULL, NULL);
    if (!engine->Renderer.vkCore.window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    initRenderer(&engine->Renderer);
    initializePipelines(engine);
    while (!glfwWindowShouldClose(engine->Renderer.vkCore.window))
    {
        glfwPollEvents();
        runSystems();
    }
}

void destroyEngine(WREngine *engine)
{
    for (uint64_t i = 0; i < WRECS.systemCount; i++)
    {
        removeSystem(i);
    }
    destroyRenderer(&engine->Renderer);
    glfwDestroyWindow(engine->Renderer.vkCore.window);
    glfwTerminate();
}

typedef struct
{
    uint64_t textureIndex;
    VkDeviceAddress vertBuf;
} meshData;
