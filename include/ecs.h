#ifndef WRE_ECS_H__
#define WRE_ECS_H__
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

typedef void (*WREntitySystemfunction)(uint64_t entityID);
typedef struct
{
    uint64_t entityID;
    uint64_t entitySysCount;
    uint8_t *components;
    WREntitySystemfunction *entitySystems;
    bool active;
} WREntity;

typedef void (*WRESystemfunction)(uint64_t *componentIDs, uint64_t compCount, void **data, uint64_t dataCount);

typedef struct
{
    uint64_t systemID;
    uint64_t touchCount;
    uint64_t datacount;
    WRESystemfunction function;
    bool active;
    void **data;
    uint64_t *componentIDs;
} WRESystem;

typedef struct
{
    uint8_t *entities; // bitmask
} WREScene;

void addComponent(WREntity *entity, WREComponent *comp, void *constructionData);
void addEntitySystem(WREntity *entity, WREntitySystemfunction function);
void registerComponent(WREComponent *component);
void registerEntity(WREntity *entity, WREScene *scene);
void registerSystem(WRESystem *system);
WREComponent *getComponent(uint64_t compID);
WREntity *getEntity(uint64_t entityID);
WRESystem *getSystem(uint64_t SystemID);

WREScene createScene();
WREComponent createComponent(ComponentFunction init, ComponentFunction destroy);
WREntity createEntity();
void setActiveScene(WREScene *scene);

void destroyEntity(uint64_t entityID);
void removeSystem(uint64_t systemID);
#endif