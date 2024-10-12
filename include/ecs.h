#ifndef WRE_ECS_H__
#define WRE_ECS_H__
#define MAXCOMPONENTS 100
#include <stdbool.h>
#include <stdint.h>

typedef struct WREComponent WREComponent;
typedef void (*ComponentFunction)(WREComponent *self, uint64_t entityID);

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
} WREScene;

typedef struct
{
    uint64_t componentCount;
    WREComponent **components;
    uint64_t systemCount;
    WRESystem **systems;
    WREScene *activeScene;
} systemManager;

void addComponent(WREntity *entity, WREComponent *comp, void *constructionData);
void registerComponent(WREComponent *component);
void registerEntity(WREntity *entity, WREScene *scene);
void registerSystem(WRESystem *system);
WREComponent *getComponent(uint64_t compID);
WREntity *getEntity(uint64_t entityID, WREScene scene);
WRESystem *getSystem(uint64_t SystemID);

WREScene createScene();
void destroyScene(WREScene *scene);
void setActiveScene(WREScene *scene);

void destroyScene(WREScene *scene);
void destroyEntity(uint64_t entityID, WREScene *scene);
void removeSystem(uint64_t systemID);
#endif