#ifndef WRENGINE_H__
#define WRENGINE_H__

#include <ecs.h>
#include <renderer.h>
#include <stdbool.h>

typedef struct WREngine WREngine;
typedef void (*startupTask)(WREngine *engine);

struct WREngine
{
    graphicsPipeline spritePipeline;
    graphicsPipeline uiPipeline;
    graphicsPipeline PBRPipeline;
    graphicsPipeline customizablePipeline;
    renderer_t Renderer;

    uint64_t startupTaskCount;
    startupTask *startupTasks;
};
// --
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
    float x, y, z, w;
} Vector4;

typedef Vector4 Quat;

typedef struct
{
    Vector3 origin;
    Vector2 scale;
    float rotation;
} transform2D;

typedef struct
{
    Vector3 origin;
    Vector3 scale;
    Quat rotation;
} transform3D;

typedef struct
{
    transform2D *transforms;
    int instanceCount;
    char *imagePath;

    renderer_t *renderer;
    Buffer data;
    VkDeviceAddress meshAddr;
} spriteComponent;

typedef struct
{
    char *imagePath;
} simpleMaterial;

typedef struct
{
    int instanceCount;
    int vertCount;
    int indexCount;
    uint32_t *indices;        // per mesh
    transform3D *transforms;  // per instance
    Vector3 *vertexPositions; // per mesh
    Vector3 *colors;          // per mesh
    Vector2 *uvs;             // per mesh

    renderer_t *renderer;
} simpleMeshComponent;
// --
extern WREComponent spriteComp;
extern WREComponent meshComp;

void addStartupTask(WREngine *engine, startupTask task);

void launchEngine(WREngine *engine);
void destroyEngine(WREngine *engine);

#endif