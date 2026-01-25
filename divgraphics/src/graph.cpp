#include "engine.hpp"

#include <cstdio>
#include <algorithm>

extern GraphLib gGraphLib;

void GraphLib::create()
{
    Image image = GenImageChecked(32, 32, 4, 4, WHITE, BLACK);
    defaultTexture = LoadTextureFromImage(image);
    UnloadImage(image);

    Graph g = {};
    g.id = 0;
    g.texture = 0;
    g.width = defaultTexture.width;
    g.height = defaultTexture.height;
    g.clip = {0, 0, (float)g.width, (float)g.height};
    strncpy(g.name, "default", MAXNAME - 1);
    g.points.push_back({(float)defaultTexture.width / 2, (float)defaultTexture.height / 2});

    graphs.push_back(g);
    textures.push_back(defaultTexture);
}

int GraphLib::load(const char *name, const char *texturePath)
{
    Image img = LoadImage(texturePath);
    if (img.data == nullptr)
    {
        return -1;
    }

    Texture2D tex = LoadTextureFromImage(img);
    UnloadImage(img);

    Graph g = {};
    g.id = (int)graphs.size();
    g.texture = (int)textures.size();
    g.width = tex.width;
    g.height = tex.height;
    g.clip = {0, 0, (float)tex.width, (float)tex.height};
    strncpy(g.name, name, MAXNAME - 1);
    g.name[MAXNAME - 1] = '\0';
    g.points.push_back({(float)tex.width / 2, (float)tex.height / 2});

    graphs.push_back(g);
    textures.push_back(tex);

    return g.id;
}

int GraphLib::loadAtlas(const char *name, const char *texturePath, int count_x, int count_y)
{
    Image img = LoadImage(texturePath);
    if (img.data == nullptr)
    {
        return -1;
    }

    Texture2D tex = LoadTextureFromImage(img);
    UnloadImage(img);

    int tile_w = tex.width / count_x;
    int tile_h = tex.height / count_y;
    int firstId = (int)graphs.size();

    for (int y = 0; y < count_y; y++)
    {
        for (int x = 0; x < count_x; x++)
        {
            Graph g = {};
            g.id = (int)graphs.size();
            g.texture = (int)textures.size();
            g.width = tile_w;
            g.height = tile_h;
            g.clip = {(float)(x * tile_w), (float)(y * tile_h), (float)tile_w, (float)tile_h};
            snprintf(g.name, MAXNAME, "%s_%d_%d", name, x, y);
            g.name[MAXNAME - 1] = '\0';
            g.points.push_back({(float)tile_w / 2, (float)tile_h / 2});

            graphs.push_back(g);
        }
    }

    textures.push_back(tex);
    return firstId;
}

int GraphLib::addSubGraph(int parentId, const char *name, int x, int y, int w, int h)
{
    if (parentId < 0 || parentId >= (int)graphs.size())
        return -1;

    Graph parent = graphs[parentId];

    Graph g = {};
    g.id = (int)graphs.size();
    g.texture = parent.texture; // Reutiliza a MESMA textura!
    g.width = w;
    g.height = h;
    g.clip = {(float)x, (float)y, (float)w, (float)h};
    strncpy(g.name, name, MAXNAME - 1);
    g.name[MAXNAME - 1] = '\0';
    g.points.push_back({(float)w / 2, (float)h / 2});

    graphs.push_back(g);
    return g.id;
}

Graph *GraphLib::getGraph(int id)
{
    if (id < 0 || id >= (int)graphs.size())
        return &graphs[0];
    return &graphs[id];
}

Texture2D *GraphLib::getTexture(int id)
{
    if (id < 0 || id >= (int)textures.size())
        return nullptr;
    return &textures[id];
}

bool GraphLib::savePak(const char *pakFile)
{
    FILE *f = fopen(pakFile, "wb");
    if (!f)
        return false;


// Header
PakHeader header;
strcpy(header.magic, PAK_MAGIC);
header.version = PAK_VERSION;
header.textureCount = (int)textures.size();
header.graphCount = (int)graphs.size();

if (fwrite(&header, sizeof(PakHeader), 1, f) != 1)
{
    fclose(f);
    return false;
}

// ===== SAVE UNIQUE TEXTURES =====
for (size_t texIdx = 0; texIdx < textures.size(); texIdx++)
{
    Texture2D &tex = textures[texIdx];

    // Lê os pixels da VRAM
    Image img = LoadImageFromTexture(tex);

    // Texture header
    PakTextureHeader texHeader;
    snprintf(texHeader.name, MAXNAME, "tex_%ld", texIdx);
    texHeader.width = img.width;
    texHeader.height = img.height;
    texHeader.size = img.width * img.height * 4;

    if (fwrite(&texHeader, sizeof(PakTextureHeader), 1, f) != 1)
    {
        UnloadImage(img);
        fclose(f);
        return false;
    }

    // Pixels (RGBA)
    Image rgba = ImageCopy(img);
    ImageFormat(&rgba, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);

    if (fwrite(rgba.data, 1, texHeader.size, f) != (size_t)texHeader.size)
    {
        UnloadImage(img);
        UnloadImage(rgba);
        fclose(f);
        return false;
    }

    UnloadImage(img);
    UnloadImage(rgba);
}

// ===== SAVE GRAPHS (COM REFERÊNCIAS À TEXTURA) =====
for (auto &g : graphs)
{
    PakGraphHeader graphHeader;
    strncpy(graphHeader.name, g.name, MAXNAME - 1);
    graphHeader.name[MAXNAME - 1] = '\0';

    graphHeader.texture = g.texture;
    graphHeader.clip_x = g.clip.x;
    graphHeader.clip_y = g.clip.y;
    graphHeader.clip_w = g.clip.width;
    graphHeader.clip_h = g.clip.height;
    graphHeader.point_count = (int)g.points.size();

    if (fwrite(&graphHeader, sizeof(PakGraphHeader), 1, f) != 1)
    {
        fclose(f);
        return false;
    }

    // Save points
    for (auto &p : g.points)
    {
        if (fwrite(&p, sizeof(Vector2), 1, f) != 1)
        {
            fclose(f);
            return false;
        }
    }
}

fclose(f);
return true;
}

bool GraphLib::loadPak(const char *pakFile)
{
    FILE *f = fopen(pakFile, "rb");
    if (!f)
        return false;

    // Header
    PakHeader header;
    if (fread(&header, sizeof(PakHeader), 1, f) != 1)
    {
        fclose(f);
        return false;
    }

    // Verifica
    if (strncmp(header.magic, PAK_MAGIC, 4) != 0 || header.version != PAK_VERSION)
    {
        fclose(f);
        return false;
    }

    // Limpa
    destroy();

    // ===== LOAD UNIQUE TEXTURES =====
    for (int i = 0; i < header.textureCount; i++)
    {
        PakTextureHeader texHeader;
        if (fread(&texHeader, sizeof(PakTextureHeader), 1, f) != 1)
        {
            fclose(f);
            return false;
        }

        unsigned char *pixels =  (unsigned char*)malloc(texHeader.size);
        if (fread(pixels, 1, texHeader.size, f) != (size_t)texHeader.size)
        {
            delete[] pixels;
            fclose(f);
            return false;
        }
        printf("Loaded texture %s (%d x %d)\n", texHeader.name, texHeader.width, texHeader.height);

        Image img = {};
        img.data = pixels;
        img.width = texHeader.width;
        img.height = texHeader.height;
        img.mipmaps = 1;
        img.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;

        Texture2D tex = LoadTextureFromImage(img);
        textures.push_back(tex);

        UnloadImage(img);
    }

    // ===== LOAD GRAPHS (QUE REFERENCIAM AS TEXTURAS) =====
    for (int i = 0; i < header.graphCount; i++)
    {
        PakGraphHeader graphHeader;
        if (fread(&graphHeader, sizeof(PakGraphHeader), 1, f) != 1)
        {
            fclose(f);
            return false;
        }

        Graph g = {};
        g.id = (int)graphs.size();
        g.texture = graphHeader.texture; // Aponta à textura deduplicated!
        g.width = (int)graphHeader.clip_w;
        g.height = (int)graphHeader.clip_h;
        g.clip = {graphHeader.clip_x, graphHeader.clip_y, graphHeader.clip_w, graphHeader.clip_h};
        strncpy(g.name, graphHeader.name, MAXNAME - 1);
        g.name[MAXNAME - 1] = '\0';
        

        // Load points
        for (int p = 0; p < graphHeader.point_count; p++)
        {
            Vector2 pt;
            if (fread(&pt, sizeof(Vector2), 1, f) != 1)
            {
                fclose(f);
                return false;
            }
            g.points.push_back(pt);
        }

        graphs.push_back(g);
    }

    fclose(f);
    return true;
}

void GraphLib::destroy()
{
    for (size_t i = 0; i < textures.size(); i++)
    {
        UnloadTexture(textures[i]);
    }
    textures.clear();
    graphs.clear();
}

int LoadGraph(const char *name, const char *texturePath)
{
    return gGraphLib.load(name, texturePath);
}

int LoadAtlas(const char *name, const char *texturePath, int count_x, int count_y)
{
    return gGraphLib.loadAtlas(name, texturePath, count_x, count_y);
}

int LoadSubGraph(int id, const char *name, int x, int iy, int iw, int ih)
{
    return gGraphLib.addSubGraph(id, name, x, iy, iw, ih);
}

bool SaveGraphics(const char *name)
{
    return gGraphLib.savePak(name);
}

bool LoadGraphics(const char *name)
{
    return gGraphLib.loadPak(name);
}