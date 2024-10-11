#ifndef WRE_ECS_H__
#define WRE_ECS_H__
#define MAXCOMPONENTS 100
#include <stdbool.h>
#include <stdint.h>

typedef struct WREComponent WREComponent;
typedef void (*ComponentInitFunction)(WREComponent *self);
typedef void (*ComponentDestroyFunction)(WREComponent *self, uint64_t entityID);

struct WREComponent
{
    uint64_t compID;
    ComponentInitFunction initializer;
    ComponentDestroyFunction destructor;
    uint64_t entityCount;
    uint64_t *entityIDS;
    void **entityData;
};

typedef struct
{
    uint64_t entityID;
    uint64_t compCount;
    int64_t components[MAXCOMPONENTS];
    bool active;
} WREntity;

typedef void (*WRESystemfunction)(uint64_t *componentIDs, uint64_t compCount, void *data, uint64_t dataCount);

typedef struct
{
    uint64_t systemID;
    uint64_t touchCount;
    uint64_t datacount;
    WRESystemfunction function;
    bool active;
    void *data;
    uint64_t *componentIDs;
} WRESystem;

typedef struct
{
    uint64_t entityCount;
    WREntity **entities;
    uint64_t componentCount;
    WREComponent **components;
    uint64_t systemCount;
    WRESystem **systems;
} systemManager;

static systemManager WRECS = {0};
void addComponent(WREntity *entity, WREComponent *comp, void *constructionData);
void registerComponent(WREComponent *component);
void registerEntity(WREntity *entity);
void registerSystem(WRESystem *system);
WREComponent *getComponent(uint64_t compID);
WREntity *getEntity(uint64_t entityID);
WRESystem *getSystem(uint64_t SystemID);
#endif