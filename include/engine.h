#ifndef WRENGINE_H__
#define WRENGINE_H__

#include <ecs.h>
#include <renderer.h>
#include <stdbool.h>

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
    float x, y;
} Vector2;

typedef struct
{
    float x, y, z;
} Vector3;

typedef struct
{
    Vector3 pos;
    Vector2 scale;
    float rotation;
} transform2D;

typedef struct
{
    transform2D *transforms;
    int instanceCount;
    char *imagePath;

    renderer_t *renderer;
    Buffer data;
    VkDeviceAddress meshAddr;
} spriteComponent;

WREComponent spriteComp;

void launchEngine(WREngine *engine);
void destroyEngine(WREngine *engine);

#endif