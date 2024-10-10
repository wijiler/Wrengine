#ifndef WRE_ECS_H__
#define WRE_ECS_H__
#define MAXCOMPONENTS 100
#include <stdbool.h>
#include <stdint.h>

typedef struct WREComponent WREComponent;
typedef void (*ComponentFunction)(WREComponent *self, void *data);

struct WREComponent
{
    uint64_t compID;
    ComponentFunction initializer;
    ComponentFunction destructor;
};

typedef struct
{
    uint64_t entityID;
    uint64_t compCount;
    int64_t components[MAXCOMPONENTS];
    void *compData[MAXCOMPONENTS];
    bool active;
} WREntity;

typedef void (*WRESystemfunction)(uint64_t *entityIDs, uint64_t entityCount, void *data, uint64_t dataCount);

typedef struct
{
    uint64_t systemID;
    uint64_t touchCount;
    uint64_t *entityIDs;
    uint64_t datacount;
    WRESystemfunction function;
    void *data;
    bool active;
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
void addComponent(WREntity *entity, WREComponent comp, void *constructionData);
void registerComponent(WREComponent *component);
void registerEntity(WREntity *entity);
void registerSystem(WRESystem *system);
WREComponent *getComponent(uint64_t compID);
WREntity *getEntity(uint64_t entityID);
WRESystem *getSystem(uint64_t SystemID);
#endif