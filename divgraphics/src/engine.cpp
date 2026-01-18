#include "engine.hpp"

extern GraphLib gGraphLib;
Scene gScene;


void InitScene()
{
    int screeWidth = GetScreenWidth();
    int screeHeight = GetScreenHeight();
    gGraphLib.create();
    gScene.width = screeWidth;
    gScene.height = screeHeight;
    for (int i = 0; i < MAX_LAYERS; i++)
    {
        gScene.layers[i].back = -1;
        gScene.layers[i].front = -1;

        gScene.layers[i].mode = LAYER_MODE_TILEX | LAYER_MODE_TILEY;

        gScene.layers[i].size.width = screeWidth;
        gScene.layers[i].size.height = screeHeight;
        gScene.layers[i].size.x = 0;
        gScene.layers[i].size.y = 0;

        gScene.layers[i].scroll_x = 0;
        gScene.layers[i].scroll_y = 0;
        gScene.layers[i].scroll_factor_x = 1;
        gScene.layers[i].scroll_factor_y = 1;
    }
}

void DestroyScene()
{
    for (int i = 0; i < MAX_LAYERS; i++)
    {
        gScene.layers[i].destroy();
    }
    gGraphLib.destroy();
}

void RenderScene()
{
  //rlDisableBackfaceCulling();
    for (int i = 0; i < MAX_LAYERS; i++)
    {
        gScene.layers[i].render();
    }

    for (size_t i = 0; i < gScene.nodesToRemove.size(); i++)
    {
        delete gScene.nodesToRemove[i];
    }
    gScene.nodesToRemove.clear();



// //   //  rlEnableBackfaceCulling();
//     DrawRectangle(10, 10, 300, 200, ColorAlpha(BLACK, 0.7f));
//    // DrawText("F1: Toggle Quadtree | F2: Toggle Bounds", 10, 10, 16, WHITE);
//     DrawFPS(20, 20);
//     DrawText(TextFormat("Collision Statics: %d Dynamic: %d", gScene.staticEntities.size(), gScene.dynamicEntities.size()), 20,40, 15, WHITE);

    // Debug: desenha quadtree (F1 para toggle)
    static bool showQuadtree = false;
    if (IsKeyPressed(KEY_F1))
        showQuadtree = !showQuadtree;

    if (showQuadtree && gScene.staticTree)
    {
        gScene.staticTree->draw();
    }

    // Debug: desenha bounds das entities (F2 para toggle)
    static bool showBounds = false;
    if (IsKeyPressed(KEY_F2))
        showBounds = !showBounds;

    if (showBounds)
    {
        for (int i = 0; i < MAX_LAYERS; i++)
        {
            Layer &l = gScene.layers[i];
            for (size_t j = 0; j < l.nodes.size(); j++)
            {
                Entity *e = l.nodes[j];
                if (e->shape)
                {
                    Rectangle b = e->getBounds();
                    Color c = (e->flags & B_STATIC) ? YELLOW : RED;

                        int screenX = b.x;
                        int screenY = b.y;
                        screenX -= l.scroll_x;
                        screenY -= l.scroll_y;

                     DrawRectangleLines(screenX, screenY,
                                        (int)b.width, (int)b.height, c);
                }
            }
        }
    }

    static bool showShapes = false;
    if (IsKeyPressed(KEY_F3))
        showShapes = !showShapes;

    if (showShapes)
    {
        for (int i = 0; i < MAX_LAYERS; i++)
        {
            for (Entity *e : gScene.layers[i].nodes)
            {
                if (e->shape && (e->flags & B_COLLISION))
                {
                    Color col = (e->flags & B_STATIC) ? YELLOW : GREEN;
                    

                    e->shape->draw(e,col);
                }
            }
        }
    }


}

void InitCollision(int x, int y, int width, int height, CollisionCallback onCollision)
{
    Rectangle r;
    r.x = x;
    r.y = y;
    r.width = width;
    r.height = height;
    gScene.initCollision(r);
    gScene.setCollisionCallback(onCollision);
    UpdateCollision();
}

void UpdateCollision()
{
    gScene.updateCollision();
}


Entity *CreateEntity(int graphId, int layer, double x, double y)
{
    return gScene.addEntity(graphId, layer, x, y);
}

void SetLayerMode(int layer, uint8 mode)
{
    if (layer < 0 || layer > MAX_LAYERS)
        layer = 0;

    gScene.layers[layer].mode = mode;
}

void SetLayerScrollFactor(int layer, double x, double y)
{
    if (layer < 0 || layer > MAX_LAYERS)
        layer = 0;

    gScene.layers[layer].scroll_factor_x = x;
    gScene.layers[layer].scroll_factor_y = y;
}

void SetLayerSize(int layer, int x, int y, int width, int height)
{

    if (layer > MAX_LAYERS)
        layer = MAX_LAYERS - 1;

    if (layer < 0)
    {
        for (int i = 0; i < MAX_LAYERS; i++)
        {
            gScene.layers[i].size.x = x;
            gScene.layers[i].size.y = y;
            gScene.layers[i].size.width = width;
            gScene.layers[i].size.height = height;
        }
        return;
    }

    gScene.layers[layer].size.x = x;
    gScene.layers[layer].size.y = y;
    gScene.layers[layer].size.width = width;
    gScene.layers[layer].size.height = height;
}

void SetLayerBackGraph(int layer, int graph)
{
    if (layer < 0 || layer > MAX_LAYERS)
        layer = 0;

    gScene.layers[layer].back = graph;
}

void SetLayerFrontGraph(int layer, int graph)
{
    if (layer < 0 || layer > MAX_LAYERS)
        layer = 0;

    gScene.layers[layer].front = graph;
}



void SetScroll(double x, double y)
{
    gScene.scroll_x = x;
    gScene.scroll_y = y;

 
    int screenWidth = gScene.width;
    int screenHeight = gScene.height;
    for (int i = 0; i < MAX_LAYERS; i++)
    {
        Layer &layer = gScene.layers[i];

      
        layer.scroll_x = x ;
        layer.scroll_y = y ;

        // Clamp aos limites do mundo (se layer tem bounds definidos)
        if (layer.size.width > 0 && layer.size.height > 0)
        {
            // Limite esquerdo/superior
            if (layer.scroll_x < layer.size.x)
                layer.scroll_x = layer.size.x;
            if (layer.scroll_y < layer.size.y)
                layer.scroll_y = layer.size.y;

            // Limite direito/inferior (world_width - screen_width)
            double max_scroll_x = layer.size.x + layer.size.width - screenWidth;
            double max_scroll_y = layer.size.y + layer.size.height - screenHeight;

            if (layer.scroll_x > max_scroll_x)
                layer.scroll_x = max_scroll_x;
            if (layer.scroll_y > max_scroll_y)
                layer.scroll_y = max_scroll_y;
        }
    }
}

