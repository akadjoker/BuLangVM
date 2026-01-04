
#include "test.hpp"




void handleCollision(Entity *a, Entity *b, void *userdata)
{
    if (!a || !b)
        return;
    printf("Collision between %d and %d\n", a->graph, b->graph);

    if (a->graph == 1)
    {
        a->kill();
    }
    else if (b->graph == 1)
    {
        b->kill();
    }
}

 inline Vector2 Slide(Vector2 v, Vector2 n)
{
    float dot = v.x * n.x + v.y * n.y;
    return {
        v.x - n.x * dot,
        v.y - n.y * dot
    };
}

 inline Vector2 Reflect(Vector2 v, Vector2 n)
{
    float dot = v.x * n.x + v.y * n.y;
    return {
        v.x - n.x * dot * 2,
        v.y - n.y * dot * 2
    };
}

int main()
{
    //teste_tileset();
    //teste_tileset_iso();
  //  teste_tileset_hexa();
  //teste_tile_iso();
  //teste_tileset_horto();
  teste_particles();
    return 0;
}
