#ifndef WRE_ECS_H__
#define WRE_ECS_H__
#include <stdbool.h>
#include <stdint.h>

typedef struct WREComponent WREComponent;
typedef void (*ComponentFunction)(WREComponent *self, uint64_t entityID);

typedef enum
{
    USAGE_INACTIVE,
    USAGE_REMOVED,
    USAGE_ACTIVE,
} WREUsage;

struct WREComponent
{
    uint64_t compID;
    ComponentFunction initializer;
    ComponentFunction destructor;
    void **entityData;
};

typedef struct
{
    uint64_t entityID;
    uint8_t *components;
    WREUsage active;
} WREntity;

typedef void (*WRESystemfunction)(uint64_t *componentIDs, uint64_t compCount, void *data);

typedef struct
{
    uint64_t systemID;
    uint64_t touchCount;
    WRESystemfunction function;
    WREUsage active;
    void *data;
    uint64_t *componentIDs;
} WRESystem;

typedef struct
{
    uint8_t *entities; // bitmask
} WREScene;

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
extern systemManager WRECS;

void addComponent(WREntity *entity, WREComponent *comp, void *constructionData, size_t constructionDataSize);
void registerComponent(WREComponent *component);
void registerEntity(WREntity *entity, WREScene *scene);
void registerSystem(WRESystem *system);
WREComponent *getComponent(uint64_t compID);
WREntity *getEntity(uint64_t entityID);
WRESystem *getSystem(uint64_t SystemID);

WREScene createScene();
WREComponent createComponent(ComponentFunction init, ComponentFunction destroy);
WRESystem createSystem(uint32_t componentCount, uint64_t components[], void *data);
WREntity createEntity();
void setActiveScene(WREScene *scene);

void destroyEntity(uint64_t entityID);
void removeSystem(uint64_t systemID);
#endif