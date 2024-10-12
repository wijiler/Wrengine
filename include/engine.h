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
    float dimensions[2];
    float position[2];
    float zLayer;
    char *imagePath;

    renderer_t *renderer;
} spriteComponent;

void launchEngine(WREngine *engine);
void destroyEngine(WREngine *engine);

#endif