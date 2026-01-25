#include "engine.hpp"
#include <cstdio>
#include <algorithm>
#include <cmath>
 

 
extern GraphLib gGraphLib;
extern Scene gScene;

// ===================== Tilemap =====================

Tilemap::Tilemap()
{
    
    
};

Tilemap::~Tilemap()
{
   
    delete[] tiles;
}

void Tilemap::init(int w, int h, int tile_width, int tile_height, int graphId)
{
    delete[] tiles;

    width = w;
    height = h;
    tilewidth = tile_width;
    tileheight= tile_height;
    graph = graphId;
    
    tiles = new Tile[width * height]{};
}

 


void Tilemap::setTilesetInfo(int tileWidth, int tileHeight,
                             int spacing_, int margin_, int columns)
{
 
   this->tilewidth = tileWidth;
    this->tileheight = tileHeight;
    spacing      = spacing_;
    margin       = margin_;
    columns = columns;
}

void Tilemap::clear()
{
    if (!tiles) return;
    for (int i = 0; i < width * height; i++)
        tiles[i] = Tile{0, 0, 0, 0.0f};
}

void Tilemap::setTile(int gx, int gy, const Tile& t)
{
    if (gx < 0 || gx >= width || gy < 0 || gy >= height)
        return;
    tiles[gy * width + gx] = t;
}

Rectangle Tilemap::GetTileRect(float x, float y)
{
    Rectangle r;
    
    x = fmaxf(0, fminf(x, width * tilewidth));
    y = fmaxf(0, fminf(y, height * tileheight));

    int px = x / tilewidth;
    int py = y / tileheight;

 

    float tileX = px  * tilewidth;
    float tileY = py  * tileheight;
    r.x = tileX;
    r.y = tileY;
    r.width  = tilewidth;
    r.height = tileheight;
    return r;
 
}

Vector2 Tilemap::gridToWorld(int gx, int gy)
{
    switch (grid_type)
    {
        case ORTHO:
            return {
                (float)gx * tilewidth,
                (float)gy * tileheight
            };

        case HEXAGON: // point-top hex
        {
            float offset = (gy % 2) * (tilewidth * 0.5f);
            return {
                (float)gx * tilewidth + offset,
                (float)gy * (tileheight * 0.75f)
            };
        }

        case ISOMETRIC:
        {
             
            float halfW = tilewidth * 0.5f;
            float halfH = tileheight * 0.5f;

            float x = (gx - gy) * halfW;
            float y = (gx + gy) * halfH * iso_compression; // compressão vertical

            return { x, y };
        }
    }

    return {0,0};
}


void Tilemap::worldToGrid(Vector2 pos, int& gx, int& gy)
{
    switch (grid_type)
    {
        case ORTHO:
            gx = (int)(pos.x / tilewidth);
            gy = (int)(pos.y / tileheight);
            return;

        case HEXAGON: // point-top hex
        {
            gy = (int)(pos.y / (tileheight * 0.75f));
            float offset = (gy % 2) * (tilewidth * 0.5f);
            gx = (int)((pos.x - offset) / tilewidth);
            return;
        }

        case ISOMETRIC:
        {
            float halfW = tilewidth * 0.5f;
            float halfH = tileheight * 0.5f;

            float gx_f = (pos.x / halfW + pos.y / (halfH * 0.5f)) * 0.5f;
            float gy_f = (pos.y / (halfH * 0.5f) - pos.x / halfW) * 0.5f;

            gx = (int)floor(gx_f);
            gy = (int)floor(gy_f);
            return;
        }
    }
}


void Tilemap::paintRect(int cx, int cy, int radius, uint16 id)
{
    for (int dy = -radius; dy <= radius; dy++)
    {
        for (int dx = -radius; dx <= radius; dx++)
        {
            Tile t{ id, 1, 1, 0.0f }; // shape=rect, solid=1
            setTile(cx + dx, cy + dy, t);
        }
    }
}

void Tilemap::paintCircle(int cx, int cy, float radius, uint16 id)
{
    int r = (int)radius;
    for (int dy = -r; dy <= r; dy++)
    {
        for (int dx = -r; dx <= r; dx++)
        {
            if (dx*dx + dy*dy <= radius*radius)
            {
                Tile t{ id, 2, 1, radius }; // shape=circle, solid=1
                setTile(cx + dx, cy + dy, t);
            }
        }
    }
}

void Tilemap::erase(int cx, int cy, int radius)
{
    for (int dy = -radius; dy <= radius; dy++)
    {
        for (int dx = -radius; dx <= radius; dx++)
        {
            setTile(cx + dx, cy + dy, Tile{0, 0, 0, 0.0f});
        }
    }
}

void Tilemap::eraseCircle(int cx, int cy, float radius)
{
    int r = (int)radius;
    for (int dy = -r; dy <= r; dy++)
    {
        for (int dx = -r; dx <= r; dx++)
        {
            if (dx*dx + dy*dy <= radius*radius)
                setTile(cx + dx, cy + dy, Tile{0, 0, 0, 0.0f});
        }
    }
}

void Tilemap::fill(int gx, int gy, uint16 id)
{
    Tile* start = getTile(gx, gy);
    if (!start) return;

    uint16 old_id = start->id;
    if (old_id == id) return;

    struct Coord { int x, y; };
    std::vector<Coord> stack;
    stack.reserve(256);
    stack.push_back({gx, gy});

    while (!stack.empty())
    {
        Coord c = stack.back();
        stack.pop_back();

        Tile* t = getTile(c.x, c.y);
        if (!t || t->id != old_id) continue;

        setTile(c.x, c.y, Tile{id, 1, 1, 0.0f});

        stack.push_back({c.x + 1, c.y});
        stack.push_back({c.x - 1, c.y});
        stack.push_back({c.x, c.y + 1});
        stack.push_back({c.x, c.y - 1});
    }
}



void Tilemap::render()
{
    if (graph == -1)
    {
        debug();
        return;
    }

    Graph* g = gGraphLib.getGraph(graph);
    if (!g) 
    {
        printf("Graph not found: %d\n", graph);
        debug();
        return;
    }

    Layer& l = gScene.layers[0];
    float scroll_x = l.scroll_x;
    float scroll_y = l.scroll_y;

    int start_x = (int)(scroll_x / tilewidth) - 1;
    int start_y = (int)(scroll_y / tileheight) - 1;
    int end_x = start_x + (gScene.width / tilewidth) + 3;
    int end_y = start_y + (gScene.height / tileheight) + 3;

    // int start_x = 0;
    // int start_y = 0;
    // int end_x = width;
    // int end_y = height;

    Texture2D atlas = gGraphLib.textures[g->texture];

    for (int gy = start_y; gy < end_y; gy++)
    {
        for (int gx = start_x; gx < end_x; gx++)
        {
            Tile* t = getTile(gx, gy);
            if (!t || t->id == 0) continue;

            Vector2 world = gridToWorld(gx, gy);
            float screen_x = (world.x - scroll_x);
            float screen_y = (world.y - scroll_y);

            int tile_id = t->id - 1;

            // Tiled: tilewidth/tileheight/spacing/margin/columns
            int atlas_x = margin + (tile_id % columns) * (tilewidth + spacing);
            int atlas_y = margin + (tile_id / columns) * (tileheight + spacing);

            Rectangle clip = {
                (float)atlas_x,
                (float)atlas_y,
                (float)tilewidth,
                (float)tileheight
            };

            RenderClipSize(
                atlas,
                screen_x, screen_y,
                (float)tilewidth, (float)tileheight,
                clip,
                false, false,
                WHITE,
                0
            );

           
        }
    }
    debug();
}

void Tilemap::debug()
{
    Layer& l = gScene.layers[0];
    float scroll_x = l.scroll_x;
    float scroll_y = l.scroll_y;

    int start_x = (int)(scroll_x / tilewidth) - 1;
    int start_y = (int)(scroll_y / tileheight) - 1;
    int end_x = start_x + (gScene.width / tilewidth) + 3;
    int end_y = start_y + (gScene.height / tileheight) + 3;

    for (int gy = start_y; gy < end_y; gy++)
    {
        for (int gx = start_x; gx < end_x; gx++)
        {
            Tile* t = getTile(gx, gy);
            if (!t || t->id == 0) continue;

            Vector2 world = gridToWorld(gx, gy);
            float screen_x = world.x - scroll_x;
            float screen_y = world.y - scroll_y;

            DrawRectangleLines((int)screen_x, (int)screen_y,
                               tilewidth, tileheight, GRAY);
            DrawText(TextFormat("%d", t->id),
                     (int)screen_x + 4, (int)screen_y + 4, 10, WHITE);
        }
    }
}

void Tilemap::renderGrid()
{
    int screen_w = gScene.width;
    int screen_h = gScene.height;

    for (int x = 0; x < screen_w; x += tilewidth)
        DrawLine(x, 0, x, screen_h, {50, 50, 50, 200});

    for (int y = 0; y < screen_h; y += tileheight)
        DrawLine(0, y, screen_w, y, {50, 50, 50, 200});
}

bool Tilemap::save(const char* filename)
{
    FILE* f = fopen(filename, "wb");
    if (!f) return false;

    fwrite(&width, sizeof(int), 1, f);
    fwrite(&height, sizeof(int), 1, f);
    fwrite(&tilewidth, sizeof(int), 1, f);
    fwrite(&tileheight, sizeof(int), 1, f);
    fwrite(&grid_type, sizeof(int), 1, f);
    fwrite(&graph, sizeof(int), 1, f);
    fwrite(&spacing, sizeof(int), 1, f);
    fwrite(&margin, sizeof(int), 1, f);
    fwrite(&iso_compression, sizeof(float), 1, f);
    fwrite(&columns, sizeof(int), 1, f);
    fwrite(tiles, sizeof(Tile), width * height, f);

    fclose(f);
    return true;
}

bool Tilemap::load(const char* filename)
{
    FILE* f = fopen(filename, "rb");
    if (!f) return false;

    int w, h, tile_w,tile_h, gt, graphId, spacing_, margin_, cols_;
    float iso_compression_;
    fread(&w, sizeof(int), 1, f);
    fread(&h, sizeof(int), 1, f);
    fread(&tile_w, sizeof(int), 1, f);
    fread(&tile_h, sizeof(int), 1, f);
    fread(&gt, sizeof(int), 1, f);
    fread(&graphId, sizeof(int), 1, f);
    fread(&spacing_, sizeof(int), 1, f);
    fread(&margin_, sizeof(int), 1, f);
    fread(&iso_compression_, sizeof(float), 1, f);
    fread(&cols_, sizeof(int), 1, f);

    init(w, h, tile_w, tile_h, graphId);
    grid_type = (GridType)gt;
    spacing = spacing_;
    margin = margin_;
    columns = cols_;
    iso_compression = iso_compression_;

    fread(tiles, sizeof(Tile), w * h, f);

    fclose(f);
    return true;
}



void SetTileMap(int layer, int map_width, int map_height, int tile_width, int tile_height, int columns, int graph)
{
    if (layer < 0 || layer >= MAX_LAYERS) return;
    if (gScene.layers[layer].tilemap) delete gScene.layers[layer].tilemap;
    gScene.layers[layer].tilemap = new Tilemap();
    gScene.layers[layer].tilemap->init(map_width, map_height, tile_width, tile_height, graph);
    gScene.layers[layer].tilemap->columns = columns;
    gScene.layers[layer].tilemap->grid_type = Tilemap::GridType::ORTHO;
    gScene.layers[layer].tilemap->spacing = 0;
    gScene.layers[layer].tilemap->margin = 0;

}

void SetTileMapSpacing(int layer, double spacing)
{
    if (layer < 0 || layer >= MAX_LAYERS) return;
    if (!gScene.layers[layer].tilemap) return;
    gScene.layers[layer].tilemap->spacing = spacing;
}

void SetTileMapMargin(int layer, double margin)
{
    if (layer < 0 || layer >= MAX_LAYERS) return;
    if (!gScene.layers[layer].tilemap) return;
    gScene.layers[layer].tilemap->margin = margin;
}

void SetTileMapMode(int layer, int mode)
{
    if (layer < 0 || layer >= MAX_LAYERS) return;
    if (!gScene.layers[layer].tilemap) return;
    gScene.layers[layer].tilemap->grid_type = (Tilemap::GridType)mode;
}

void SetTileMapIsoCompression(int layer, double compression)
{
    if (layer < 0 || layer >= MAX_LAYERS) return;
    if (!gScene.layers[layer].tilemap) return;
    gScene.layers[layer].tilemap->iso_compression = compression;
}

void SetTileMapTile(int layer, int x, int y, int tile,int solid)
{
    if (layer < 0 || layer >= MAX_LAYERS) return;
    if (!gScene.layers[layer].tilemap) return;
    Tile* t = gScene.layers[layer].tilemap->getTile(x, y);
    t->id = tile;
    t->solid = solid;
    gScene.layers[layer].tilemap->setTile(x, y, *t);
}

int GetTileMapTile(int layer, int x, int y)
{
    if (layer < 0 || layer >= MAX_LAYERS) return 0;
    if (!gScene.layers[layer].tilemap) return 0;
    Tile* t = gScene.layers[layer].tilemap->getTile(x, y);
    if (!t) return 0;
    return t->id;
    return 0;
}

void SetTileMapColumns(int layer, int columns)
{
    if (layer < 0 || layer >= MAX_LAYERS) return;
    if (!gScene.layers[layer].tilemap) return;
    gScene.layers[layer].tilemap->columns = columns;
}