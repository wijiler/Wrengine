#include <engine.h>
WREngine engine = {0};
WREScene scene = {0};
transform2D trans = {0};
spriteComponent sprite = {
    &trans,
    1,
    "assets/birb.png",
    &engine.Renderer,
    {0},
    0,
};
void startTask(WREngine *engine)
{
    setActiveScene(&scene);
    trans = (transform2D){
        (Vector3){0, 0, 0},
        (Vector2){1, 1},
        0,
    };
    WREntity entity = createEntity();

    registerEntity(&entity, &scene);

    addComponent(&entity, &spriteComp, &sprite);
}

int main(void)
{
    scene = createScene();
    addStartupTask(&engine, startTask);
    launchEngine(&engine);
    destroyEngine(&engine);
}