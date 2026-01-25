#include "engine.hpp"
#include "render.hpp"

GraphLib gGraphLib;
extern Scene gScene;

Entity *Scene::addEntity(int graphId, int layer, double x, double y)
{
    Entity *node = new Entity();
    Graph *g = gGraphLib.getGraph(graphId);
    node->graph = graphId;
    node->layer = layer;
    node->x = x;
    node->y = y;
    if (layer < 0 || layer > MAX_LAYERS)
        layer = 0;
    node->id = layers[layer].nodes.size();
    layers[layer].nodes.push_back(node);
    return node;
}

void Scene::moveEntityToLayer(Entity *node, int layer)
{
    if (!node)
        return;

    if (layer < 0 || layer > 5)
        layer = 0;
    if (node->layer == layer)
        return;

    int srcLayer = node->layer;
    uint32 idx = node->id;

    // node a mover (pode ser o próprio "node")
    Entity *remove = layers[srcLayer].nodes[idx];

    // swap-remove NO LAYER DE ORIGEM
    Entity *back = layers[srcLayer].nodes.back();
    layers[srcLayer].nodes[idx] = back;
    back->id = idx;
    layers[srcLayer].nodes.pop_back();

    // adicionar ao destino
    remove->layer = (uint8)layer;
    remove->id = (uint32)layers[layer].nodes.size();
    layers[layer].nodes.push_back(remove);
}

void Scene::removeEntity(Entity *node)
{
    if (!node)
        return;

    Layer &layer = layers[node->layer];
    uint32 idx = node->id;

    if (idx >= layer.nodes.size())
        return;

    Entity *last = layer.nodes.back();

    // swap-remove
    layer.nodes[idx] = last;
    last->id = idx;
    layer.nodes.pop_back();

    // marca para destruir mais tarde
    nodesToRemove.push_back(node);
}

void Scene::destroy()
{
    if (staticTree)
    {
        delete staticTree;
        staticTree = nullptr;
    }
    for (int i = 0; i < MAX_LAYERS; i++)
        layers[i].destroy();
}

void Scene::setCollisionCallback(CollisionCallback callback, void *userdata)
{

    onCollision = callback;
    collisionUserData = userdata;
}
Scene::Scene()
{

    width = GetScreenWidth();
    height = GetScreenHeight();

    staticTree = nullptr;
    for (int i = 0; i < MAX_LAYERS; i++)
    {
        layers[i].back = -1;
        layers[i].front = -1;
        layers[i].tilemap=nullptr;

        layers[i].mode = LAYER_MODE_TILEX | LAYER_MODE_TILEY;

        layers[i].size.width = width;
        layers[i].size.height = height;
        layers[i].size.x = 0;
        layers[i].size.y = 0;

        layers[i].scroll_x = 0;
        layers[i].scroll_y = 0;
        layers[i].scroll_factor_x = 1;
        layers[i].scroll_factor_y = 1;
    }
}

Scene::~Scene()
{
    destroy();
}

void Layer::destroy()
{
    for (int i = 0; i < nodes.size(); i++)
    {
        delete nodes[i];
    }
    nodes.clear();

    if (tilemap)
    {
        delete tilemap;
        tilemap = nullptr;
    }
}

void Layer::render_parelax(Graph *g)
{
    if (!g)
        return;

    Texture2D bg_texture = gGraphLib.textures[g->texture];

    float screen_w = gScene.width;
    float screen_h = gScene.height;
    float tex_w = (float)bg_texture.width;
    float tex_h = (float)bg_texture.height;

    // Aplicar parallax
    float effective_scroll_x = (float)scroll_x * scroll_factor_x;
    float effective_scroll_y = (float)scroll_y * scroll_factor_y;

    //   printf("w %d %d\n",gScene.width,gScene.height);

    float uv_x1 = 0.0f, uv_y1 = 0.0f;
    float uv_x2 = 1.0f, uv_y2 = 1.0f;


//     // --- WRAP AUTOMÁTICO (repeat) ---
// if (mode & LAYER_MODE_TILEX)
// {
//     float span = uv_x2 - uv_x1;

//     // Garante pelo menos 1 repetição horizontal
//     if (span < 1.0f)
//         uv_x2 = uv_x1 + 1.0f;
// }

// if (mode & LAYER_MODE_TILEY)
// {
//     float span = uv_y2 - uv_y1;

//     // Garante pelo menos 1 repetição vertical
//     if (span < 1.0f)
//         uv_y2 = uv_y1 + 1.0f;
// }


    if (mode & LAYER_MODE_TILEX)
    {
        // Normaliza scroll para UV space
        uv_x1 = fmod(effective_scroll_x / tex_w, 1.0f);
        if (uv_x1 < 0)
            uv_x1 += 1.0f; // Handle negative scroll
        uv_x2 = uv_x1 + (screen_w / tex_w);
    }

    if (mode & LAYER_MODE_TILEY)
    {
        uv_y1 = fmod(effective_scroll_y / tex_h, 1.0f);
        if (uv_y1 < 0)
            uv_y1 += 1.0f;
        uv_y2 = uv_y1 + (screen_h / tex_h);
    }

    if (mode & LAYER_MODE_STRETCHX)
    {
        // Stretch mantém proporção 0-1 mas offset por scroll
        uv_x1 = effective_scroll_x / size.width;
        uv_x2 = (effective_scroll_x + screen_w) / size.width;
    }

    if (mode & LAYER_MODE_STRETCHY)
    {
        uv_y1 = effective_scroll_y / size.height;
        uv_y2 = (effective_scroll_y + screen_h) / size.height;
    }

    // Flips
    if (mode & LAYER_MODE_FLIPX)
        std::swap(uv_x1, uv_x2);

    if (mode & LAYER_MODE_FLIPY)
        std::swap(uv_y1, uv_y2);

    Rectangle src = {
        uv_x1 * tex_w,
        uv_y1 * tex_h,
        (uv_x2 - uv_x1) * tex_w,
        (uv_y2 - uv_y1) * tex_h};

    Rectangle dst = {
        0,
        0,
        screen_w,
        screen_h};

    // Desenhar
    DrawTexturePro(
        bg_texture,
        src,
        dst,
        {0, 0},
        0.0f,
        WHITE);

    // RenderQuadUV(0, 0, screen_w, screen_h,
    //              uv_x1, uv_y1, uv_x2, uv_y2, bg_texture);

    // DrawTexture(bg_texture, 0, 0, WHITE);
}

void Layer::render()
{

    if (back != -1)
    {

        Graph *g = gGraphLib.getGraph(back);
        render_parelax(g);
        // DrawRectangleLines(size.x, size.y, size.width, size.height, WHITE);
    }


    if(tilemap)
    {
        tilemap->render();
    }

    for (int i = 0; i < nodes.size(); i++)
    {
        Entity *e = nodes[i];
        if ((e->flags & B_VISIBLE) && !(e->flags & B_DEAD))
        {
            e->render();
        }
    }

    if (front != -1)
    {
        Graph *g = gGraphLib.getGraph(front);
        render_parelax(g);
    }

    // DrawRectangleLines(size.x, size.y, size.width , size.height  , WHITE);
}