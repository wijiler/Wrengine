
#include <engine.h>
#include <stdio.h>

#define INCREMENTAMOUNT 100

void addComponent(WREntity *entity, WREComponent *comp, void *constructionData)
{
    entity->components[entity->compCount] = comp->compID;
    entity->compCount += 1;

    if (comp->entityCount % INCREMENTAMOUNT == 0)
    {
        comp->entityIDS = realloc(comp->entityIDS, sizeof(uint64_t) * (comp->entityCount + INCREMENTAMOUNT));
        comp->entityData = realloc(comp->entityData, sizeof(void *) * (comp->entityCount + INCREMENTAMOUNT));
    }
    comp->entityIDS[comp->entityCount] = entity->entityID;
    comp->entityData[comp->entityCount] = constructionData;
    comp->entityCount += 1;
}

void registerComponent(WREComponent *component)
{
    if (WRECS.componentCount % INCREMENTAMOUNT == 0)
    {
        WRECS.components = realloc(WRECS.components, sizeof(WREComponent) * (WRECS.componentCount + INCREMENTAMOUNT));
    }
    component->compID = WRECS.componentCount;
    WRECS.components[WRECS.componentCount] = malloc(sizeof(WREComponent));
    memcpy(WRECS.components[WRECS.componentCount], component, sizeof(WREComponent));
    WRECS.componentCount += 1;
}

void registerEntity(WREntity *entity)
{
    entity->active = true;
    if (WRECS.entityCount % INCREMENTAMOUNT == 0)
    {
        WRECS.entities = realloc(WRECS.entities, sizeof(WREntity) * (WRECS.entityCount + INCREMENTAMOUNT));
    }
    for (uint64_t i = 0; i < WRECS.entityCount; i++)
    {
        if (!WRECS.entities[i]->active)
        {
            entity->entityID = i;
            WRECS.entities[i] = malloc(sizeof(WREntity));
            memcpy(WRECS.entities[i], entity, sizeof(WREntity));
            WRECS.entityCount += 1;
            return;
        }
    }
    WRECS.entities[WRECS.entityCount] = malloc(sizeof(WREntity));
    memcpy(WRECS.entities[WRECS.entityCount], entity, sizeof(WREntity));
    WRECS.entityCount += 1;
}

void registerSystem(WRESystem *system)
{
    system->active = true;
    if (WRECS.systemCount % INCREMENTAMOUNT == 0)
    {
        WRECS.systems = realloc(WRECS.systems, sizeof(WRESystem) * (WRECS.systemCount + INCREMENTAMOUNT));
    }
    for (uint64_t i = 0; i < WRECS.systemCount; i++)
    {
        if (!WRECS.systems[i]->active)
        {
            system->systemID = i;
            WRECS.systems[i] = malloc(sizeof(WRESystem));
            memcpy(WRECS.systems[i], system, sizeof(WRESystem));
            WRECS.systemCount += 1;
            return;
        }
    }
    system->systemID = WRECS.systemCount;
    WRECS.systems[WRECS.systemCount] = malloc(sizeof(WRESystem));
    memcpy(WRECS.systems[WRECS.systemCount], system, sizeof(WRESystem));
    WRECS.systemCount += 1;
}

WREComponent *getComponent(uint64_t compID)
{
    return WRECS.components[compID];
}

WREntity *getEntity(uint64_t entityID)
{
    return WRECS.entities[entityID];
}

WRESystem *getSystem(uint64_t SystemID)
{
    return WRECS.systems[SystemID];
}
void removeSystem(uint64_t systemID)
{
    WRESystem *sys = getSystem(systemID);
    sys->active = false;
    WRECS.systemCount -= 1;
}

void runECSInitFunctions()
{
    for (uint64_t i = 0; i < WRECS.componentCount; i++)
    {
        WRECS.components[i]->initializer(WRECS.components[i]);
    }
}

uint64_t getEntityIndex(WREComponent comp, uint64_t entityID)
{
    for (uint64_t i = 0; i < comp.entityCount; i++)
    {
        if (comp.entityIDS[i] == entityID)
        {
            return i;
        }
    }
}

void destroyEntity(uint64_t entityID)
{
    WREntity *entity = getEntity(entityID);
    entity->active = false;
    for (uint64_t i = 0; i < entity->compCount; i++)
    {
        WREComponent *currentComponent = getComponent(entity->components[i]);
        currentComponent->destructor(currentComponent, getEntityIndex(*currentComponent, entityID));
    }
    entity->compCount = 0;
    WRECS.entityCount -= 1;
}

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

void meshInit(WREComponent *self, void *data);
void meshDest(WREComponent *self, void *data);

void registerDefaultComponents()
{
    WREComponent meshComp = {0};
    registerComponent(&meshComp);
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
    runECSInitFunctions();
    while (!glfwWindowShouldClose(engine->Renderer.vkCore.window))
    {
        glfwPollEvents();
        runSystems();
    }
}

void destroyEngine(WREngine *engine)
{
    for (uint64_t i = 0; i < WRECS.entityCount; i++)
    {
        destroyEntity(i);
    }
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
