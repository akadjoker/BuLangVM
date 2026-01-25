#pragma once
#include "config.hpp"
#include "render.hpp"
#include <vector>
#include <raylib.h>
#include <cstring>
#include <cmath>
#include <algorithm>

#define PAK_MAGIC "IMBU"
#define PAK_VERSION 1

enum LayerMode : uint8
{
    LAYER_MODE_TILEX = 1,    // 0001
    LAYER_MODE_TILEY = 2,    // 0010
    LAYER_MODE_STRETCHX = 4, // 0100
    LAYER_MODE_STRETCHY = 8, // 1000
    LAYER_MODE_FLIPX = 16,   // 10000
    LAYER_MODE_FLIPY = 32    // 100000
};

// Entity flags
#define MAX_LAYERS 6
#define MAXNAME 64
#define B_HMIRROR (1 << 0)
#define B_VMIRROR (1 << 1)
#define B_VISIBLE (1 << 2)
#define B_DEAD (1 << 3)
#define B_FROZEN (1 << 4)
#define B_STATIC (1 << 5)
#define B_COLLISION (1 << 6)

#define MAX_POINTS 8

enum ShapeType
{
    CIRCLE,
    RECTANGLE,
    POLYGON
};

class Scene;
struct Entity;

typedef void (*CollisionCallback)(Entity *a, Entity *b, void *userdata);

struct Shape
{
    uint8 type;
    virtual ~Shape() {}
    bool collide(Shape *other,
                 double x1, double y1, double a1, bool fx1, bool fy1,
                 double x2, double y2, double a2, bool fx2, bool fy2);

    virtual void draw(const Entity *entity, Color color) = 0;
};
struct CircleShape : public Shape
{
    float radius;
    Vector2 offset;

    CircleShape(float r = 1.0f) : radius(r), offset({0, 0}) { type = CIRCLE; }
    void draw(const Entity *entity, Color color) override;
};

struct PolygonShape : public Shape
{
    int num_points;
    Vector2 points[MAX_POINTS];
    Vector2 normals[MAX_POINTS];

    PolygonShape(int n = 0) : num_points(n) { type = POLYGON; }
    void calcNormals();
    void draw(const Entity *entity, Color color) override;
};

struct RectangleShape : public PolygonShape
{
    RectangleShape(int x, int y, int w, int h);
};

struct Graph
{
    std::vector<Vector2> points;
    int texture;        // id do opengl da textura , com raylib podmeos passar direito
    int width, height;  // obtemos da textura2d
    Rectangle clip;     // clip da textura (x, y, width, height)
    int id;             // id do graph
    char name[MAXNAME]; // nome do graph
};

struct PakHeader
{
    char magic[4];
    int version;
    int textureCount; // Texturas únicas
    int graphCount;   // Graphs (podem reutilizar texturas)
};

// Metadata de cada textura no .pak (guardada UMA VEZ)
struct PakTextureHeader
{
    char name[MAXNAME];
    int width;
    int height;
    int size; // tamanho em bytes dos pixels (width * height * 4)
};

// Metadata de cada graph no .pak (referencia texture_id + clip)
struct PakGraphHeader
{
    char name[MAXNAME];
    int texture; // qual textura usa
    float clip_x, clip_y, clip_w, clip_h;
    int point_count; // número de pontos (hotspots)
};

struct CollisionInfo
{
    Entity *collider;
    Vector2 normal;
    bool hit;
    double depth;
};

struct Entity
{
    uint32 id;
    int graph; // referencia ao Graph via ID
    Shape *shape;
    uint8 layer;
    double x, y;
    double last_x, last_y;
    bool flip_x, flip_y;

    uint32 collision_layer; // Em que layer esta entity está
    uint32 collision_mask;  // Com que layers esta entity colide

    double center_x;
    double center_y;

    int64 flags;

    double size_x; // GRAPHSIZEX
    double size_y; // GRAPHSIZEY

    Rectangle bounds;
    bool bounds_dirty;

    void updateBounds(); // Recalcula AABB
    Rectangle getBounds();
    int64 resolution;
    int64 angle;
    Color color;
    bool on_floor = false;
    bool on_wall = false;
    bool on_ceiling = false;
    Vector2 floor_normal = {0, 0};

    Vector2 getPoint(int pointIdx) const;
    Vector2 getRealPoint(int pointIdx) const;

    void setCollisionLayer(uint32 layer) { collision_layer = (1 << layer); }
    void setCollisionMask(uint32 mask) { collision_mask = mask; }
    void addCollisionMask(uint32 layer) { collision_mask |= (1 << layer); }
    void removeCollisionMask(uint32 layer) { collision_mask &= ~(1 << layer); }
    bool canCollideWith(const Entity *other) const { return (collision_mask & other->collision_layer) != 0; }

    bool collide(Entity *other);
    bool intersects(const Entity *other) const;

    Vector2 getRealPointCustom(double px, double py) const;

    int64 getAngle(const Entity *other) const;

    double getDistance(const Entity *other) const;

    void advance(double distance);
    void xadvance(int64 angle_param, double distance);

    // Movement helpers

    void turnTowards(double targetX, double targetY, int64 maxTurn);
    bool moveTowards(double targetX, double targetY, double speed, double stopDistance = 0.0);

    Entity();
    ~Entity();

    void setFlag(uint32 flag) { flags |= flag; }
    void clearFlag(uint32 flag) { flags &= ~flag; }
    void toggleFlag(uint32 flag) { flags ^= flag; }
    bool hasFlag(uint32 flag) const { return (flags & flag) != 0; }

    // Shortcuts comuns
    void show() { setFlag(B_VISIBLE); }
    void hide() { clearFlag(B_VISIBLE); }
    void kill() { setFlag(B_DEAD); }
    void freeze() { setFlag(B_FROZEN); }
    void unfreeze() { clearFlag(B_FROZEN); }
    void enableCollision() { setFlag(B_COLLISION); }
    void disableCollision() { clearFlag(B_COLLISION); }
    void setStatic() { setFlag(B_STATIC); }

    bool isVisible() const { return hasFlag(B_VISIBLE); }
    bool isDead() const { return hasFlag(B_DEAD); }
    bool isFrozen() const { return hasFlag(B_FROZEN); }
    bool isStatic() const { return hasFlag(B_STATIC); }

    void render();

    bool place_free(double x, double y);
    Entity *place_meeting(double x, double y);
    bool move_and_slide(Vector2 &velocity, float delta, Vector2 up_direction = {0, -1});
    bool move_and_collide(double vel_x, double vel_y, CollisionInfo *result);
    bool tiles_move_and_slide(Vector2 &velocity, float delta, Vector2 up_direction);
    void moveBy(double x, double y);
    bool snap_to_floor(float snap_len, Vector2 up_direction, Vector2 &velocity);
    bool collide_with_tiles(const Rectangle &box);
    void move_topdown(Vector2 velocity, float dt);

    void setRectangleShape(int x, int y, int w, int h);
    void setCircleShape(float radius);
    void setShape(Vector2 *points, int n);
};

// Enums
enum EmitterType
{
    EMITTER_CONTINUOUS,
    EMITTER_ONESHOT
};

 

// ============================================================================
// Particle Structure
// ============================================================================
struct Particle
{
    Vector2 pos = {0, 0};
    Vector2 vel = {0, 0};
    Vector2 acc = {0, 0}; // Aceleração (ex: gravidade)

    Color color = WHITE;
    Color startColor = WHITE;
    Color endColor = WHITE;

    float life = 0.0f;
    float maxLife = 1.0f;

    float size = 1.0f;
    float startSize = 1.0f;
    float endSize = 0.0f;

    float rotation = 0.0f;
    float angularVel = 0.0f;

    bool alive = false;
};

// ============================================================================
// Emitter Class
// ============================================================================
class Emitter
{
public:
    // Configuração básica
    EmitterType type;
    int graph;
    int layer;

    Vector2 pos;
    Vector2 dir;

    // Parâmetros de emissão
    float spread; // Cone de dispersão (radians)
    float rate;   // Partículas por segundo
    float speedMin;
    float speedMax;
    float particleLife;
    float lifetime; // Duração do emitter (oneshot)

    // Aparência
    Color colorStart;
    Color colorEnd;
    float sizeStart;
    float sizeEnd;

    // Física
    Vector2 gravity; // Aceleração constante
    float drag;      // Resistência do ar (0-1)

    // Rotação
    float rotationMin;
    float rotationMax;
    float angularVelMin;
    float angularVelMax;
     Rectangle spawnZone;

    // Rendering
    BlendMode blendMode;

    // Estado
    bool active;
    bool finished;

    // Partículas
    std::vector<Particle> particles;


    // Setters convenientes
    void setPosition(float x, float y) { pos = {x, y}; }
    void setDirection(float x, float y) { dir = {x, y}; }
    void setEmissionRate(float r) { rate = r; }
    void setLife(float life) { particleLife = life; }
    void setSpeedRange(float min, float max) { speedMin = min; speedMax = max; }
    void setSpread(float radians) { spread = radians; }
    void setColorCurve(Color start, Color end) { colorStart = start; colorEnd = end; }
    void setSizeCurve(float start, float end) { sizeStart = start; sizeEnd = end; }
    void setSpawnZone(float x, float y, float w, float h) { spawnZone = {x, y, w, h}; }
    void setLifeTime(float time) { lifetime = time; }
    void setGravity(float x, float y) { gravity = {x, y}; }
    void setDrag(float d) { drag = d; }
    void setRotationRange(float min, float max) { rotationMin = min; rotationMax = max; }
    void setAngularVelRange(float min, float max) { angularVelMin = min; angularVelMax = max; }
    void setBlendMode(BlendMode mode) { blendMode = mode; }
    void setLayer(int l) { layer = l; }
    
    // Getters
    int getAliveCount() const { return aliveCount; }
    int getMaxParticles() const { return (int)particles.size(); }
    bool isFinished() const { return finished; }
    Vector2 getPosition() const { return pos; }


private:
    float elapsed;
    float accumulator;
    int aliveCount;
    int firstDead;

    void emit();
    void emitAt(int index);

public:
    Emitter(EmitterType t, int gr, int maxParticles);
    ~Emitter() = default;

    void update(float dt);
    void draw();

    void burst(int count);
    void stop();
    void restart();

 
};

// ============================================================================
// Particle System Manager
// ============================================================================
class ParticleSystem
{
private:
    std::vector<Emitter *> emitters;

public:
    ParticleSystem();
    ~ParticleSystem();

    // Criação de emitters
    Emitter *spawn(EmitterType type, int graph, int maxParticles = 100);

    // Presets comuns
    Emitter *createExplosion(Vector2 pos, int graph, Color color);
    Emitter *createSmoke(Vector2 pos, int graph);
    Emitter *createFire(Vector2 pos, int graph);
    Emitter *createSparks(Vector2 pos, int graph, Color color);

    void update(float dt);
    void draw();
    void clear();

    int getEmitterCount() const { return (int)emitters.size(); }
    int getTotalParticles() const;
};
struct QuadtreeNode
{
    Rectangle bounds;
    QuadtreeNode *children[4];
    std::vector<Entity *> items;
    int depth;

    static const int MAX_ITEMS = 8;
    static const int MAX_DEPTH = 8;

    QuadtreeNode(Rectangle b, int d) : bounds(b), depth(d)
    {
        children[0] = children[1] = children[2] = children[3] = nullptr;
    }

    ~QuadtreeNode()
    {
        for (int i = 0; i < 4; i++)
            if (children[i])
                delete children[i];
    }

    bool overlapsRect(Rectangle other);
    void insert(Entity *entity);
    void query(Rectangle area, std::vector<Entity *> &result);
    void split();
    void clear();
    void draw();
};

class Quadtree
{
    QuadtreeNode *root;

public:
    Quadtree(Rectangle world_bounds);

    ~Quadtree();

    void draw();

    void clear();
    void insert(Entity *entity);
    void query(Rectangle area, std::vector<Entity *> &result);
    void rebuild(Scene *scene);
};

struct Tile
{
    uint16 id;    // 0 = vazio
    uint8 shape;  // 0=none, 1=rect, 2=circle (se quiseres no futuro)
    uint8 solid;  // 0=false, 1=true
    float radius; // usado se shape=circle
};

class Tilemap
{
public:
    enum GridType
    {
        ORTHO = 0,
        HEXAGON = 1,
        ISOMETRIC = 2
    };

    Tilemap();
    ~Tilemap();

    void init(int w, int h, int tile_width, int tile_height, int graphId);

    // Configuração do tileset (Tiled: tilewidth, tileheight, spacing, margin, columns)
    void setTilesetInfo(int tileWidth, int tileHeight,
                        int spacing, int margin, int columns);

    void clear();

    inline Tile *getTile(int gx, int gy)
    {
        if (gx < 0 || gx >= width || gy < 0 || gy >= height)
            return nullptr;
        return &tiles[gy * width + gx];
    }

    void setTile(int gx, int gy, const Tile &t);

    // Conversão de coordenadas
    Vector2 gridToWorld(int gx, int gy);
    void worldToGrid(Vector2 pos, int &gx, int &gy);
    Rectangle GetTileRect(float x, float y);

    // Pintura
    void paintRect(int cx, int cy, int radius, uint16 id);
    void paintCircle(int cx, int cy, float radius, uint16 id);
    void erase(int cx, int cy, int radius);
    void eraseCircle(int cx, int cy, float radius);
    void fill(int gx, int gy, uint16 id);

    // Colisão
    void getCollidingTiles(Rectangle bounds, std::vector<Rectangle> &out);
    void getCollidingSolids(Rectangle bounds, std::vector<Rectangle> &out);

    inline bool isSolid(int gx, int gy)
    {
        Tile *t = getTile(gx, gy);
        return t && t->solid;
    }

    // Render
    void render();
    void debug();
    void renderGrid();

    // Save/Load
    bool save(const char *filename);
    bool load(const char *filename);

    // Dados públicos (simples, estilo engine)
    int width = 0;
    int height = 0;
    int tilewidth = 32; // tilewidth/tileheight (assumimos quadrado)
    int tileheight = 32;
    int tileset_id = 0; // id no GraphLib (se usares)
    int spacing = 0;    // Tiled: spacing
    int margin = 0;     // Tiled: margin
    int columns = 1;    // Tiled: columns
    int graph = -1;     // índice do Graph/atlas
    float iso_compression = 0.5f;
    GridType grid_type = ORTHO;

private:
    Tile *tiles = nullptr;
};

struct Layer
{
    std::vector<Entity *> nodes;
    int front{-1}; // -1 se não tem
    int back{-1};  // -1 se não tem
    Tilemap *tilemap{nullptr};
    uint8 mode; // tilex, tiley, stretch x, stretch y
    Rectangle size;
    // Parallax/scroll
    double scroll_factor_x; // 0.5 = parallax, 1.0 = normal
    double scroll_factor_y;
    double scroll_x; // offset de scroll
    double scroll_y;
    void destroy();
    void render();
    void render_parelax(Graph *g);
};

struct Scene
{
    std::vector<Entity *> nodesToRemove;
    std::vector<Entity *> staticEntities;  // Cache de estáticas
    std::vector<Entity *> dynamicEntities; // Cache de dinâmicas
    Quadtree *staticTree;
    double scroll_x, scroll_y;
    int width, height;

    CollisionCallback onCollision;
    void *collisionUserData;

    Layer layers[6];
    Entity *addEntity(int graphId, int layer, double x, double y);
    void moveEntityToLayer(Entity *node, int layer);
    void removeEntity(Entity *node);
    void destroy();

    // Collision
    void initCollision(Rectangle worldBounds);
    void updateCollision();
    void checkCollisions();
    void setCollisionCallback(CollisionCallback callback, void *userdata = nullptr);

    Scene();
    ~Scene();
};

struct GraphLib
{
    std::vector<Graph> graphs;
    std::vector<Texture2D> textures;
    Texture2D defaultTexture;

    // Carrega individual
    int load(const char *name, const char *texturePath);
    int loadAtlas(const char *name, const char *texturePath, int count_x, int count_y);
    int addSubGraph(int parentId, const char *name, int x, int y, int w, int h);

    bool savePak(const char *pakFile);
    bool loadPak(const char *pakFile);

    Graph *getGraph(int id);
    Texture2D *getTexture(int id);

    int getGraphCount() const { return graphs.size(); }
    int getTextureCount() const { return textures.size(); }

    void create();
    void destroy();
};

enum PathHeuristic
{
    PF_MANHATTAN = 0,
    PF_EUCLIDEAN,
    PF_OCTILE,
    PF_CHEBYSHEV
};

enum PathAlgorithm
{
    PATH_ASTAR,
    PATH_DIJKSTRA
};

enum NodeState
{
    NS_UNVISITED = 0,
    NS_OPEN,
    NS_CLOSED
};

struct GridNode
{
    float f = 0.0f;
    float g = 0.0f;
    float h = 0.0f;
    int parent_idx = -1;
    NodeState state = NS_UNVISITED;
    int walkable = 1;
};

// Binary heap para open list (min-heap por f; tie-break por h)
class OpenListHeap
{
private:
    std::vector<int> heap; // indices de nós
    std::vector<int> pos;  // pos[nodeIndex] = posição no heap, -1 se não está
    GridNode *nodes = nullptr;

    bool better(int a, int b) const;
    void swapAt(int a, int b);
    void bubbleUp(int idx);
    void bubbleDown(int idx);

public:
    OpenListHeap(int size = 0);

    void resize(int size);
    void setNodes(GridNode *g);

    bool empty() const;
    void clear();

    void push(int idx);
    int pop();
    void update(int idx);
    bool contains(int idx) const;
};

class Mask
{
private:
    GridNode *grid = nullptr;
    int width = 0;
    int height = 0;
    int resolution = 1;
    OpenListHeap openList;

    static const int dx[8];
    static const int dy[8];

    float manhattan(int dx, int dy) const;
    float euclidean(int dx, int dy) const;
    float octile(int dx, int dy) const;
    float chebyshev(int dx, int dy) const;

    float calcHeuristic(int dx, int dy, PathHeuristic heur, int diag) const;
    bool isValidDiagonal(int cx, int cy, int nx, int ny) const;

public:
    Mask(int w, int h, int res = 1);
    ~Mask();

    void setOccupied(int x, int y);
    void setFree(int x, int y);
    void clearAll();

    bool isOccupied(int x, int y) const; // out-of-bounds = true
    bool isWalkable(int x, int y) const;

    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getResolution() const { return resolution; }

    // world ↔ grid
    Vector2 worldToGrid(Vector2 pos) const;
    Vector2 gridToWorld(Vector2 pos) const;
    Vector2 gridToWorldFloat(float gx, float gy) const;

    void loadFromImage(const char *imagePath, int threshold = 128);

    std::vector<Vector2> findPath(int sx, int sy, int ex, int ey,
                                  int diag = 1,
                                  PathAlgorithm algo = PATH_ASTAR,
                                  PathHeuristic heur = PF_MANHATTAN);
};

struct SoundData
{
    Sound sound;
    int id;
    char name[MAXNAME];
};

struct SoundLib
{

    std::vector<SoundData> sounds;

    int load(const char *name, const char *soundPath);

    Sound *getSound(int id);
    SoundData *getSoundData(int id);

    // Play direto pelo ID
    void play(int id, float volume = 1.0f, float pitch = 1.0f);
    void stop(int id);
    void pause(int id);
    void resume(int id);

    // Info
    bool isSoundPlaying(int id);
    int getSoundCount() const { return sounds.size(); }

    // Cleanup
    void destroy();
};

void InitScene();
void DestroyScene();
void RenderScene();
void InitCollision(int x, int y, int width, int height, CollisionCallback onCollision);
void UpdateCollision();
void CheckCollisions();

void InitSound();
void DestroySound();
void PlaySound(int id, float volume = 1.0f, float pitch = 1.0f);
void StopSound(int id);
void PauseSound(int id);
void ResumeSound(int id);
bool IsSoundPlaying(int id);

Entity *CreateEntity(int graphId, int layer, double x, double y);
void RemoveEntity(Entity *node);
void MoveEntityToLayer(Entity *node, int layer);

void SetLayerMode(int layer, uint8 mode);
void SetLayerScroll(int layer, double x, double y);
void SetLayerScrollFactor(int layer, double x, double y);
void SetLayerSize(int layer, int x, int y, int width, int height);
void SetLayerBackGraph(int layer, int graph);
void SetLayerFrontGraph(int layer, int graph);

void SetTileMap(int layer, int map_width, int map_height, int tile_width, int tile_height, int columns, int graph);
void SetTileMapSpacing(int layer, double spacing);
void SetTileMapMargin(int layer, double margin);
void SetTileMapMode(int layer, int mode);
void SetTileMapIsoCompression(int layer, double compression);

void SetTileMapTile(int layer, int x, int y, int tile, int solid = 1);
int GetTileMapTile(int layer, int x, int y);

void SetScroll(double x, double y);

int LoadGraph(const char *name, const char *texturePath);
int LoadAtlas(const char *name, const char *texturePath, int count_x, int count_y);
int LoadSubGraph(int id, const char *name, int x, int iy, int iw, int ih);
bool SaveGraphics(const char *name);
bool LoadGraphics(const char *name);