#include <ecs.h>

#include <stdlib.h>
#include <string.h>

#define INCREMENTAMOUNT 100

void addComponent(WREntity *entity, WREComponent *comp, void *constructionData)
{
    entity->components[entity->compCount] = comp->compID;
    entity->compCount += 1;

    if (comp->entityCount % INCREMENTAMOUNT == 0)
    {
        comp->entityIDS = realloc(comp->entityIDS, sizeof(uint64_t) * (comp->entityCount + INCREMENTAMOUNT));
        comp->entityData = realloc(comp->entityData, sizeof(void *) * (comp->entityCount + INCREMENTAMOUNT));
    }
    comp->entityIDS[comp->entityCount] = entity->entityID;
    comp->entityData[comp->entityCount] = constructionData;
    comp->entityCount += 1;
}

void registerComponent(WREComponent *component, WREScene *scene)
{
    if (scene->componentCount % INCREMENTAMOUNT == 0)
    {
        scene->components = realloc(scene->components, sizeof(WREComponent) * (scene->componentCount + INCREMENTAMOUNT));
    }
    component->compID = scene->componentCount;
    scene->components[scene->componentCount] = malloc(sizeof(WREComponent));
    memcpy(scene->components[scene->componentCount], component, sizeof(WREComponent));
    scene->componentCount += 1;
}

void registerEntity(WREntity *entity, WREScene *scene)
{
    entity->active = true;
    if (scene->entityCount % INCREMENTAMOUNT == 0)
    {
        scene->entities = realloc(scene->entities, sizeof(WREntity) * (scene->entityCount + INCREMENTAMOUNT));
    }
    for (uint64_t i = 0; i < scene->entityCount; i++)
    {
        if (!scene->entities[i]->active)
        {
            entity->entityID = i;
            scene->entities[i] = malloc(sizeof(WREntity));
            memcpy(scene->entities[i], entity, sizeof(WREntity));
            scene->entityCount += 1;
            return;
        }
    }
    scene->entities[scene->entityCount] = malloc(sizeof(WREntity));
    memcpy(scene->entities[scene->entityCount], entity, sizeof(WREntity));
    scene->entityCount += 1;
}

void registerSystem(WRESystem *system)
{
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

WREComponent *getComponent(uint64_t compID, WREScene scene)
{
    return scene.components[compID];
}

WREntity *getEntity(uint64_t entityID, WREScene scene)
{
    return scene.entities[entityID];
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

int64_t getEntityIndex(WREComponent comp, uint64_t entityID)
{
    for (uint64_t i = 0; i < comp.entityCount; i++)
    {
        if (comp.entityIDS[i] == entityID)
        {
            return i;
        }
    }
    return -1;
}

void destroyEntity(uint64_t entityID, WREScene *scene)
{
    WREntity *entity = getEntity(entityID, *scene);
    entity->active = false;
    for (uint64_t i = 0; i < entity->compCount; i++)
    {
        WREComponent *currentComponent = getComponent(entity->components[i], *scene);
        int64_t index = getEntityIndex(*currentComponent, entityID);
        currentComponent->destructor(currentComponent, index);
    }
    entity->compCount = 0;
    scene->entityCount -= 1;
}

void registerDefaultComponents(WREScene *scene)
{
}

WREScene createScene()
{
    WREScene scene = {0};
    registerDefaultComponents(&scene);
    return scene;
}

void destroyScene(WREScene *scene)
{
    for (uint64_t i = 0; i < scene->entityCount; i++)
    {
        destroyEntity(scene->entities[i]->entityID, scene);
    }
    scene->constructed = false;
}

void setActiveScene(WREScene *scene)
{
    WRECS.activeScene = scene;
    if (!scene->constructed)
    {
        for (uint64_t i = 0; i < scene->componentCount; i++)
        {
            scene->components[i]->initializer(WRECS.activeScene->components[i]);
        }
    }
    scene->constructed = true;
    for (uint64_t i = 0; i < WRECS.activeScene->componentCount; i++)
    {
        for (uint64_t s = 0; s < WRECS.systemCount; s++)
        {
            for (uint64_t t = 0; t < WRECS.systems[s]->touchCount; t++)
            {
                WRECS.systems[s]->active = true;
                break;
            }
        }
    }
}