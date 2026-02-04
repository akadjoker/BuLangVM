
#include "raylib.h"
#include "engine.hpp"
#include <cmath>
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 800;
const int GRID_SIZE = 40;

inline void scroll_test()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Physics2D - Features Demo");
    //    SetTargetFPS(30);

    InitScene();

    int nave = LoadGraph("nave", "assets/ship.png");
    int gr = LoadGraph("orb", "assets/Orb.png");
    int bk1 = LoadGraph("bck1", "assets/0.png");
    int bk2 = LoadGraph("bck2", "assets/1.png");
    int bk3 = LoadGraph("bck3", "assets/2.png");
    int bk4 = LoadGraph("bck4", "assets/3.png");
    int bk5 = LoadGraph("bck5", "assets/4.png");

    for (int i = 0; i < 20; i++)
    {
        int x = GetRandomValue(50, SCREEN_WIDTH * 2);
        int y = GetRandomValue(50, SCREEN_HEIGHT - 50);
        Entity *e = CreateEntity(gr, 4, x, y);
        e->setStatic();
        e->setCircleShape(20);
        e->setCollisionLayer(1);                  // Enemy layer
        e->setCollisionMask((1 << 0) | (1 << 2)); // Colide com Player e Bullets
    }

    for (int i = 0; i < 20; i++)
    {
        int x = GetRandomValue(0, SCREEN_WIDTH * 2);
        int y = GetRandomValue(50, SCREEN_HEIGHT - 50);
        Entity *e = CreateEntity(gr, 5, x, y);
        e->setStatic();
        e->setCircleShape(20);
        e->setCollisionLayer(1);                  // Enemy layer
        e->setCollisionMask((1 << 0) | (1 << 2)); // Colide com Player e Bullets
    }

    Entity *player = CreateEntity(nave, 4, 200, 200);
    player->setRectangleShape(-40, -40, 60, 80);
    // player->setCircleShape(20);

    // player->setCircleShape(20);
    player->setCollisionLayer(0);       // Player layer
    player->setCollisionMask((1 << 1)); // Só colide com Enemies

    SetLayerMode(0, LAYER_MODE_STRETCHX | LAYER_MODE_STRETCHY);
    SetLayerMode(1, LAYER_MODE_TILEX);
    SetLayerMode(2, LAYER_MODE_TILEX);
    SetLayerMode(3, LAYER_MODE_TILEX);
    SetLayerMode(4, LAYER_MODE_TILEX);

    SetLayerBackGraph(0, bk5);
    SetLayerBackGraph(1, bk4);
    SetLayerBackGraph(2, bk3);
    SetLayerBackGraph(3, bk2);
    SetLayerBackGraph(4, bk1);

    SetLayerSize(-1, 0, 0, SCREEN_WIDTH * 2, SCREEN_HEIGHT);

    SetLayerScrollFactor(1, 0.1, 0.0);
    SetLayerScrollFactor(2, 0.3, 0.0);
    SetLayerScrollFactor(3, 0.5, 0.0);
    SetLayerScrollFactor(4, 0.7, 0.0);

    //   InitCollision(0, 0, SCREEN_WIDTH * 2, SCREEN_HEIGHT, handleCollision);
    InitCollision(0, 0, SCREEN_WIDTH * 2, SCREEN_HEIGHT, nullptr);

    Vector2 velocity = {0, 0};

    while (!WindowShouldClose())
    {

        float dt = GetFrameTime();

        SetScroll(player->x - SCREEN_WIDTH / 2, player->y - SCREEN_HEIGHT / 2);

        BeginDrawing();

        ClearBackground(BLACK);

        Vector2 velocity = {0, 0};
        if (IsKeyDown(KEY_RIGHT))
            velocity.x = 300;
        if (IsKeyDown(KEY_LEFT))
            velocity.x = -300;
        if (IsKeyDown(KEY_DOWN))
            velocity.y = 300;
        if (IsKeyDown(KEY_UP))
            velocity.y = -300;
        RenderScene();

        // CollisionInfo collision;
        // if (player->move_and_collide(velocity.x * GetFrameTime(),
        //                              velocity.y * GetFrameTime(),
        //                              &collision))
        // {

        //      velocity = Slide(velocity, collision.normal);
        //     // Slide
        //    //double dot = velocity.x * collision.normal.x + velocity.y * collision.normal.y;
        //   //  velocity.x -= dot * collision.normal.x;
        //   //  velocity.y -= dot * collision.normal.y;
        //   DrawText(TextFormat("%f, %f , %f", collision.normal.x, collision.normal.y,collision.depth), 10, 10, 20, RED);
        // }

        player->move_and_slide(velocity, dt, {0, -1});
        //  player->snap_to_floor(6.0f, {0, -1}, velocity);

        // Move and slide
        // Vector2 up = {0, -1};  // Up direction (para floating não importa)
        // player->move_and_slide(velocity, dt, up);

        // UI

        EndDrawing();
    }

    DestroyScene();

    CloseWindow();
}

inline Vector2 vector2(float x, float y)
{
    return {x, y};
}

void test_path()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pathfinding Demo - A* com Raylib");
    SetTargetFPS(60);

    // Cria a grid (30x20 células)
    int gridWidth = SCREEN_WIDTH / GRID_SIZE;
    int gridHeight = SCREEN_HEIGHT / GRID_SIZE;
    Mask mask(gridWidth, gridHeight, GRID_SIZE);

    // Posições inicial e final
    Vector2 start = vector2(1, 1);
    Vector2 end = vector2(gridWidth - 2, gridHeight - 2);

    // Caminho atual
    std::vector<Vector2> path;

    // Estado da UI
    bool drawingWalls = false;
    bool erasingWalls = false;
    bool settingStart = false;
    bool settingEnd = false;
    bool showGrid = true;

    // Configurações do algoritmo
    PathAlgorithm currentAlgo = PATH_ASTAR;
    PathHeuristic currentHeuristic = PF_MANHATTAN;
    bool allowDiagonal = true;

    // Adiciona algumas paredes iniciais
    for (int x = 10; x < 20; x++)
    {
        mask.setOccupied(x, 10);
    }
    for (int y = 5; y < 15; y++)
    {
        mask.setOccupied(15, y);
    }

    // Calcula caminho inicial
    path = mask.findPath((int)start.x, (int)start.y,
                         (int)end.x, (int)end.y,
                         allowDiagonal ? 1 : 0,
                         currentAlgo, currentHeuristic);

    while (!WindowShouldClose())
    {
        // ============ INPUT ============
        Vector2 mousePos = GetMousePosition();
        int gridX = (int)(mousePos.x / GRID_SIZE);
        int gridY = (int)(mousePos.y / GRID_SIZE);

        bool needsUpdate = false;

        // Desenhar/apagar paredes
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            if (gridX >= 0 && gridX < gridWidth && gridY >= 0 && gridY < gridHeight)
            {
                if (gridX != (int)start.x || gridY != (int)start.y)
                {
                    if (gridX != (int)end.x || gridY != (int)end.y)
                    {
                        mask.setOccupied(gridX, gridY);
                        needsUpdate = true;
                    }
                }
            }
        }

        if (IsKeyDown(KEY_F) && IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
        {
            if (gridX >= 0 && gridX < gridWidth && gridY >= 0 && gridY < gridHeight)
            {
                mask.setFree(gridX, gridY);
                needsUpdate = true;
            }
        }

        // Mover início (pressionar 'S' + click)
        if (IsKeyDown(KEY_S) && IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
        {
            if (gridX >= 0 && gridX < gridWidth && gridY >= 0 && gridY < gridHeight)
            {
                if (mask.isWalkable(gridX, gridY))
                {
                    start = {(float)gridX, (float)gridY};
                    needsUpdate = true;
                }
            }
        }

        // Mover fim (pressionar 'E' + click)
        if (IsKeyDown(KEY_E) && IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
        {
            if (gridX >= 0 && gridX < gridWidth && gridY >= 0 && gridY < gridHeight)
            {
                if (mask.isWalkable(gridX, gridY))
                {
                    end = {(float)gridX, (float)gridY};
                    needsUpdate = true;
                }
            }
        }

        // Limpar tudo
        if (IsKeyPressed(KEY_C))
        {
            mask.clearAll();
            needsUpdate = true;
        }

        // Toggle diagonal
        if (IsKeyPressed(KEY_D))
        {
            allowDiagonal = !allowDiagonal;
            needsUpdate = true;
        }

        // Alternar algoritmo
        if (IsKeyPressed(KEY_A))
        {
            currentAlgo = (currentAlgo == PATH_ASTAR) ? PATH_DIJKSTRA : PATH_ASTAR;
            needsUpdate = true;
        }

        // Alternar heurística (apenas para A*)
        if (IsKeyPressed(KEY_H))
        {
            currentHeuristic = (PathHeuristic)((currentHeuristic + 1) % 4);
            needsUpdate = true;
        }

        // Toggle grid
        if (IsKeyPressed(KEY_G))
        {
            showGrid = !showGrid;
        }

        // Recalcula caminho se necessário
        if (needsUpdate)
        {
            path = mask.findPath((int)start.x, (int)start.y,
                                 (int)end.x, (int)end.y,
                                 allowDiagonal ? 1 : 0,
                                 currentAlgo, currentHeuristic);
        }

        // ============ DRAW ============
        BeginDrawing();
        ClearBackground({20, 20, 20, 255});

        // Desenha grid
        if (showGrid)
        {
            for (int x = 0; x <= gridWidth; x++)
            {
                DrawLine(x * GRID_SIZE, 0, x * GRID_SIZE, SCREEN_HEIGHT, LIGHTGRAY);
            }
            for (int y = 0; y <= gridHeight; y++)
            {
                DrawLine(0, y * GRID_SIZE, SCREEN_WIDTH, y * GRID_SIZE, LIGHTGRAY);
            }
        }

        // Desenha células ocupadas
        for (int y = 0; y < gridHeight; y++)
        {
            for (int x = 0; x < gridWidth; x++)
            {
                if (mask.isOccupied(x, y))
                {
                    DrawRectangle(x * GRID_SIZE, y * GRID_SIZE,
                                  GRID_SIZE, GRID_SIZE, DARKGRAY);
                }
            }
        }

        // Desenha o caminho
        if (!path.empty())
        {
            for (size_t i = 0; i < path.size() - 1; i++)
            {
                Vector2 p1 = path[i];
                Vector2 p2 = path[i + 1];

                DrawLineEx(
                    {p1.x + GRID_SIZE * 0.5f, p1.y + GRID_SIZE * 0.5f},
                    {p2.x + GRID_SIZE * 0.5f, p2.y + GRID_SIZE * 0.5f},
                    4.0f, GREEN);
            }

            // Desenha pontos do caminho
            for (const auto &p : path)
            {
                DrawCircle((int)p.x + GRID_SIZE / 2, (int)p.y + GRID_SIZE / 2,
                           6, SKYBLUE);
            }
        }

        // Desenha início (verde)
        DrawRectangle((int)start.x * GRID_SIZE, (int)start.y * GRID_SIZE,
                      GRID_SIZE, GRID_SIZE, GREEN);
        DrawText("S", (int)start.x * GRID_SIZE + GRID_SIZE / 2 - 8,
                 (int)start.y * GRID_SIZE + GRID_SIZE / 2 - 10, 20, WHITE);

        // Desenha fim (vermelho)
        DrawRectangle((int)end.x * GRID_SIZE, (int)end.y * GRID_SIZE,
                      GRID_SIZE, GRID_SIZE, RED);
        DrawText("E", (int)end.x * GRID_SIZE + GRID_SIZE / 2 - 8,
                 (int)end.y * GRID_SIZE + GRID_SIZE / 2 - 10, 20, WHITE);

        // Highlight célula sob o mouse
        if (gridX >= 0 && gridX < gridWidth && gridY >= 0 && gridY < gridHeight)
        {
            Color highlightColor = {255, 255, 0, 80};
            DrawRectangle(gridX * GRID_SIZE, gridY * GRID_SIZE,
                          GRID_SIZE, GRID_SIZE, highlightColor);
        }

        // UI - Informações
        DrawRectangle(0, 0, 320, 220, Fade(BLACK, 0.7f));
        DrawText("PATHFINDING DEMO", 10, 10, 20, YELLOW);

        DrawText(TextFormat("Algoritmo: %s (A)",
                            currentAlgo == PATH_ASTAR ? "A*" : "Dijkstra"),
                 10, 40, 16, WHITE);

        if (currentAlgo == PATH_ASTAR)
        {
            const char *heurNames[] = {"Manhattan", "Euclidean", "Octile", "Chebyshev"};
            DrawText(TextFormat("Heuristica: %s (H)", heurNames[currentHeuristic]),
                     10, 60, 16, WHITE);
        }

        DrawText(TextFormat("Diagonal: %s (D)", allowDiagonal ? "ON" : "OFF"),
                 10, 80, 16, WHITE);
        DrawText(TextFormat("Caminho: %d nodes", (int)path.size()),
                 10, 100, 16, WHITE);

        DrawText("Click Esq: Parede", 10, 130, 14, LIGHTGRAY);
        DrawText("Click Dir: Apagar", 10, 145, 14, LIGHTGRAY);
        DrawText("S + Click: Mover inicio", 10, 160, 14, LIGHTGRAY);
        DrawText("E + Click: Mover fim", 10, 175, 14, LIGHTGRAY);
        DrawText("C: Limpar | G: Grid", 10, 190, 14, LIGHTGRAY);

        EndDrawing();
    }

    CloseWindow();
}

void teste_tile_iso()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tileset Demo");
    SetTargetFPS(60);

    InitScene();

    int nave = LoadGraph("nave", "assets/ball.png");
    int gr = LoadGraph("orb", "assets/Orb.png");
    int back = LoadGraph("bck1", "assets/FloorTexture.png");
    int tiles = LoadGraph("tiles", "assets/iso-test.png");

    for (int i = 0; i < 50; i++)
    {
        int x = GetRandomValue(50, SCREEN_WIDTH * 2);
        int y = GetRandomValue(50, SCREEN_HEIGHT * 2);
        Entity *e = CreateEntity(gr, 4, x, y);
        e->setStatic();
        e->setCircleShape(20);
        e->setCollisionLayer(1);                  // Enemy layer
        e->setCollisionMask((1 << 0) | (1 << 2)); // Colide com Player e Bullets
    }

    Entity *player = CreateEntity(nave, 4, 200, 200);
    // player->setRectangleShape(-40, -40, 60, 80);
    player->setCircleShape(10);

    // player->setCircleShape(20);
    player->setCollisionLayer(0);       // Player layer
    player->setCollisionMask((1 << 1)); // Só colide com Enemies

    SetLayerMode(0, LAYER_MODE_TILEX | LAYER_MODE_TILEY);

    SetLayerSize(-1, -SCREEN_WIDTH, -SCREEN_HEIGHT, SCREEN_WIDTH * 2, SCREEN_HEIGHT * 2);
    //   SetLayerBackGraph(0, back);

    InitCollision(0, 0, SCREEN_WIDTH * 2, SCREEN_HEIGHT * 2, nullptr);
    const int MAP_W = 64;
    const int MAP_H = 64;
    const int TILE_W = 64;
    const int TILE_H = 115;

    SetTileMap(0, MAP_W, MAP_H, TILE_W, TILE_H, 8, tiles);
    SetTileMapMode(0, 2);
    SetTileMapIsoCompression(0, 0.278125f);
    SetTileMapSpacing(0, 2);
    SetTileMapMargin(0, 2);

    uint16 mapData[MAP_W * MAP_H] = {
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3};

    for (int y = 0; y < MAP_H; y++)
    {
        for (int x = 0; x < MAP_W; x++)
        {

            uint16 id = mapData[y * MAP_W + x];

            if (id == 0)
                continue; // vazio
            SetTileMapTile(0, x, y, id);
        }
    }

    Vector2 velocity = {0, 0};

    while (!WindowShouldClose())
    {

        float dt = GetFrameTime();

        SetScroll(player->x - SCREEN_WIDTH / 2, player->y - SCREEN_HEIGHT / 2);

        BeginDrawing();

        ClearBackground(BLACK);

        Vector2 velocity = {0, 0};
        if (IsKeyDown(KEY_RIGHT))
            velocity.x = 300;
        if (IsKeyDown(KEY_LEFT))
            velocity.x = -300;
        if (IsKeyDown(KEY_DOWN))
            velocity.y = 300;
        if (IsKeyDown(KEY_UP))
            velocity.y = -300;

        player->move_and_slide(velocity, dt, {0, -1});

        RenderScene();

        EndDrawing();
    }

    DestroyScene();

    CloseWindow();
}

void teste_tileset_horto()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tileset horto Demo");
    SetTargetFPS(60);

    InitScene();

    int nave = LoadGraph("nave", "assets/ball.png");
    int gr = LoadGraph("orb", "assets/Orb.png");
    int back = LoadGraph("bck1", "assets/FloorTexture.png");
    int tiles = LoadGraph("tiles", "assets/tmw_desert_spacing.png");

    // SaveGraphics("assets/tmw_desert_spacing.pak");
    LoadGraphics("assets/tmw_desert_spacing.pak");

    for (int i = 0; i < 50; i++)
    {
        int x = GetRandomValue(50, SCREEN_WIDTH * 2);
        int y = GetRandomValue(50, SCREEN_HEIGHT * 2);
        Entity *e = CreateEntity(gr, 4, x, y);
        e->setStatic();
        e->setCircleShape(20);
        e->setCollisionLayer(1);                  // Enemy layer
        e->setCollisionMask((1 << 0) | (1 << 2)); // Colide com Player e Bullets
    }

    Entity *player = CreateEntity(nave, 4, 200, 400);
    // player->setRectangleShape(-8, -8, 8,8);
    player->setCircleShape(8);

    // player->setCircleShape(20);
    player->setCollisionLayer(0);       // Player layer
    player->setCollisionMask((1 << 1)); // Só colide com Enemies

    SetLayerMode(0, LAYER_MODE_TILEX | LAYER_MODE_TILEY);

    SetLayerSize(-1, -100, -100, SCREEN_WIDTH * 2, SCREEN_HEIGHT * 2);
    // SetLayerBackGraph(0, back);

    InitCollision(-100, -100, SCREEN_WIDTH * 2, SCREEN_HEIGHT * 2, nullptr);

    const int MAP_W = 32;
    const int MAP_H = 32;
    const int TILE_W = 32;
    const int TILE_H = 32;

    SetTileMap(0, MAP_W, MAP_H, TILE_W, TILE_H, 8, tiles);
    SetTileMapMargin(0, 1);
    SetTileMapSpacing(0, 1);
    SetTileMapMode(0, 0);

    uint16 mapData[MAP_W * MAP_H] = {
        36, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 43, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        43, 46, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        30, 30, 30, 30, 1, 2, 2, 2, 2, 2, 3, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        27, 30, 30, 30, 9, 10, 10, 10, 10, 10, 11, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        35, 30, 30, 30, 9, 10, 10, 10, 10, 10, 11, 30, 30, 30, 25, 26, 27, 25, 26, 26, 26, 26, 27, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        35, 30, 30, 30, 9, 10, 10, 10, 10, 10, 11, 30, 30, 30, 33, 34, 35, 33, 34, 34, 34, 34, 35, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        35, 30, 30, 30, 17, 18, 18, 18, 18, 18, 19, 30, 30, 30, 41, 42, 43, 41, 42, 42, 42, 42, 43, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        35, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 25, 26, 27, 30, 30, 30, 25, 26, 27, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        43, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 33, 34, 35, 30, 30, 30, 33, 34, 35, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 33, 34, 35, 30, 30, 30, 41, 42, 43, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 25, 45, 34, 44, 26, 26, 27, 25, 26, 27, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 33, 34, 34, 34, 34, 34, 35, 33, 34, 35, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 33, 36, 42, 42, 42, 42, 43, 41, 42, 43, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 41, 43, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30

    };

    for (int y = 0; y < MAP_H; y++)
    {
        for (int x = 0; x < MAP_W; x++)
        {

            uint16 id = mapData[y * MAP_W + x];

            if (id == 0)
                continue; // vazio

            if (id == 30)
                SetTileMapTile(0, x, y, id, 0);
            else
                SetTileMapTile(0, x, y, id, 1);
        }
    }

    Vector2 velocity = {0, 0};

    while (!WindowShouldClose())
    {

        float dt = GetFrameTime();

        SetScroll(player->x - SCREEN_WIDTH / 2, player->y - SCREEN_HEIGHT / 2);

        BeginDrawing();

        ClearBackground(BLACK);

        Vector2 velocity = {0, 0};
        if (IsKeyDown(KEY_RIGHT))
            velocity.x = 300;
        if (IsKeyDown(KEY_LEFT))
            velocity.x = -300;
        if (IsKeyDown(KEY_DOWN))
            velocity.y = 300;
        if (IsKeyDown(KEY_UP))
            velocity.y = -300;

        RenderScene();

        player->move_topdown(velocity, dt);
        //  player->collide_with_tiles(player->bounds);

        // player->move_and_slide(velocity, dt);

        EndDrawing();
    }

    DestroyScene();

    CloseWindow();
}

void teste_tileset_hexa()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tileset hexa Demo");
    SetTargetFPS(60);

    InitScene();

    int nave = LoadGraph("nave", "assets/ball.png");
    int gr = LoadGraph("orb", "assets/Orb.png");
    int back = LoadGraph("bck1", "assets/FloorTexture.png");
    int tiles = LoadGraph("tiles", "assets/hexa-tiles.png");

    for (int i = 0; i < 50; i++)
    {
        int x = GetRandomValue(50, SCREEN_WIDTH * 2);
        int y = GetRandomValue(50, SCREEN_HEIGHT * 2);
        Entity *e = CreateEntity(gr, 4, x, y);
        e->setStatic();
        e->setCircleShape(20);
        e->setCollisionLayer(1);                  // Enemy layer
        e->setCollisionMask((1 << 0) | (1 << 2)); // Colide com Player e Bullets
    }

    Entity *player = CreateEntity(nave, 4, 200, 200);
    // player->setRectangleShape(-40, -40, 60, 80);
    player->setCircleShape(10);

    // player->setCircleShape(20);
    player->setCollisionLayer(0);       // Player layer
    player->setCollisionMask((1 << 1)); // Só colide com Enemies

    SetLayerMode(0, LAYER_MODE_TILEX | LAYER_MODE_TILEY);

    SetLayerSize(-1, 0, 0, SCREEN_WIDTH * 2, SCREEN_HEIGHT * 2);
    // SetLayerBackGraph(0, back);

    InitCollision(0, 0, SCREEN_WIDTH * 2, SCREEN_HEIGHT * 2, nullptr);

    const int MAP_W = 5;
    const int MAP_H = 5;
    const int TILE_W = 175;
    const int TILE_H = 150;

    SetTileMap(0, MAP_W, MAP_H, TILE_W, TILE_H, 2, tiles);
    SetTileMapMode(0, 1);

    uint16 mapData[MAP_W * MAP_H] = {
        2, 6, 1, 2, 1,
        5, 2, 5, 2, 1,
        6, 5, 6, 2, 1,
        5, 5, 6, 6, 6,
        5, 5, 6, 6, 6

    };

    for (int y = 0; y < MAP_H; y++)
    {
        for (int x = 0; x < MAP_W; x++)
        {

            uint16 id = mapData[y * MAP_W + x];

            if (id == 0)
                continue; // vazio
        }
    }

    Vector2 velocity = {0, 0};

    while (!WindowShouldClose())
    {

        float dt = GetFrameTime();

        SetScroll(player->x - SCREEN_WIDTH / 2, player->y - SCREEN_HEIGHT / 2);

        BeginDrawing();

        ClearBackground(BLACK);

        Vector2 velocity = {0, 0};
        if (IsKeyDown(KEY_RIGHT))
            velocity.x = 300;
        if (IsKeyDown(KEY_LEFT))
            velocity.x = -300;
        if (IsKeyDown(KEY_DOWN))
            velocity.y = 300;
        if (IsKeyDown(KEY_UP))
            velocity.y = -300;

        player->move_and_slide(velocity, dt, {0, -1});

        RenderScene();

        EndDrawing();
    }

    DestroyScene();

    CloseWindow();
}

extern ParticleSystem gParticleSystem;

float randf()
{
    return (float)rand() / (float)RAND_MAX;
}

float randf(float min, float max)
{
    return min + randf() * (max - min);
}

class Firework
{
public:
    Vector2 pos;
    Vector2 vel;
    Color color;
    bool exploded;
    float life;
    Emitter *trail;

    Firework(Vector2 startPos, Color c, int graph)
    {
        pos = startPos;
        color = c;
        exploded = false;
        life = randf(1.5f, 3.0f); // Tempo até explodir

        // Velocidade para cima
        vel = {randf(-20.0f, 20.0f), randf(-400.0f, -500.0f)};

        // Trail que segue o foguete [web:26]
        trail = gParticleSystem.spawn(EMITTER_CONTINUOUS, graph, 100);
        trail->setPosition(pos.x, pos.y);
        trail->setDirection(0, 1); // Para baixo
        trail->setSpread(0.2f);
        trail->setEmissionRate(50.0f);
        trail->setSpeedRange(10.0f, 30.0f);
        trail->setLife(0.5f);
        trail->setColorCurve(ColorAlpha(color, 0.8f), ColorAlpha(color, 0.0f));
        trail->setSizeCurve(3.0f, 0.5f);
        trail->setGravity(0, 50.0f);
        trail->setBlendMode(BLEND_ADDITIVE);
    }

    void update(float dt)
    {
        if (exploded)
            return;

        life -= dt;

        // Física
        vel.y += 200.0f * dt; // Gravidade
        pos.x += vel.x * dt;
        pos.y += vel.y * dt;

        // Atualiza trail
        trail->setPosition(pos.x, pos.y);

        // Explode quando atinge apex ou tempo acaba
        if (vel.y >= 0 || life <= 0)
        {
            explode();
        }
    }

    void explode()
    {
        exploded = true;

        // Para o trail
        trail->stop();

        // Cria explosão principal [web:24]
        int numParticles = GetRandomValue(80, 150);
        Emitter *explosion = gParticleSystem.spawn(EMITTER_ONESHOT, 0, numParticles);
        explosion->setPosition(pos.x, pos.y);
        explosion->setSpread(2.0f * PI);
        explosion->setSpeedRange(100.0f, 250.0f);
        explosion->setLife(randf(1.0f, 1.5f));

        // Cores variadas
        Color explosionColor = color;
        explosion->setColorCurve(explosionColor, ColorAlpha(explosionColor, 0.0f));
        explosion->setSizeCurve(6.0f, 0.5f);
        explosion->setGravity(0, 150.0f);
        explosion->setDrag(0.3f);
        explosion->setBlendMode(BLEND_ADDITIVE);

        // Burst instantâneo
        explosion->burst(numParticles);
        explosion->stop();

        // Sub-explosão (trails das partículas principais) [web:26]
        Emitter *subTrails = gParticleSystem.spawn(EMITTER_ONESHOT, 0, numParticles * 3);
        subTrails->setPosition(pos.x, pos.y);
        subTrails->setSpread(2.0f * PI);
        subTrails->setSpeedRange(80.0f, 200.0f);
        subTrails->setLife(randf(0.5f, 1.0f));
        subTrails->setColorCurve(
            ColorAlpha(explosionColor, 0.6f),
            ColorAlpha(explosionColor, 0.0f));
        subTrails->setSizeCurve(3.0f, 0.0f);
        subTrails->setGravity(0, 200.0f);
        subTrails->setDrag(0.5f);
        subTrails->setBlendMode(BLEND_ADDITIVE);
        subTrails->burst(numParticles * 2);
        subTrails->stop();
    }

    void draw()
    {
        if (!exploded)
        {
            // Desenha o foguete subindo
            DrawCircleV(pos, 4, color);
        }
    }
};

void teste_particles()
{

    InitWindow(800, 600, "Particles");
    SetTargetFPS(60);

    InitScene();

    Emitter *e = gParticleSystem.spawn(EMITTER_CONTINUOUS, -1, 200);
    e->setPosition(100, 200);
    e->setDirection(0, -1);
    e->setSpread(0.5f);
    e->setEmissionRate(30.0f);
    e->setLife(2.0f);
    e->setSpeedRange(50.0f, 150.0f);
    e->setColorCurve(YELLOW, RED);
    e->setSizeCurve(5.0f, 1.0f);
    e->setGravity(0, 50.0f);
    e->setDrag(0.1f);
    e->setBlendMode(BLEND_ADDITIVE);

    Emitter *rain = gParticleSystem.spawn(EMITTER_CONTINUOUS, -1, 500);
    rain->setPosition(200, 200);
    rain->setSpawnZone(0, 0, 800, 10); // Chuva em toda largura
    rain->setDirection(0, 1);
    rain->setSpread(0.1f);
    rain->setEmissionRate(100.0f);
    rain->setSpeedRange(200.0f, 300.0f);
    rain->setLife(3.0f);
    rain->setColorCurve(ColorAlpha(SKYBLUE, 0.8f), ColorAlpha(BLUE, 0.3f));
    rain->setSizeCurve(2.0f, 1.0f);
    rain->setGravity(0, 100.0f);

    e->setPosition(100, 200);

    int particleGraph = 0;
    Emitter *mouseEmitter = nullptr;

    std::vector<Firework *> fireworks;
    float spawnTimer = 0;

    // Cores de fogos
    Color fireworkColors[] = {
        RED, ORANGE, YELLOW, GREEN, SKYBLUE, BLUE, PURPLE, PINK, GOLD};

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

         
        spawnTimer += dt;
        if (spawnTimer > randf(0.3f, 0.8f))
        {
            spawnTimer = 0;
            Vector2 startPos = {randf(100.0f, 700.0f), 600};
            Color color = fireworkColors[GetRandomValue(0, 8)];
            fireworks.push_back(new Firework(startPos, color, 0));
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            Vector2 mousePos = GetMousePosition();
            Vector2 startPos = {mousePos.x, 600};
            Color color = fireworkColors[GetRandomValue(0, 8)];
            fireworks.push_back(new Firework(startPos, color, 0));
        }

        for (int i = fireworks.size() - 1; i >= 0; i--)
        {
            fireworks[i]->update(dt);
            if (fireworks[i]->exploded)
            {
                delete fireworks[i];
                fireworks[i] = fireworks.back();
                fireworks.pop_back();
            }
        }

        gParticleSystem.update(dt);

        Vector2 mousePos = GetMousePosition();

        // Botão esquerdo: Fire particles
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            if (!mouseEmitter)
            {
                mouseEmitter = gParticleSystem.spawn(EMITTER_CONTINUOUS, particleGraph, 500);
                mouseEmitter->setSpread(2.0f * PI); // 360 graus
                mouseEmitter->setEmissionRate(100.0f);
                mouseEmitter->setSpeedRange(50.0f, 150.0f);
                mouseEmitter->setLife(1.0f);
                mouseEmitter->setColorCurve(ORANGE, ColorAlpha(RED, 0.0f));
                mouseEmitter->setSizeCurve(8.0f, 0.0f);
                mouseEmitter->setGravity(0, 100.0f);
                mouseEmitter->setBlendMode(BLEND_ADDITIVE);
            }
            // Segue o mouse
            mouseEmitter->setPosition(mousePos.x, mousePos.y);
        }
        else
        {
            if (mouseEmitter)
            {
                // mouseEmitter->stop();
                mouseEmitter = nullptr;
            }
        }

        // Botão direito: Explosão
        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
        {
            Emitter *explosion = gParticleSystem.spawn(EMITTER_ONESHOT, 0, 100);
            explosion->setPosition(mousePos.x, mousePos.y);
            explosion->setSpread(2.0f * PI);
            explosion->setEmissionRate(2000.0f);
            explosion->setSpeedRange(100.0f, 300.0f);
            explosion->setLife(0.8f);
            explosion->setLifeTime(0.01f);
            explosion->setColorCurve(YELLOW, ColorAlpha(RED, 0.0f));
            explosion->setSizeCurve(10.0f, 0.0f);
            explosion->setGravity(0, 200.0f);
            explosion->setDrag(0.5f);

            explosion->setBlendMode(BLEND_ADDITIVE);
        }

        // Botão do meio: Fumaça
        if (IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON))
        {
            Emitter *smoke = gParticleSystem.createSmoke(mousePos, particleGraph);
            smoke->setLifeTime(2.0f); // 2 segundos de fumaça
        }

        BeginDrawing();
        ClearBackground(BLACK);

        DrawRectangleGradientV(0, 0, 800, 600,
                               (Color){10, 10, 30, 255},
                               (Color){5, 5, 15, 255});
        for (auto *fw : fireworks)
        {
            fw->draw();
        }

        gParticleSystem.draw();

        DrawText("F - Toggle fountain", 10, 10, 16, WHITE);
        DrawText("SPACE - Explosion at mouse", 10, 40, 16, WHITE);

        EndDrawing();
    }

      for (auto* fw : fireworks) delete fw;
      gParticleSystem.clear();
    DestroyScene();

    CloseWindow();
}