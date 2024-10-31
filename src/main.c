#include <engine.h>
WREngine engine = {0};
WREScene scene = {0};
transform2D trans = {
    .origin = {0, 0, 0},
    .scale = {1, 1},
    .rotation = 0,
};
void startTask(WREngine *engine)
{
    spriteComponent sprite = {
        &trans,
        1,
        "assets/birb.png",
        &engine->Renderer,
        {0},
        0,
    };
    setActiveScene(&scene);

    WREntity entity = createEntity();

    registerEntity(&entity, &scene);

    addComponent(&entity, &spriteComp, &sprite, sizeof(sprite));
}

int main(void)
{
    scene = createScene();
    addStartupTask(&engine, startTask);
    launchEngine(&engine);
    destroyEngine(&engine);
}