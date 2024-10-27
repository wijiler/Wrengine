#include <ecs.h>

#include <stdlib.h>
#include <string.h>

systemManager WRECS = {0};

#define INCREMENTAMOUNT 100

void addComponent(WREntity *entity, WREComponent *comp, void *constructionData, size_t constructionDataSize)
{
    WREntity *entt = getEntity(entity->entityID);
    entt->components = realloc(entt->components, sizeof(uint8_t) * WRECS.componentCount);
    entt->components[comp->compID] = 1;
    entity->components = entt->components;
    WREComponent *rcomp = getComponent(comp->compID);
    rcomp->entityData = realloc(comp->entityData, sizeof(void *) * (WRECS.entityCount));
    rcomp->entityData[entity->entityID] = malloc(constructionDataSize);
    memcpy(rcomp->entityData[entity->entityID], constructionData, constructionDataSize);
    comp->entityData = rcomp->entityData;

    rcomp->initializer(rcomp, entity->entityID);
}

void registerComponent(WREComponent *component)
{
    if (WRECS.componentCount % INCREMENTAMOUNT == 0)
    {
        WRECS.components = realloc(WRECS.components, sizeof(WREComponent) * (WRECS.componentCount + INCREMENTAMOUNT));
    }
    component->compID = WRECS.componentCount;
    WRECS.components[WRECS.componentCount] = malloc(sizeof(WREComponent));
    memcpy(WRECS.components[WRECS.componentCount], component, sizeof(WREComponent));
    WRECS.componentCount += 1;
}

void registerEntity(WREntity *entity, WREScene *scene)
{
    entity->active = true;
    scene->entities = realloc(scene->entities, sizeof(uint8_t) * (WRECS.entityCount + 1));
    scene->entities[entity->entityID] = 1;
    if (WRECS.entityCount % INCREMENTAMOUNT == 0)
    {
        WRECS.entities = realloc(WRECS.entities, sizeof(WREntity) * (WRECS.entityCount + INCREMENTAMOUNT));
    }

    for (uint64_t i = 0; i < WRECS.entityCount; i++)
    {
        if (!WRECS.entities[i]->active)
        {
            entity->entityID = i;
            WRECS.entities[i] = malloc(sizeof(WREntity));
            memcpy(WRECS.entities[i], entity, sizeof(WREntity));
            WRECS.entityCount += 1;
            return;
        }
    }

    entity->entityID = WRECS.entityCount;
    WRECS.entities[WRECS.entityCount] = malloc(sizeof(WREntity));
    memcpy(WRECS.entities[WRECS.entityCount], entity, sizeof(WREntity));
    WRECS.entityCount += 1;
}

void registerSystem(WRESystem *system)
{
    system->active = true;
    if (WRECS.systemCount % INCREMENTAMOUNT == 0)
    {
        WRECS.systems = realloc(WRECS.systems, sizeof(WRESystem) * (WRECS.systemCount + INCREMENTAMOUNT));
    }
    for (uint64_t i = 0; i < WRECS.systemCount; i++)
    {
        if (!WRECS.systems[i]->active)
        {
            system->systemID = i;
            WRECS.systems[i] = malloc(sizeof(WRESystem));
            memcpy(WRECS.systems[i], system, sizeof(WRESystem));
            WRECS.systemCount += 1;
            return;
        }
    }
    system->systemID = WRECS.systemCount;
    WRECS.systems[WRECS.systemCount] = malloc(sizeof(WRESystem));
    memcpy(WRECS.systems[WRECS.systemCount], system, sizeof(WRESystem));
    WRECS.systemCount += 1;
}

WREComponent *getComponent(uint64_t compID)
{
    return WRECS.components[compID];
}

WREntity *getEntity(uint64_t entityID)
{
    return WRECS.entities[entityID];
}

WRESystem *getSystem(uint64_t SystemID)
{
    return WRECS.systems[SystemID];
}
void removeSystem(uint64_t systemID)
{
    WRESystem *sys = getSystem(systemID);
    sys->active = false;
    WRECS.systemCount -= 1;
}

void destroyEntity(uint64_t entityID)
{
    WREntity *entity = getEntity(entityID);
    entity->active = false;
    for (uint64_t i = 0; i < WRECS.componentCount; i++)
    {
        if (entity->components[i] == 1)
        {
            WRECS.components[i]->destructor(WRECS.components[i], entity->entityID);
        }
    }
    WRECS.entityCount -= 1;
}

WREScene createScene()
{
    WREScene scene = {0};
    // logic might go here one day
    return scene;
}

void setActiveScene(WREScene *scene)
{
    WRECS.activeScene = scene;
}
WREComponent createComponent(ComponentFunction init, ComponentFunction destroy)
{
    return (WREComponent){
        0,
        init,
        destroy,
        NULL,
    };
}
WREntity createEntity()
{
    return (WREntity){
        0,
        NULL,
        false,
    };
}

WRESystem createSystem(uint32_t componentCount, uint64_t components[], void *data)
{
    return (WRESystem){
        0,
        componentCount,
        NULL,
        false,
        data,
        components,
    };
}
