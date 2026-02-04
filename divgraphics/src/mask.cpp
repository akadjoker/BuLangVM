#include "engine.hpp"
#include <cmath>
#include <algorithm>

// ================= OpenListHeap =================

OpenListHeap::OpenListHeap(int size)
{
    resize(size);
}

void OpenListHeap::resize(int size)
{
    pos.assign(size, -1);
    heap.clear();
    heap.reserve(size > 0 ? size : 128);
}

void OpenListHeap::setNodes(GridNode *g)
{
    nodes = g;
}

bool OpenListHeap::empty() const
{
    return heap.empty();
}

void OpenListHeap::clear()
{
    for (int idx : heap)
        pos[idx] = -1;
    heap.clear();
}

bool OpenListHeap::better(int a, int b) const
{
    float fa = nodes[a].f;
    float fb = nodes[b].f;
    if (fa < fb)
        return true;
    if (fa > fb)
        return false;
    return nodes[a].h < nodes[b].h;
}

void OpenListHeap::swapAt(int a, int b)
{
    int tmp = heap[a];
    heap[a] = heap[b];
    heap[b] = tmp;
    pos[heap[a]] = a;
    pos[heap[b]] = b;
}

void OpenListHeap::bubbleUp(int idx)
{
    while (idx > 0)
    {
        int parent = (idx - 1) >> 1;
        if (!better(heap[idx], heap[parent]))
            break;
        swapAt(parent, idx);
        idx = parent;
    }
}

void OpenListHeap::bubbleDown(int idx)
{
    int n = (int)heap.size();
    while (true)
    {
        int smallest = idx;
        int left = (idx << 1) + 1;
        int right = left + 1;

        if (left < n && better(heap[left], heap[smallest]))
            smallest = left;
        if (right < n && better(heap[right], heap[smallest]))
            smallest = right;

        if (smallest == idx)
            break;
        swapAt(idx, smallest);
        idx = smallest;
    }
}

void OpenListHeap::push(int idx)
{
    pos[idx] = (int)heap.size();
    heap.push_back(idx);
    bubbleUp((int)heap.size() - 1);
}

int OpenListHeap::pop()
{
    int result = heap[0];
    pos[result] = -1;

    if (heap.size() == 1)
    {
        heap.clear();
        return result;
    }

    heap[0] = heap.back();
    heap.pop_back();
    pos[heap[0]] = 0;
    bubbleDown(0);
    return result;
}

void OpenListHeap::update(int idx)
{
    int p = pos[idx];
    if (p < 0)
        return;
    bubbleUp(p);
    bubbleDown(pos[idx]);
}

bool OpenListHeap::contains(int idx) const
{
    return pos[idx] != -1;
}

// ================= Mask =================

const int Mask::dx[8] = {0, 1, 0, -1, -1, 1, 1, -1};
const int Mask::dy[8] = {-1, 0, 1, 0, -1, -1, 1, 1};

Mask::Mask(int w, int h, int res)
    : width(w), height(h), resolution(res), openList(w * h)
{
    grid = new GridNode[width * height];
    for (int i = 0; i < width * height; i++)
    {
        grid[i].walkable = 1;
        grid[i].f = grid[i].g = grid[i].h = 0.0f;
        grid[i].state = NS_UNVISITED;
        grid[i].parent_idx = -1;
    }
    openList.setNodes(grid);
}

Mask::~Mask()
{
    delete[] grid;
}

void Mask::setOccupied(int x, int y)
{
    if (x >= 0 && x < width && y >= 0 && y < height)
        grid[y * width + x].walkable = 0;
}

void Mask::setFree(int x, int y)
{
    if (x >= 0 && x < width && y >= 0 && y < height)
        grid[y * width + x].walkable = 1;
}

void Mask::clearAll()
{
    for (int i = 0; i < width * height; i++)
        grid[i].walkable = 1;
}

bool Mask::isOccupied(int x, int y) const
{
    if (x < 0 || x >= width || y < 0 || y >= height)
        return true;
    return grid[y * width + x].walkable == 0;
}

bool Mask::isWalkable(int x, int y) const
{
    if (x < 0 || x >= width || y < 0 || y >= height)
        return false;
    return grid[y * width + x].walkable == 1;
}

// world ↔ grid

Vector2 Mask::worldToGrid(Vector2 pos) const
{
    return {pos.x / (float)resolution, pos.y / (float)resolution};
}

Vector2 Mask::gridToWorld(Vector2 pos) const
{
    return {pos.x * (float)resolution, pos.y * (float)resolution};
}

Vector2 Mask::gridToWorldFloat(float gx, float gy) const
{
    return {gx * (float)resolution, gy * (float)resolution};
}

// heurísticas

float Mask::manhattan(int dx, int dy) const
{
    return (float)(dx + dy);
}

float Mask::euclidean(int dx, int dy) const
{
    return std::sqrt((float)(dx * dx + dy * dy));
}

float Mask::octile(int dx, int dy) const
{
    float F = std::sqrt(2.0f) - 1.0f;
    return (dx < dy) ? F * dx + dy : F * dy + dx;
}

float Mask::chebyshev(int dx, int dy) const
{
    return (float)((dx > dy) ? dx : dy);
}

float Mask::calcHeuristic(int dx, int dy, PathHeuristic heur, int diag) const
{
    switch (heur)
    {
    default:
    case PF_MANHATTAN:
        return diag ? octile(dx, dy) : manhattan(dx, dy);
    case PF_EUCLIDEAN:
        return euclidean(dx, dy);
    case PF_OCTILE:
        return octile(dx, dy);
    case PF_CHEBYSHEV:
        return chebyshev(dx, dy);
    }
}

bool Mask::isValidDiagonal(int cx, int cy, int nx, int ny) const
{
    if (nx != cx && ny != cy)
    {
        return isWalkable(cx, ny) && isWalkable(nx, cy);
    }
    return true;
}

void Mask::loadFromImage(const char *imagePath, int threshold)
{
    clearAll();

    Image img = LoadImage(imagePath);
    ImageResize(&img, width, height);

    Color *pixels = LoadImageColors(img);

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            Color c = pixels[y * width + x];
            int brightness = (c.r + c.g + c.b) / 3;
            if (brightness < threshold)
                setOccupied(x, y);
        }
    }

    UnloadImageColors(pixels);
    UnloadImage(img);
}

std::vector<Vector2> Mask::findPath(int sx, int sy, int ex, int ey,
                                    int diag,
                                    PathAlgorithm algo,
                                    PathHeuristic heur)
{
    std::vector<Vector2> path;

    // Reset nodes
    for (int i = 0; i < width * height; i++)
    {
        grid[i].f = grid[i].g = grid[i].h = 0.0f;
        grid[i].state = NS_UNVISITED;
        grid[i].parent_idx = -1;
    }
    openList.clear();

    // Validação
    if (!isWalkable(sx, sy) || !isWalkable(ex, ey))
        return path;

    int start_idx = sx + sy * width;
    int end_idx = ex + ey * width;

    // Inicializa nó inicial
    GridNode &start = grid[start_idx];
    start.g = 0.0f;

    if (algo == PATH_ASTAR)
    {
        int ddx = std::abs(sx - ex);
        int ddy = std::abs(sy - ey);
        start.h = calcHeuristic(ddx, ddy, heur, diag);
    }
    else
    {
        start.h = 0.0f;
    }

    start.f = start.g + start.h;
    start.parent_idx = -1;
    start.state = NS_OPEN;
    openList.push(start_idx);

    int dirs = diag ? 8 : 4;

    while (!openList.empty())
    {
        int curr_idx = openList.pop();
        GridNode *current = &grid[curr_idx];

        if (current->state == NS_CLOSED)
        {
            continue;
        }

        current->state = NS_CLOSED;

        if (curr_idx == end_idx)
        {
            int idx = end_idx;
            while (idx != -1)
            {
                int gx = idx % width;
                int gy = idx / width;
                path.push_back(gridToWorldFloat((float)gx, (float)gy));
                idx = grid[idx].parent_idx;
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        int cx = curr_idx % width;
        int cy = curr_idx / width;

        for (int i = 0; i < dirs; i++)
        {
            int nx = cx + dx[i];
            int ny = cy + dy[i];

            if (!isWalkable(nx, ny))
                continue;
            if (!isValidDiagonal(cx, cy, nx, ny))
                continue;

            int n_idx = nx + ny * width;
            GridNode *neighbor = &grid[n_idx];

            if (neighbor->state == NS_CLOSED)
                continue;

            float cost = ((nx != cx) && (ny != cy)) ? 1.41421356f : 1.0f;
            float ng = current->g + cost;

            if (neighbor->state == NS_UNVISITED || ng < neighbor->g)
            {
                neighbor->g = ng;
                neighbor->parent_idx = curr_idx;

                if (algo == PATH_ASTAR)
                {
                    int ddx = std::abs(nx - ex);
                    int ddy = std::abs(ny - ey);
                    neighbor->h = calcHeuristic(ddx, ddy, heur, diag);
                }
                else
                {
                    neighbor->h = 0.0f;
                }

                neighbor->f = neighbor->g + neighbor->h;

                if (neighbor->state == NS_UNVISITED)
                {
                    neighbor->state = NS_OPEN;
                    openList.push(n_idx);
                }
                else // NS_OPEN
                {
                    openList.update(n_idx);
                }
            }
        }
    }

    return path;
}
