#include <engine.h>
WREScene scene = {0};

void startTask(WREngine *engine)
{
    setActiveScene(&scene);

    WREntity entity = createEntity();
    registerEntity(&entity, &scene);
    spriteComponent sprite = {
        (transform2D[1]){{(Vector3){0, 0, 0}, (Vector2){1, 1}, 0}},
        1,
        "assets/birb.png",
        &engine->Renderer,
        {0},
        0,
    };
    addComponent(&entity, &spriteComp, &sprite);
}

int main(void)
{
    scene = createScene();
    WREngine engine = {0};
    addStartupTask(&engine, startTask);
    launchEngine(&engine);
    destroyEngine(&engine);
}