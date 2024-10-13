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
    WREScene scene = createScene();
    setActiveScene(&scene);

    WREComponent component = {
        0,
        thing,
        thing2,
        0,
    };
    registerComponent(&component);
    WRESystem sys = {
        0,
        1,
        0,
        thing3,
        true,
        NULL,
        (uint64_t[1]){component.compID},
    };
    registerSystem(&sys);
    WREntity entity = {
        0,
        0,
        NULL,
        NULL,
        false,
    };
    addComponent(&entity, &component, NULL);
    registerEntity(&entity, &scene);
    WREngine engine = {0};
    launchEngine(&engine);
    destroyEngine(&engine);
}