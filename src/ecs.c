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
    comp->initializer(comp, entity->entityID);
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

WREComponent *getComponent(uint64_t compID)
{
    return WRECS.components[compID];
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
        WREComponent *currentComponent = getComponent(entity->components[i]);
        int64_t index = getEntityIndex(*currentComponent, entityID);
        currentComponent->destructor(currentComponent, index);
    }
    entity->compCount = 0;
    scene->entityCount -= 1;
}

WREScene createScene()
{
    WREScene scene = {0};
    // logic might go here one day
    return scene;
}

void destroyScene(WREScene *scene)
{
    for (uint64_t i = 0; i < scene->entityCount; i++)
    {
        destroyEntity(scene->entities[i]->entityID, scene);
    }
}

void setActiveScene(WREScene *scene)
{
    WRECS.activeScene = scene;

    uint64_t compCount = 0;
    uint64_t *comps = NULL;
    for (uint64_t i = 0; i < scene->entityCount; i++)
    {
        for (uint64_t c = 0; c < scene->entities[i]->compCount; c++)
        {
            uint64_t ccID = scene->entities[i]->components[c];
            comps = realloc(comps, sizeof(uint64_t) * (compCount + 1));
            comps[compCount] = ccID;
            compCount += 1;
        }
    }
    for (uint64_t i = 0; i < WRECS.systemCount; i++)
    {
        for (uint64_t ci = 0; ci < WRECS.systems[i]->touchCount; ci++)
        {
            for (uint64_t c = 0; c < compCount; c++)
            {
                if (WRECS.systems[i]->componentIDs[ci] == comps[c])
                {
                    WRECS.systems[i]->active = true;
                }
            }
        }
    }
}