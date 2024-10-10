#ifndef WRENGINE_H__
#define WRENGINE_H__

#include <ecs.h>
#include <renderer.h>
#include <stdbool.h>

typedef struct
{
    float position[3];
    float color[3];
    float uv[2];
} Vertex;

typedef struct
{
    graphicsPipeline spritePipeline;
    graphicsPipeline uiPipeline;
    graphicsPipeline PBRPipeline;
    graphicsPipeline customizablePipeline;
    renderer_t Renderer;
} WREngine;

typedef struct
{
    uint64_t meshIndex;
    uint64_t textureID;
    uint64_t indexCount;
    uint64_t vertexCount;
    uint64_t instanceCount;
    renderer_t *Renderer;
    Vertex *vertices;
    uint32_t *indices;
} MeshComponent;

void launchEngine(WREngine *engine);
void destroyEngine(WREngine *engine);

#endif