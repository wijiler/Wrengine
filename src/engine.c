#include <engine.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <stdio.h>

// ---GLOBALS
MeshHandler spriteHandler = {0};
MeshHandler meshHandler = {0};
WREComponent spriteComp = {0};
WREComponent meshComp = {0};

void runEntitySystems()
{
    for (uint64_t i = 0; i < WRECS.systemCount; i++)
    {
        WRECS.systems[i]->function(
            WRECS.systems[i]->componentIDs,
            WRECS.systems[i]->touchCount,
            WRECS.systems[i]->data);
    }
}

static void error_callback(int error, const char *description)
{
    fprintf(stderr, "GLFW ERROR:%i, %s\n", error, description);
}

void initializePipelines(WREngine *engine)
{
    uint64_t SpriteLen = 0;
    uint32_t *SpriteShader = NULL;

    readShaderSPRV("./shaders/sprite.spv", &SpriteLen, &SpriteShader);
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
    setShaderSLSPRV(engine->Renderer.vkCore, &engine->spritePipeline, SpriteShader, SpriteLen);
    //
    uint64_t PBRLen = 0;
    uint32_t *PBRShader = NULL;
    readShaderSPRV("./shaders/pbr.spv", &PBRLen, &PBRShader);
    engine->PBRPipeline.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    engine->PBRPipeline.colorBlending = VK_TRUE;
    engine->PBRPipeline.logicOpEnable = VK_FALSE;
    engine->PBRPipeline.reasterizerDiscardEnable = VK_FALSE;
    engine->PBRPipeline.polyMode = VK_POLYGON_MODE_FILL;
    engine->PBRPipeline.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    engine->PBRPipeline.primitiveRestartEnable = VK_FALSE;
    engine->PBRPipeline.depthBiasEnable = VK_FALSE;
    engine->PBRPipeline.depthTestEnable = VK_FALSE;
    engine->PBRPipeline.depthClampEnable = VK_FALSE;
    engine->PBRPipeline.depthClipEnable = VK_FALSE;
    engine->PBRPipeline.stencilTestEnable = VK_FALSE;
    engine->PBRPipeline.alphaToCoverageEnable = VK_FALSE;
    engine->PBRPipeline.rastSampleCount = VK_SAMPLE_COUNT_1_BIT;
    engine->PBRPipeline.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    engine->PBRPipeline.cullMode = VK_CULL_MODE_NONE;
    engine->PBRPipeline.colorBlendEq = (VkColorBlendEquationEXT){
        VK_BLEND_FACTOR_SRC_ALPHA,
        VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        VK_BLEND_OP_ADD,
        VK_BLEND_FACTOR_ONE,
        VK_BLEND_FACTOR_ZERO,
        VK_BLEND_OP_ADD,
    };
    engine->PBRPipeline.depthBoundsEnable = VK_FALSE;
    engine->PBRPipeline.alphaToOneEnable = VK_TRUE;
    engine->PBRPipeline.sampleMask = UINT32_MAX;
    setPushConstantRange(&engine->PBRPipeline, sizeof(VkDeviceAddress), SHADER_STAGE_VERTEX);
    createPipelineLayout(engine->Renderer.vkCore, &engine->PBRPipeline);
    setShaderSLSPRV(engine->Renderer.vkCore, &engine->PBRPipeline, PBRShader, PBRLen);
}

void spriteInit(WREComponent *self, uint64_t entityID);
void spriteRender(uint64_t *componentIDs, uint64_t compCount, void *data);
void spriteDestroy(WREComponent *self, uint64_t entityID);
void meshInit(WREComponent *self, uint64_t entityID);
void meshRender(uint64_t *componentIDs, uint64_t compCount, void *data);
void meshDestroy(WREComponent *self, uint64_t entityID);
void initDefaultComponents(WREngine *engine)
{
    spriteComp = createComponent(spriteInit, spriteDestroy);
    registerComponent(&spriteComp);
    WRESystem spriteSys = createSystem(1, &spriteComp.compID, engine);
    spriteSys.function = spriteRender;

    meshComp = createComponent(meshInit, meshDestroy);
    registerComponent(&meshComp);
    WRESystem meshSys = createSystem(1, &meshComp.compID, engine);
    meshSys.function = meshRender;

    registerSystem(&spriteSys);
    registerSystem(&meshSys);
}
int index = 0;
int frameindex = 0;
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
    initDefaultComponents(engine);
    for (uint64_t i = 0; i < engine->startupTaskCount; i++)
    {
        engine->startupTasks[i](engine);
    }
    while (!glfwWindowShouldClose(engine->Renderer.vkCore.window))
    {
        frameindex += 1;
        index = frameindex % FRAMECOUNT;
        glfwPollEvents();
        runEntitySystems();
        drawRenderer(&engine->Renderer, index);
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
    for (uint64_t i = 0; i < WRECS.componentCount; i++)
    {
        free(WRECS.components[i]->entityData);
    }
    destroyRenderer(&engine->Renderer);
    glfwDestroyWindow(engine->Renderer.vkCore.window);
    glfwTerminate();
}

void addStartupTask(WREngine *engine, startupTask task)
{
    engine->startupTasks = realloc(engine->startupTasks, sizeof(startupTask) * (engine->startupTaskCount + 1));
    engine->startupTasks[engine->startupTaskCount] = task;
    engine->startupTaskCount += 1;
}

/// ------------
typedef struct
{
    uint64_t textureID;
    VkDeviceAddress vertices;
} Sprite;

void spriteInit(WREComponent *self, uint64_t entityID)
{
    spriteComponent *data = self->entityData[entityID];
    renderer_t *renderer = data->renderer;

    Buffer vBuf = {0};
    {
        BufferCreateInfo bCI = {
            sizeof(transform2D) * data->instanceCount,
            BUFFER_USAGE_STORAGE_BUFFER,
            CPU_ONLY,
        };
        createBuffer(renderer->vkCore, bCI, &vBuf);
        pushDataToBuffer(data->transforms, sizeof(transform2D) * data->instanceCount, vBuf, 0);
    }
    stbi_set_flip_vertically_on_load(true);

    int texWidth, texHeight, texChannels;
    stbi_uc *img = stbi_load(data->imagePath, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    Texture tex = createTexture(renderer->vkCore, texWidth, texHeight);

    {
        BufferCreateInfo tci = {
            texWidth * texHeight * 4,
            BUFFER_USAGE_TRANSFER_SRC,
            CPU_ONLY,
        };

        Buffer buf;

        createBuffer(renderer->vkCore, tci, &buf);
        pushDataToBuffer(img, texWidth * texHeight * 4, buf, 0);
        copyDataToTextureImage(renderer->vkCore, &tex.img, &buf, texWidth, texHeight);

        destroyBuffer(buf, renderer->vkCore);
        stbi_image_free(img);
        stbi_set_flip_vertically_on_load(false);

        submitTexture(renderer, &tex, renderer->vkCore.linearSampler);
    }

    data->data = vBuf;
    Sprite sprite = {
        tex.index,
        vBuf.gpuAddress,
    };
    Mesh mesh = createMesh(*data->renderer, 1, &sprite, 6, (uint32_t[6]){0, 1, 2, 2, 3, 1}, data->instanceCount, sizeof(sprite));
    submitMesh(mesh, &spriteHandler);
    data->meshAddr = mesh.verticies.gpuAddress;
}

VkOffset2D offset = {0, 0};
void spriteRender(uint64_t *componentIDs, uint64_t compCount, void *data)
{
    if (spriteHandler.instancedmeshCount != 0)
    {
        char *passName = "SpritePass";
        WREngine *engine = (WREngine *)data;
        removePass(engine->Renderer.rg, passName);
        RenderPass pass = sceneDraw(&engine->Renderer, &spriteHandler, passName);
        pass.gPl = engine->spritePipeline;
        pass.drawArea = (drawArea){
            &offset,
            &engine->Renderer.vkCore.extent,
        };
        addPass(engine->Renderer.rg, &pass);
        // update

        WREComponent *comp = getComponent(componentIDs[0]);
        for (uint64_t j = 0; j < WRECS.entityCount; j++)
        {
            spriteComponent *compDat = (spriteComponent *)comp->entityData[j];

            if (compDat == NULL)
                continue;

            if (WRECS.entities[j]->active)
            {
                pushDataToBuffer(compDat->transforms, sizeof(transform2D) * compDat->instanceCount, compDat->data, 0);
            }
        }
    }
}
void spriteDestroy(WREComponent *self, uint64_t entityID)
{
    spriteComponent *comp = self->entityData[entityID];
    removeMesh((Mesh){
                   (Buffer){
                       0,
                       0,
                       0,
                       NULL,
                       NULL,
                       NULL,
                       comp->meshAddr,
                   },
                   (Buffer){0},
                   0,
               },
               &spriteHandler, *comp->renderer);
    destroyBuffer(comp->data, comp->renderer->vkCore);
    free(comp);
    WRECS.entities[entityID]->active = USAGE_REMOVED;
}

void meshInit(WREComponent *self, uint64_t entityID)
{
    typedef struct
    {
        Vector3 pos;
        Vector3 color;
        Vector2 uv;
    } Vertex;
    typedef struct
    {
        VkDeviceAddress Instances;
        VkDeviceAddress Vertices;
    } shaderMesh; // we could pack this all into one buffer?
    //        Instanceinfo | VertInfo
    // sizeof(Instanceinfo)  sizeof(VertInfo)
    shaderMesh mesh = {0};

    simpleMeshComponent *data = self->entityData[entityID];
    renderer_t *renderer = data->renderer;

    Vertex *verts = malloc(sizeof(Vertex) * data->vertCount);
    for (uint32_t i = 0; i < data->vertCount; i++)
    {
        verts[i] = (Vertex){
            data->vertexPositions[i],
            data->colors[i],
            data->uvs[i],
        };
    }
    Buffer meshBuffer = {0};
    {
        size_t size = sizeof(Vertex) * data->vertCount;
        Buffer stagingBuf = {0};
        BufferCreateInfo bCI = {
            size,
            BUFFER_USAGE_TRANSFER_SRC,
            CPU_ONLY,
        };
        createBuffer(renderer->vkCore, bCI, &stagingBuf);

        pushDataToBuffer(verts, size, stagingBuf, 0);

        bCI.access = DEVICE_ONLY;
        bCI.usage = BUFFER_USAGE_TRANSFER_DST | BUFFER_USAGE_VERTEX;
        createBuffer(renderer->vkCore, bCI, &meshBuffer);

        copyBuf(renderer->vkCore, stagingBuf, meshBuffer, size, 0, 0);
        destroyBuffer(stagingBuf, renderer->vkCore);

        mesh.Vertices = meshBuffer.gpuAddress;
    }
    Buffer instanceBuffer = {0};
    {
        BufferCreateInfo bCI = {
            sizeof(transform3D) * data->instanceCount,
            BUFFER_USAGE_STORAGE_BUFFER,
            CPU_ONLY,
        };
        createBuffer(renderer->vkCore, bCI, &instanceBuffer);
        pushDataToBuffer(data->transforms, sizeof(transform3D) * data->instanceCount, instanceBuffer, 0);
        mesh.Instances = instanceBuffer.gpuAddress;
    }
    Mesh tmesh = createMesh(*renderer, 1, &mesh, data->indexCount, data->indices, data->indexCount, sizeof(shaderMesh));
    submitMesh(tmesh, &meshHandler);
    data->verts = meshBuffer;
    data->instances = instanceBuffer;
}

void meshRender(uint64_t *componentIDs, uint64_t compCount, void *data)
{
    WREComponent *comp = getComponent(componentIDs[0]);

    if (meshHandler.instancedmeshCount != 0)
    {
        char *passName = "MeshPass";
        WREngine *engine = (WREngine *)data;
        removePass(engine->Renderer.rg, passName);
        RenderPass pass = sceneDraw(&engine->Renderer, &spriteHandler, passName);
        pass.gPl = engine->PBRPipeline;
        pass.drawArea = (drawArea){
            &offset,
            &engine->Renderer.vkCore.extent,
        };
        addPass(engine->Renderer.rg, &pass);
        // update

        for (uint64_t j = 0; j < WRECS.entityCount; j++)
        {
            simpleMeshComponent *compDat = (simpleMeshComponent *)comp->entityData[j];

            if (compDat == NULL)
                continue;

            if (WRECS.entities[j]->active)
            {
                pushDataToBuffer(compDat->transforms, sizeof(transform2D) * compDat->instanceCount, compDat->instances, 0);
            }
        }
    }
}

void meshDestroy(WREComponent *self, uint64_t entityID)
{
}