#include <engine.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <stdio.h>

// ---GLOBALS
MeshHandler spriteHandler = {0};
// ---ECSBEG
typedef struct
{
    uint64_t componentCount;
    WREComponent **components;
    uint64_t systemCount;
    WRESystem **systems;
    WREScene *activeScene;
    uint64_t entityCount;
    WREntity **entities;
} systemManager;
systemManager WRECS = {0};
#include <ecs.h>

#include <stdlib.h>
#include <string.h>

#define INCREMENTAMOUNT 100

void addComponent(WREntity *entity, WREComponent *comp, void *constructionData)
{
    entity->components = realloc(entity->components, WRECS.componentCount);
    entity->components[comp->compID] = 1;

    comp->entityData = realloc(comp->entityData, sizeof(void *) * (WRECS.componentCount));
    comp->entityData[entity->entityID] = constructionData;
}

void addEntitySystem(WREntity *entity, WREntitySystemfunction function)
{
    if (entity->entitySysCount % INCREMENTAMOUNT == 0)
    {
        entity->entitySystems = realloc(entity->entitySystems, sizeof(WREComponent) * (entity->entitySysCount + INCREMENTAMOUNT));
    }
    entity->entitySystems[entity->entitySysCount] = function;
    entity->entitySysCount += 1;
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

void registerEntity(WREntity *entity, WREScene *scene)
{
    entity->active = true;
    scene->entities = realloc(scene->entities, sizeof(uint8_t) * (WRECS.entityCount) + 1);
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
            scene->entities[entity->entityID] = 1;
            for (uint64_t i = 0; i < WRECS.componentCount; i++)
            {
                if (entity->components[i] == 1)
                {
                    WRECS.components[i]->initializer(WRECS.components[i], entity->entityID);
                }
            }
            return;
        }
    }

    entity->entityID = WRECS.entityCount;
    for (uint64_t i = 0; i < WRECS.componentCount; i++)
    {
        if (entity->components[i] == 1)
        {
            WRECS.components[i]->initializer(WRECS.components[i], entity->entityID);
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

void destroyEntity(uint64_t entityID)
{
    WREntity *entity = getEntity(entityID);
    entity->active = false;
    for (uint64_t i = 0; i < WRECS.componentCount; i++)
    {
        if (entity->components[i] == 1)
        {
            WRECS.components[i]->destructor(WRECS.components[i], entity->entityID);
        }
    }
    WRECS.entityCount -= 1;
}

WREScene createScene()
{
    WREScene scene = {0};
    // logic might go here one day
    return scene;
}

void setActiveScene(WREScene *scene)
{
    WRECS.activeScene = scene;
}
WREComponent createComponent(ComponentFunction init, ComponentFunction destroy)
{
    WREComponent comp = {
        0,
        init,
        destroy,
        NULL,
    };
    return comp;
}
// --ECSEND

void runEntitySystems()
{
    for (uint64_t i = 0; i < WRECS.systemCount; i++)
    {
        WRECS.systems[i]->function(
            WRECS.systems[i]->componentIDs,
            WRECS.systems[i]->touchCount,
            WRECS.systems[i]->data,
            WRECS.systems[i]->datacount);
    }
    for (uint64_t i = 0; i < WRECS.entityCount; i++)
    {
        WREntity *cEnt = WRECS.entities[i];
        if (WRECS.activeScene->entities[cEnt->entityID] == 1)
        {
            for (uint64_t i = 0; i < cEnt->entitySysCount; i++)
            {
                cEnt->entitySystems[i](cEnt->entityID);
            }
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

void spriteInit(WREComponent *self, uint64_t entityID);
void spriteDestroy(WREComponent *self, uint64_t entityID);
void initDefaultComponents()
{
    WREComponent sprite = createComponent(spriteInit, spriteDestroy);
    registerComponent(&sprite);
}

void launchEngine(WREngine *engine)
{
    GraphBuilder gb = {0};
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
    engine->Renderer.rg = &gb; // <- we should prolly think more critically about this object's lifetime when we do multithreading?
    initializePipelines(engine);

    while (!glfwWindowShouldClose(engine->Renderer.vkCore.window))
    {
        glfwPollEvents();
        runEntitySystems();
    }
}

void destroyEngine(WREngine *engine)
{
    for (uint64_t i = 0; i < WRECS.entityCount; i++)
    {
        WREntity *entity = WRECS.entities[i];
        if (entity->active)
        {
            destroyEntity(i);
        }
    }
    destroyRenderer(&engine->Renderer);
    glfwDestroyWindow(engine->Renderer.vkCore.window);
    glfwTerminate();
}

typedef struct
{
    uint16_t textureID;
    VkDeviceAddress vertices;
} Sprite;

void spriteInit(WREComponent *self, uint64_t entityID)
{
    spriteComponent *data = self->entityData[entityID];
    renderer_t renderer = *data->renderer;
    BufferCreateInfo bCI = {
        sizeof(transform2D) * data->instanceCount,
        BUFFER_USAGE_UNIFORM_BUFFER | BUFFER_USAGE_TRANSFER_DST,
        DEVICE_ONLY,
    };
    Buffer vBuf = {0};
    createBuffer(renderer.vkCore, bCI, &vBuf);
    bCI.usage = BUFFER_USAGE_STORAGE_BUFFER;
    bCI.access = DEVICE_ONLY;
    createBuffer(renderer.vkCore, bCI, &vBuf);
    pushDataToBuffer(data->transforms, sizeof(transform2D) * data->instanceCount, vBuf, 0);

    int texWidth, texHeight, texChannels;
    stbi_uc *img = stbi_load(data->imagePath, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    Texture tex = createTexture(renderer.vkCore, texWidth, texHeight);
    BufferCreateInfo tci = {
        texWidth * texHeight * 4,
        BUFFER_USAGE_TRANSFER_SRC,
        CPU_ONLY,
    };
    Buffer buf;
    createBuffer(renderer.vkCore, tci, &buf);
    pushDataToBuffer(img, texWidth * texHeight * 4, buf, 0);
    copyDataToTextureImage(renderer.vkCore, &tex.img, &buf, texWidth, texHeight);
    destroyBuffer(buf, renderer.vkCore);
    stbi_image_free(img);
    write_textureDescriptorSet(renderer.vkCore, tex.img.imgview, renderer.vkCore.linearSampler, 0);

    Sprite sprite = {
        tex.index,
        vBuf.gpuAddress,
    };
    Mesh mesh = createMesh(*data->renderer, 1, &sprite, 6, (uint32_t[6]){0, 1, 2, 2, 3, 0}, data->instanceCount, sizeof(sprite));
    submitMesh(mesh, &spriteHandler);
}

void spriteRender(uint64_t *componentIDs, uint64_t compCount, void **data, uint64_t dataCount)
{
    const char *passName = "SpritePass";
    WREngine *engine = data[0];
    removePass(engine->Renderer.rg, passName);
    RenderPass pass = sceneDraw(&engine->Renderer, &spriteHandler);
    pass.gPl = engine->spritePipeline;

    // update

    WREComponent *comp = getComponent(componentIDs[0]);
    for (uint64_t j = 0; j < WRECS.entityCount; j++)
    {
        spriteComponent *compDat = (spriteComponent *)comp->entityData[j];
        if (compDat == NULL)
            continue;
        VkMappedMemoryRange memRange = {
            VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
            NULL,
            compDat->data.associatedMemory,
            0,
            VK_WHOLE_SIZE,
        };
        vkFlushMappedMemoryRanges(engine->Renderer.vkCore.lDev, 1, &memRange); // TODO: we can batch these, but we need to figure out how to batch pushing data as well
        pushDataToBuffer(compDat->transforms, sizeof(transform2D) * compDat->instanceCount, compDat->data, 0);
    }
}
