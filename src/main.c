#include <engine.h>
#include <stdio.h>

void thing(WREComponent *self, uint64_t id)
{
    printf("hi\n");
}
void thing2(WREComponent *self, uint64_t id)
{
    printf("hi but different\n");
}

void thing3(uint64_t *componentIDs, uint64_t compCount, void **data, uint64_t dataCount)
{
    printf("hi but different2\n");
}

int main(void)
{
    WREngine engine = {0};
    WREScene scene = createScene();
    setActiveScene(&scene);

    WRESystem sys = {
        0,
        1,
        0,
        thing3,
        true,
        &engine,
        &spriteComp.compID,
    };
    registerSystem(&sys);
    WREntity entity = createEntity();
    addComponent(&entity, &spriteComp, NULL);
    registerEntity(&entity, &scene);
    launchEngine(&engine);
    destroyEngine(&engine);
}