#include "engine.hpp"
#include <raymath.h>
extern Scene gScene;

static inline float Dot2(Vector2 a, Vector2 b) { return a.x * b.x + a.y * b.y; }

static inline Vector2 Perp(Vector2 e) { return {-e.y, e.x}; }

static inline Vector2 Normalize(Vector2 v)
{
    float len = sqrtf(v.x * v.x + v.y * v.y);
    if (len < 1e-8f)
        return {1, 0};
    return {v.x / len, v.y / len};
}

static inline void Project(const Vector2 *pts, int n, Vector2 axis, float &outMin, float &outMax)
{
    outMin = FLT_MAX;
    outMax = -FLT_MAX;
    for (int i = 0; i < n; i++)
    {
        float p = Dot2(pts[i], axis);
        if (p < outMin)
            outMin = p;
        if (p > outMax)
            outMax = p;
    }
}

static inline void ProjectPoly(const Vector2 *pts, int n, Vector2 axis, float &outMin, float &outMax)
{
    outMin = FLT_MAX;
    outMax = -FLT_MAX;
    for (int i = 0; i < n; i++)
    {
        float p = Dot2(pts[i], axis);
        if (p < outMin)
            outMin = p;
        if (p > outMax)
            outMax = p;
    }
}

static inline bool getAABBCollisionInfo(const Rectangle &a, const Rectangle &b,
                                        Vector2 &normal, double &depth)
{
    // Centros
    float ax = a.x + a.width * 0.5f;
    float ay = a.y + a.height * 0.5f;
    float bx = b.x + b.width * 0.5f;
    float by = b.y + b.height * 0.5f;

    // Distância entre centros
    float dx = ax - bx;
    float dy = ay - by;

    // Overlap permitido
    float px = (a.width + b.width) * 0.5f - fabsf(dx);
    if (px <= 0)
        return false;

    float py = (a.height + b.height) * 0.5f - fabsf(dy);
    if (py <= 0)
        return false;

    // Escolher o eixo de menor penetração (MTV)
    if (px < py)
    {
        depth = px;
        normal = {dx < 0 ? -1.0f : 1.0f, 0.0f};
    }
    else
    {
        depth = py;
        normal = {0.0f, dy < 0 ? -1.0f : 1.0f};
    }

    return true;
}

static inline bool TestAxesFromPoly(const Vector2 *polyPts, int nPoly,
                                    const Vector2 *a, int na,
                                    const Vector2 *b, int nb,
                                    float &ioBestOverlap, Vector2 &ioBestAxis)
{
    for (int i = 0; i < nPoly; i++)
    {
        Vector2 p0 = polyPts[i];
        Vector2 p1 = polyPts[(i + 1) % nPoly];
        Vector2 edge = {p1.x - p0.x, p1.y - p0.y};
        Vector2 axis = Normalize(Perp(edge));

        float amin, amax, bmin, bmax;
        Project(a, na, axis, amin, amax);
        Project(b, nb, axis, bmin, bmax);

        float overlap = fminf(amax, bmax) - fmaxf(amin, bmin);
        if (overlap <= 0.0f)
            return false;

        if (overlap < ioBestOverlap)
        {
            ioBestOverlap = overlap;
            ioBestAxis = axis;
        }
    }
    return true;
}

static bool testAxis(Vector2 axis, Vector2 *pts1, int n1, Vector2 *pts2, int n2)
{
    float min1 = FLT_MAX, max1 = -FLT_MAX;
    float min2 = FLT_MAX, max2 = -FLT_MAX;

    // Projeta polígono 1
    for (int i = 0; i < n1; i++)
    {
        float proj = pts1[i].x * axis.x + pts1[i].y * axis.y;
        if (proj < min1)
            min1 = proj;
        if (proj > max1)
            max1 = proj;
    }

    // Projeta polígono 2
    for (int i = 0; i < n2; i++)
    {
        float proj = pts2[i].x * axis.x + pts2[i].y * axis.y;
        if (proj < min2)
            min2 = proj;
        if (proj > max2)
            max2 = proj;
    }

    // Retorna true se há overlap (colisão)
    return !(max1 < min2 || max2 < min1);
}

static void transformPoints(Vector2 *src, Vector2 *dst, int n, double x, double y, double angle, bool fx, bool fy)
{
    float rad = angle * M_PI / -180000.0f;
    float c = cosf(rad), s = sinf(rad);

    for (int i = 0; i < n; i++)
    {
        float px = src[i].x * (fx ? -1.0f : 1.0f);
        float py = src[i].y * (fy ? -1.0f : 1.0f);
        dst[i].x = x + px * c - py * s;
        dst[i].y = y + px * s + py * c;
    }
}

static inline void ProjectCircle(float cx, float cy, float r, Vector2 axis, float &outMin, float &outMax)
{
    float c = cx * axis.x + cy * axis.y;
    outMin = c - r;
    outMax = c + r;
}

static inline Vector2 ComputeCenter(const Vector2 *pts, int n)
{
    Vector2 c = {0, 0};
    for (int i = 0; i < n; i++)
    {
        c.x += pts[i].x;
        c.y += pts[i].y;
    }
    c.x /= n;
    c.y /= n;
    return c;
}

static bool SATCirclePoly(float cx, float cy, float r,
                          const Vector2 *polyPts, int n,
                          Vector2 &outAxis, float &outOverlap)
{
    outOverlap = FLT_MAX;
    outAxis = {1, 0};

    // 1) Eixos das arestas do polígono
    for (int i = 0; i < n; i++)
    {
        Vector2 p0 = polyPts[i];
        Vector2 p1 = polyPts[(i + 1) % n];
        Vector2 edge = {p1.x - p0.x, p1.y - p0.y};
        Vector2 axis = Normalize({-edge.y, edge.x});

        float cmin, cmax, pmin, pmax;
        ProjectCircle(cx, cy, r, axis, cmin, cmax);
        ProjectPoly(polyPts, n, axis, pmin, pmax);

        float overlap = fminf(cmax, pmax) - fmaxf(cmin, pmin);
        if (overlap <= 0.0f)
            return false;

        if (overlap < outOverlap)
        {
            outOverlap = overlap;
            outAxis = axis;
        }
    }

    // 2) Eixo do vértice mais próximo
    int closest = 0;
    float bestD2 = FLT_MAX;
    for (int i = 0; i < n; i++)
    {
        float dx = polyPts[i].x - cx;
        float dy = polyPts[i].y - cy;
        float d2 = dx * dx + dy * dy;
        if (d2 < bestD2)
        {
            bestD2 = d2;
            closest = i;
        }
    }

    Vector2 axisV = {cx - polyPts[closest].x, cy - polyPts[closest].y};
    float len = sqrtf(axisV.x * axisV.x + axisV.y * axisV.y);
    if (len > 1e-6f)
    {
        axisV.x /= len;
        axisV.y /= len;

        float cmin, cmax, pmin, pmax;
        ProjectCircle(cx, cy, r, axisV, cmin, cmax);
        ProjectPoly(polyPts, n, axisV, pmin, pmax);

        float overlap = fminf(cmax, pmax) - fmaxf(cmin, pmin);
        if (overlap <= 0.0f)
            return false;

        if (overlap < outOverlap)
        {
            outOverlap = overlap;
            outAxis = axisV;
        }
    }
    else
    {
        // centro do círculo em cima do vértice: já está a colidir
        // deixamos outAxis como estava (melhor eixo das arestas)
    }

    return true;
}

bool getSATCollisionInfo(Shape *s1, double x1, double y1, double a1, bool fx1, bool fy1,
                         Shape *s2, double x2, double y2, double a2, bool fx2, bool fy2,
                         Vector2 &out_normal, double &out_depth)
{
    if (!s1 || !s2)
        return false;

    // Circle vs Circle
    if (s1->type == CIRCLE && s2->type == CIRCLE)
    {
        CircleShape *c1 = (CircleShape *)s1;
        CircleShape *c2 = (CircleShape *)s2;

        double dx = x1 - x2, dy = y1 - y2;
        double dist = sqrt(dx * dx + dy * dy);
        double sum_r = c1->radius + c2->radius;

        if (dist >= sum_r)
            return false;

        if (dist < 0.0001)
        {
            out_normal = {1, 0};
        }
        else
        {
            out_normal = {(float)(dx / dist), (float)(dy / dist)};
        }
        out_depth = sum_r - dist;
        return true;
    }
    // Circle vs Polygon
    if (s1->type == CIRCLE && s2->type == POLYGON)
    {
        CircleShape *c = (CircleShape *)s1;
        PolygonShape *p = (PolygonShape *)s2;

        Vector2 tp[MAX_POINTS];
        transformPoints(p->points, tp, p->num_points, x2, y2, a2, fx2, fy2);

        Vector2 axis;
        float overlap;
        if (!SATCirclePoly((float)x1, (float)y1, (float)c->radius, tp, p->num_points, axis, overlap))
            return false;

        // orienta normal: de s2 -> s1 (poly -> circle)
        Vector2 polyCenter = ComputeCenter(tp, p->num_points);
        Vector2 dir = {(float)x1 - polyCenter.x, (float)y1 - polyCenter.y};
        if (Dot2(dir, axis) < 0.0f)
            axis = {-axis.x, -axis.y};

        out_normal = axis;
        out_depth = overlap;
        return true;
    }

    // Polygon vs Circle (inverte normal para manter s2->s1)
    if (s1->type == POLYGON && s2->type == CIRCLE)
    {
        // calcula colisão como Circle vs Poly, mas devolve normal invertida
        PolygonShape *p = (PolygonShape *)s1;
        CircleShape *c = (CircleShape *)s2;

        Vector2 tp[MAX_POINTS];
        transformPoints(p->points, tp, p->num_points, x1, y1, a1, fx1, fy1);

        Vector2 axis;
        float overlap;
        if (!SATCirclePoly((float)x2, (float)y2, (float)c->radius, tp, p->num_points, axis, overlap))
            return false;

        // axis aqui estaria orientado poly -> circle (s1 -> s2), mas queremos s2 -> s1
        // então inverte: circle -> poly
        axis = {-axis.x, -axis.y};

        out_normal = axis;
        out_depth = overlap;
        return true;
    }

    // Polygon vs Polygon
    if (s1->type == POLYGON && s2->type == POLYGON)
    {
        // Polygon vs Polygon (corrigido)
        if (s1->type == POLYGON && s2->type == POLYGON)
        {
            PolygonShape *p1 = (PolygonShape *)s1;
            PolygonShape *p2 = (PolygonShape *)s2;

            Vector2 t1[MAX_POINTS], t2[MAX_POINTS];
            transformPoints(p1->points, t1, p1->num_points, x1, y1, a1, fx1, fy1);
            transformPoints(p2->points, t2, p2->num_points, x2, y2, a2, fx2, fy2);

            float bestOverlap = FLT_MAX;
            Vector2 bestAxis = {1, 0};

            // testa eixos das arestas de t1 e de t2
            if (!TestAxesFromPoly(t1, p1->num_points, t1, p1->num_points, t2, p2->num_points, bestOverlap, bestAxis))
                return false;
            if (!TestAxesFromPoly(t2, p2->num_points, t1, p1->num_points, t2, p2->num_points, bestOverlap, bestAxis))
                return false;

            // orienta normal: de s2 -> s1
            Vector2 c1 = ComputeCenter(t1, p1->num_points);
            Vector2 c2 = ComputeCenter(t2, p2->num_points);
            Vector2 dir = {c1.x - c2.x, c1.y - c2.y};
            if (Dot2(dir, bestAxis) < 0.0f)
                bestAxis = {-bestAxis.x, -bestAxis.y};

            out_normal = bestAxis;
            out_depth = bestOverlap;
            return true;
        }
    }

    return false;
}

void PolygonShape::calcNormals()
{
    for (int i = 0; i < num_points; i++)
    {
        Vector2 p1 = points[i];
        Vector2 p2 = points[(i + 1) % num_points];
        float dx = p2.x - p1.x;
        float dy = p2.y - p1.y;
        float len = sqrtf(dx * dx + dy * dy);
        normals[i] = (len > 0.0001f) ? Vector2{-dy / len, dx / len} : Vector2{0, 1};
    }
}

void CircleShape::draw(const Entity *entity, Color color)
{
    double screenX = entity->x;
    double screenY = entity->y;

    if (entity->resolution > 0)
    {
        screenX /= entity->resolution;
        screenY /= entity->resolution;
    }
    else if (entity->resolution < 0)
    {
        screenX *= -entity->resolution;
        screenY *= -entity->resolution;
    }

    Layer &l = gScene.layers[entity->layer];
    screenX -= l.scroll_x;
    screenY -= l.scroll_y;

    DrawCircleLines((int)screenX, (int)screenY, radius, color);

    // Linha de direção
    float rad = entity->angle * M_PI / -180000.0f;
    float endX = screenX + cosf(rad) * radius;
    float endY = screenY - sinf(rad) * radius;
    DrawLine((int)screenX, (int)screenY, (int)endX, (int)endY, RED);
}

void PolygonShape::draw(const Entity *entity, Color color)
{
    double screenX = entity->x;
    double screenY = entity->y;

    if (entity->resolution > 0)
    {
        screenX /= entity->resolution;
        screenY /= entity->resolution;
    }
    else if (entity->resolution < 0)
    {
        screenX *= -entity->resolution;
        screenY *= -entity->resolution;
    }

    Layer &l = gScene.layers[entity->layer];
    screenX -= l.scroll_x;
    screenY -= l.scroll_y;

    Vector2 transformed[MAX_POINTS];
    transformPoints(points, transformed, num_points,
                    screenX, screenY, entity->angle,
                    entity->flags & B_HMIRROR, entity->flags & B_VMIRROR);

    // Desenha arestas
    for (int i = 0; i < num_points; i++)
    {
        int next = (i + 1) % num_points;
        DrawLine((int)transformed[i].x, (int)transformed[i].y,
                 (int)transformed[next].x, (int)transformed[next].y, color);
    }

    // Desenha vértices
    for (int i = 0; i < num_points; i++)
    {
        DrawCircle((int)transformed[i].x, (int)transformed[i].y, 3, Fade(WHITE, 0.5f));
    }
}

// Circle vs Polygon SAT
static bool testCirclePolygon(CircleShape *c, double cx, double cy,
                              PolygonShape *p, Vector2 *pts, int n)
{
    // 1. Testa normais do polígono
    for (int i = 0; i < n; i++)
    {
        Vector2 axis = p->normals[i];

        // Projeta círculo
        float center_proj = (float)cx * axis.x + (float)cy * axis.y;
        float c_min = center_proj - c->radius;
        float c_max = center_proj + c->radius;

        // Projeta polígono
        float p_min = FLT_MAX, p_max = -FLT_MAX;
        for (int j = 0; j < n; j++)
        {
            float proj = pts[j].x * axis.x + pts[j].y * axis.y;
            if (proj < p_min)
                p_min = proj;
            if (proj > p_max)
                p_max = proj;
        }

        // Se não há overlap, não colidem
        if (c_max < p_min || p_max < c_min)
            return false;
    }

    // 2. Eixo do vértice mais próximo ao círculo
    float closest_dist_sq = FLT_MAX;
    int closest_idx = 0;

    for (int i = 0; i < n; i++)
    {
        float dx = pts[i].x - (float)cx;
        float dy = pts[i].y - (float)cy;
        float dist_sq = dx * dx + dy * dy;

        if (dist_sq < closest_dist_sq)
        {
            closest_dist_sq = dist_sq;
            closest_idx = i;
        }
    }

    // Eixo do centro do círculo ao vértice mais próximo
    float dx = (float)cx - pts[closest_idx].x;
    float dy = (float)cy - pts[closest_idx].y;
    float len = sqrtf(dx * dx + dy * dy);

    if (len < 0.0001f)
        return true; // Círculo no vértice

    Vector2 axis = {dx / len, dy / len};

    // Projeta círculo
    float center_proj = (float)cx * axis.x + (float)cy * axis.y;
    float c_min = center_proj - c->radius;
    float c_max = center_proj + c->radius;

    // Projeta polígono
    float p_min = FLT_MAX, p_max = -FLT_MAX;
    for (int j = 0; j < n; j++)
    {
        float proj = pts[j].x * axis.x + pts[j].y * axis.y;
        if (proj < p_min)
            p_min = proj;
        if (proj > p_max)
            p_max = proj;
    }

    return !(c_max < p_min || p_max < c_min);
}

static bool checkCollision(Shape *s1, double x1, double y1, double a1, bool fx1, bool fy1,
                           Shape *s2, double x2, double y2, double a2, bool fx2, bool fy2)
{
    if (!s1 || !s2)
        return false;

    // Circle vs Circle
    if (s1->type == CIRCLE && s2->type == CIRCLE)
    {
        float dx = x1 - x2, dy = y1 - y2;
        float r = ((CircleShape *)s1)->radius + ((CircleShape *)s2)->radius;
        return (dx * dx + dy * dy) < r * r;
    }

    // Circle vs Polygon
    if (s1->type == CIRCLE && s2->type == POLYGON)
    {
        CircleShape *c = (CircleShape *)s1;
        PolygonShape *p = (PolygonShape *)s2;
        Vector2 t[MAX_POINTS];

        transformPoints(p->points, t, p->num_points, x2, y2, a2, fx2, fy2);
        return testCirclePolygon(c, x1, y1, p, t, p->num_points);
    }

    // Polygon vs Circle (inverte)
    if (s1->type == POLYGON && s2->type == CIRCLE)
    {
        PolygonShape *p = (PolygonShape *)s1;
        CircleShape *c = (CircleShape *)s2;
        Vector2 t[MAX_POINTS];

        transformPoints(p->points, t, p->num_points, x1, y1, a1, fx1, fy1);
        return testCirclePolygon(c, x2, y2, p, t, p->num_points);
    }

    // Polygon vs Polygon
    if (s1->type == POLYGON && s2->type == POLYGON)
    {
        PolygonShape *p1 = (PolygonShape *)s1, *p2 = (PolygonShape *)s2;
        Vector2 t1[MAX_POINTS], t2[MAX_POINTS];

        transformPoints(p1->points, t1, p1->num_points, x1, y1, a1, fx1, fy1);
        transformPoints(p2->points, t2, p2->num_points, x2, y2, a2, fx2, fy2);

        for (int i = 0; i < p1->num_points; i++)
            if (!testAxis(p1->normals[i], t1, p1->num_points, t2, p2->num_points))
                return false;

        for (int i = 0; i < p2->num_points; i++)
            if (!testAxis(p2->normals[i], t1, p1->num_points, t2, p2->num_points))
                return false;

        return true;
    }

    return false;
}

bool Entity::intersects(const Entity *other) const
{
    if (!shape || !other->shape)
        return false;
    if (!(flags & B_COLLISION) || !(other->flags & B_COLLISION))
        return false;
    if ((flags & B_DEAD) || (other->flags & B_DEAD))
        return false;

    // Converte para screen space
    double x1 = x, y1 = y;
    double x2 = other->x, y2 = other->y;

    if (resolution > 0)
    {
        x1 /= resolution;
        y1 /= resolution;
    }
    else if (resolution < 0)
    {
        x1 *= -resolution;
        y1 *= -resolution;
    }

    if (other->resolution > 0)
    {
        x2 /= other->resolution;
        y2 /= other->resolution;
    }
    else if (other->resolution < 0)
    {
        x2 *= -other->resolution;
        y2 *= -other->resolution;
    }

    // Usa a função checkCollision diretamente
    return checkCollision(shape, x1, y1, angle,
                          flags & B_HMIRROR, flags & B_VMIRROR,
                          other->shape, x2, y2, other->angle,
                          other->flags & B_HMIRROR, other->flags & B_VMIRROR);
}
void Entity::updateBounds()
{
    if (!shape)
    {
        bounds = {(float)x, (float)y, 1, 1};
        bounds_dirty = false;
        return;
    }

    // Converte coordenadas para screen space
    double screenX = x, screenY = y;

    if (resolution > 0)
    {
        screenX /= resolution;
        screenY /= resolution;
    }
    else if (resolution < 0)
    {
        screenX *= -resolution;
        screenY *= -resolution;
    }

    if (shape->type == CIRCLE)
    {
        CircleShape *c = (CircleShape *)shape;
        float r = c->radius;
        bounds = {(float)(screenX - r), (float)(screenY - r), r * 2, r * 2};
    }
    else // POLYGON
    {
        PolygonShape *p = (PolygonShape *)shape;
        Vector2 t[MAX_POINTS];

        transformPoints(p->points, t, p->num_points,
                        screenX, screenY, angle,
                        flags & B_HMIRROR, flags & B_VMIRROR);

        float minX = FLT_MAX, maxX = -FLT_MAX;
        float minY = FLT_MAX, maxY = -FLT_MAX;

        for (int i = 0; i < p->num_points; i++)
        {
            if (t[i].x < minX)
                minX = t[i].x;
            if (t[i].x > maxX)
                maxX = t[i].x;
            if (t[i].y < minY)
                minY = t[i].y;
            if (t[i].y > maxY)
                maxY = t[i].y;
        }

        bounds = {minX, minY, maxX - minX, maxY - minY};
    }

    bounds_dirty = false;
}

RectangleShape::RectangleShape(int x, int y, int w, int h) : PolygonShape(4)
{
    points[0].x = x;
    points[0].y = y;
    points[1].x = x + w;
    points[1].y = y;
    points[2].x = x + w;
    points[2].y = y + h;
    points[3].x = x;
    points[3].y = y + h;
}

bool Shape::collide(Shape *other, double x1, double y1, double a1, bool fx1, bool fy1, double x2, double y2, double a2, bool fx2, bool fy2)
{

    return checkCollision(this, x1, y1, a1, fx1, fy1, other, x2, y2, a2, fx2, fy2);
}
 
bool Entity::collide_with_tiles(const Rectangle& box)
{
    for (size_t layer = 0; layer < MAX_LAYERS; layer++)
    {
        Tilemap* tm = gScene.layers[layer].tilemap;
        if (!tm) continue;

        int gx0, gy0, gx1, gy1;
        tm->worldToGrid({box.x, box.y}, gx0, gy0);
        tm->worldToGrid({box.x + box.width, box.y + box.height}, gx1, gy1);

         gx0 = std::max(0, gx0 - 1);
        gy0 = std::max(0, gy0 - 1);
        gx1 = std::min(tm->width - 1, gx1 + 1);    
        gy1 = std::min(tm->height - 1, gy1 + 1);  
        
        for (int gy = gy0; gy <= gy1; gy++) 
            for (int gx = gx0; gx <= gx1; gx++)
         {
            Tile* t = tm->getTile(gx, gy);
            if (!t || !t->solid) continue;

            Vector2 wp = tm->gridToWorld(gx, gy);

            Rectangle tileRect = {
                wp.x,
                wp.y,
                (float)tm->tilewidth,
                (float)tm->tileheight
            };

           //  DrawRectangleLinesEx(tileRect, 1, RED);

            if (CheckCollisionRecs(box, tileRect))
                return true;
        }
    }

    return false;
}

// bool Entity::collide_with_tiles(const Rectangle &box)
// {

//     Rectangle points[9];

//     for (size_t layer = 0; layer < MAX_LAYERS; layer++)
//     {
//         Tilemap *tm = gScene.layers[layer].tilemap;
//         if (!tm)
//             continue;

//         Layer &l = gScene.layers[layer];
//         float scroll_x = l.scroll_x;
//         float scroll_y = l.scroll_y;

//         // ===== GET CENTER GRID =====
//         int center_grid_x = (int)(box.x / tm->tilewidth);
//         int center_grid_y = (int)(box.y / tm->tileheight);

//         // ===== GENERATE 9 TILES =====
//         int idx = 0;
//         for (int dy = -1; dy <= 1; dy++)
//         {
//             for (int dx = -1; dx <= 1; dx++)
//             {
//                 int grid_x = center_grid_x + dx;
//                 int grid_y = center_grid_y + dy;

//                 // Convert to world coords
//                 float world_x = grid_x * tm->tilewidth;
//                 float world_y = grid_y * tm->tileheight;

//                 // Convert to screen coords
//                 float screen_x = world_x - scroll_x;
//                 float screen_y = world_y - scroll_y;

//                 // Store rectangle
//                 points[idx] =
//                     {
//                         screen_x,
//                         screen_y,
//                         (float)tm->tilewidth,
//                         (float)tm->tileheight};

//                 // Draw
//                 DrawRectangleLinesEx(points[idx], 1, RED);
              

//                 idx++;
//             }
//         }
//         for (int i = 0; i < 9; i++)
//         {
//             Tile *t = tm->getTile(center_grid_x + (i % 3) - 1, center_grid_y + (i / 3) - 1);

//             if (t && t->solid)
//             {
//                 // Colidiu com tile [i]
//                 if (CheckCollisionRecs(box, points[i]))
//                 {
//                     return true;
//                 }
//             }
//         }

        
//     }

//     return false;
// }

    void Entity::move_topdown(Vector2 velocity, float dt)
    {
        // converte velocidade (px/s) para deslocamento (px/frame)
        float dx = velocity.x * dt;
        float dy = velocity.y * dt;

        int stepsX = (int)fabs(dx);
        int stepsY = (int)fabs(dy);

        int sx = (dx > 0) - (dx < 0);
        int sy = (dy > 0) - (dy < 0);

        // mover no eixo X, pixel a pixel
        for (int i = 0; i < stepsX; i++)
        {
            if (place_free(x + sx, y))
                x += sx;
            else
                break;
        }

        // mover no eixo Y, pixel a pixel
        for (int i = 0; i < stepsY; i++)
        {
            if (place_free(x, y + sy))
                y += sy;
            else
                break;
        }

        bounds_dirty = true;
    }

    bool Entity::place_free(double x, double y)
    {
        if (!shape || !gScene.staticTree)
            return true;
        if (!(flags & B_COLLISION))
            return true;

        // Salva posição original

        // Move para a nova posicao
        double old_x = this->x, old_y = this->y;
        this->x = x;
        this->y = y;
        updateBounds();

        // 1) Testar colisão com tiles
        if (collide_with_tiles(bounds))
        {
            this->x = old_x;
            this->y = old_y;
            bounds_dirty = true;
            return false;
        }

        // 2) Testar colisão com entidades

        // Query na quadtree
        std::vector<Entity *> nearby;

        gScene.staticTree->query(getBounds(), nearby);

        // Adiciona dinâmicas também
        for (Entity *dyn : gScene.dynamicEntities)
        {
            if (dyn != this)
                nearby.push_back(dyn);
        }

        bool free = true;

        // Testa colisão com cada uma
        for (Entity *other : nearby)
        {
            if (other == this)
                continue;
            if (!other->shape)
                continue;
            if (!(other->flags & B_COLLISION))
                continue;
            if (other->flags & B_DEAD)
                continue;

            if (!this->canCollideWith(other))
                continue;

            if (collide(other))
            {
                free = false;
                break;
            }
        }

        // Restaura posição
        this->x = old_x;
        this->y = old_y;
        this->bounds_dirty = true;

        return free;
    }

    Entity *Entity::place_meeting(double x, double y)
    {
        if (!this->shape || !gScene.staticTree)
            return nullptr;
        if (!(this->flags & B_COLLISION))
            return nullptr;

        double old_x = this->x, old_y = this->y;
        this->x = x;
        this->y = y;
        this->updateBounds();
        Entity *hit = nullptr;

        // 1) Testar colisão com tiles
        if (collide_with_tiles(bounds))
        {
            this->x = old_x;
            this->y = old_y;
            bounds_dirty = true;
            return hit;
        }

        // 2) Testar colisão com entidades

        std::vector<Entity *> nearby;
        gScene.staticTree->query(this->getBounds(), nearby);
        for (Entity *dyn : gScene.dynamicEntities)
        {
            if (dyn != this)
                nearby.push_back(dyn);
        }

        for (Entity *other : nearby)
        {
            if (other == this)
                continue;
            if (!other->shape)
                continue;
            if (!(other->flags & B_COLLISION))
                continue;
            if (other->flags & B_DEAD)
                continue;

            if (this->collide(other))
            {
                hit = other;
                break;
            }
        }

        this->x = old_x;
        this->y = old_y;
        this->bounds_dirty = true;

        return hit;
    }
    bool Entity::move_and_collide(double vel_x, double vel_y, CollisionInfo *result)
    {
        if (!shape || !(flags & B_COLLISION) || !gScene.staticTree)
        {
            x += vel_x;
            y += vel_y;
            bounds_dirty = true;
            return false;
        }

        const float SKIN = 0.05f;

        // bounds antes do move (para broadphase)
        updateBounds();
        Rectangle startBounds = bounds;

        // sweep AABB
        Rectangle moveBounds = startBounds;
        if (vel_x < 0)
            moveBounds.x += (float)vel_x;
        if (vel_y < 0)
            moveBounds.y += (float)vel_y;
        moveBounds.width += (float)fabs(vel_x);
        moveBounds.height += (float)fabs(vel_y);

        moveBounds.x -= 2;
        moveBounds.y -= 2;
        moveBounds.width += 4;
        moveBounds.height += 4;

        // candidatos 1x
        std::vector<Entity *> nearby;
        nearby.reserve(64);
        gScene.staticTree->query(moveBounds, nearby);

        for (Entity *dyn : gScene.dynamicEntities)
        {
            if (!dyn || dyn == this)
                continue;
            if (!dyn->shape || !(dyn->flags & B_COLLISION))
                continue;
            if (dyn->flags & B_DEAD)
                continue;

            if (dyn->bounds_dirty)
                dyn->updateBounds();
            if (CheckCollisionRecs(moveBounds, dyn->bounds))
                nearby.push_back(dyn);
        }

        // move primeiro (como queres)
        x += vel_x;
        y += vel_y;
        updateBounds();

        auto CenterOf = [](const Rectangle &r) -> Vector2
        {
            return {r.x + r.width * 0.5f, r.y + r.height * 0.5f};
        };

        // escolhe melhor colisão (MTV = menor overlap)
        bool hit = false;
        double bestDepth = DBL_MAX;
        Vector2 bestNormal = {0, 0};
        Entity *bestOther = nullptr;

        for (Entity *other : nearby)
        {
            if (!other || other == this)
                continue;
            if (!other->shape || !(other->flags & B_COLLISION))
                continue;
            if (other->flags & B_DEAD)
                continue;
            if (!canCollideWith(other))
                continue;

            Vector2 n = {0, 0};
            double d = 0.0;

            if (getSATCollisionInfo(shape, x, y, angle, flags & B_HMIRROR, flags & B_VMIRROR,
                                    other->shape, other->x, other->y, other->angle,
                                    other->flags & B_HMIRROR, other->flags & B_VMIRROR,
                                    n, d))
            {
                if (d <= 0.0)
                    continue;

                if (!hit || d < bestDepth)
                {
                    hit = true;
                    bestDepth = d;
                    bestNormal = n;
                    bestOther = other;
                }
            }
        }

        if (!hit)
        {
            bounds_dirty = true;
            return false;
        }

        // orienta normal por centros dos bounds
        Vector2 cThis = CenterOf(bounds);
        if (bestOther->bounds_dirty)
            bestOther->updateBounds();
        Vector2 cOther = CenterOf(bestOther->bounds);

        Vector2 dir = {cThis.x - cOther.x, cThis.y - cOther.y};
        if (dir.x * bestNormal.x + dir.y * bestNormal.y < 0.0f)
            bestNormal = {-bestNormal.x, -bestNormal.y};

        // push out + skin
        x += bestNormal.x * (bestDepth + SKIN);
        y += bestNormal.y * (bestDepth + SKIN);
        updateBounds();

        if (result)
        {
            result->collider = bestOther;
            result->normal = bestNormal;
            result->depth = bestDepth;
        }

        return true;
    }
    bool Entity::snap_to_floor(float snap_len, Vector2 up_direction, Vector2 &velocity)
    {
        // Não snap se estás a saltar (movimento contra gravidade)
        float vel_dot_up = velocity.x * up_direction.x + velocity.y * up_direction.y;
        if (vel_dot_up < 0.0f)
            return false; // subindo (contra up)

        // Guarda posição (caso não seja chão)
        double old_x = x, old_y = y;

        CollisionInfo col;
        // Tenta snap na direção da gravidade (oposto de up)
        float snap_x = -up_direction.x * snap_len;
        float snap_y = -up_direction.y * snap_len;

        if (move_and_collide(snap_x, snap_y, &col))
        {
            float dotUp = col.normal.x * up_direction.x + col.normal.y * up_direction.y;

            // Só aceita se for "chão" (ângulo < ~45°)
            if (dotUp > 0.7f)
            {
                on_floor = true;
                on_wall = false;
                on_ceiling = false;

                // Zera componente que vai na direção da gravidade
                if (vel_dot_up > 0.0f)
                {
                    velocity.x -= up_direction.x * vel_dot_up;
                    velocity.y -= up_direction.y * vel_dot_up;
                }

                return true;
            }
        }

        // Não era chão -> reverte
        x = old_x;
        y = old_y;
        bounds_dirty = true;
        return false;
    }

    bool Entity::move_and_slide(Vector2 & velocity, float delta, Vector2 up_direction)
    {
        on_floor = on_wall = on_ceiling = false;

        // Sem colisão/shape: move livre
        if (!shape || !(flags & B_COLLISION))
        {
            x += velocity.x * delta;
            y += velocity.y * delta;
            bounds_dirty = true;
            return false;
        }

        const int MAX_SLIDES = 4;
        const float SKIN = 0.05f; // ajusta: 0.05~0.1 em mundo "pixel"

        // Motion do frame
        Vector2 motion = {velocity.x * delta, velocity.y * delta};

        // Atualiza bounds da entidade
        updateBounds();

        // Sweep AABB (broadphase)
        Rectangle moveBounds = bounds;
        if (motion.x < 0)
            moveBounds.x += motion.x;
        if (motion.y < 0)
            moveBounds.y += motion.y;
        moveBounds.width += (float)fabs(motion.x);
        moveBounds.height += (float)fabs(motion.y);

        // margem por precisão/skin
        moveBounds.x -= 2;
        moveBounds.y -= 2;
        moveBounds.width += 4;
        moveBounds.height += 4;

        std::vector<Entity *> nearby;
        nearby.reserve(64);
        gScene.staticTree->query(moveBounds, nearby);

        for (Entity *dyn : gScene.dynamicEntities)
        {
            if (!dyn || dyn == this)
                continue;
            if (!dyn->shape || !(dyn->flags & B_COLLISION))
                continue;
            if (dyn->flags & B_DEAD)
                continue;

            if (dyn->bounds_dirty)
                dyn->updateBounds();
            if (CheckCollisionRecs(moveBounds, dyn->bounds))

                nearby.push_back(dyn);
        }

        auto CenterOf = [](const Rectangle &r) -> Vector2
        {
            return {r.x + r.width * 0.5f, r.y + r.height * 0.5f};
        };

        // Resolve até MAX_SLIDES
        for (int slide = 0; slide < MAX_SLIDES; slide++)
        {
            if (motion.x * motion.x + motion.y * motion.y < 1e-10f)
                break;

            // tenta mover tudo
            double old_x = x, old_y = y;
            x += motion.x;
            y += motion.y;
            updateBounds();

            // encontra melhor colisão (MTV = menor overlap)
            bool hit = false;
            double bestDepth = DBL_MAX;
            Vector2 bestNormal = {0, 0};
            Entity *bestOther = nullptr;

            for (Entity *other : nearby)
            {
                if (!other || other == this)
                    continue;
                if (!other->shape || !(other->flags & B_COLLISION))
                    continue;
                if (other->flags & B_DEAD)
                    continue;
                if (!canCollideWith(other))
                    continue;

                Vector2 n = {0, 0};
                double d = 0.0;

                if (getSATCollisionInfo(shape, x, y, angle, flags & B_HMIRROR, flags & B_VMIRROR,
                                        other->shape, other->x, other->y, other->angle,
                                        other->flags & B_HMIRROR, other->flags & B_VMIRROR,
                                        n, d))
                {
                    if (d <= 0.0)
                        continue;
                    if (!hit || d < bestDepth)
                    {
                        hit = true;
                        bestDepth = d;
                        bestNormal = n;
                        bestOther = other;
                    }
                }
            }

            if (!hit)
                break;

            // orienta normal usando centros dos bounds (robusto)

            Vector2 cThis = CenterOf(bounds);
            if (bestOther->bounds_dirty)
                bestOther->updateBounds();
            Vector2 cOther = CenterOf(bestOther->bounds);

            Vector2 dir = {cThis.x - cOther.x, cThis.y - cOther.y};
            float dotDir = dir.x * bestNormal.x + dir.y * bestNormal.y;
            if (dotDir < 0.0f)
            {
                bestNormal.x = -bestNormal.x;
                bestNormal.y = -bestNormal.y;
            }

            // push out + skin
            x += bestNormal.x * (bestDepth + SKIN);
            y += bestNormal.y * (bestDepth + SKIN);
            updateBounds();

            // flags por dot com up
            float dotUp = bestNormal.x * up_direction.x + bestNormal.y * up_direction.y;
            if (dotUp > 0.7f)
                on_floor = true;
            else if (dotUp < -0.7f)
                on_ceiling = true;
            else
                on_wall = true;

            // travel + remainder
            Vector2 travel = {(float)(x - old_x), (float)(y - old_y)};
            Vector2 remainder = {motion.x - travel.x, motion.y - travel.y};

            // slide remainder (remove componente na normal)
            float dotR = remainder.x * bestNormal.x + remainder.y * bestNormal.y;
            motion.x = remainder.x - bestNormal.x * dotR;
            motion.y = remainder.y - bestNormal.y * dotR;

            // slide velocity (px/s)
            float dotV = velocity.x * bestNormal.x + velocity.y * bestNormal.y;
            velocity.x = velocity.x - bestNormal.x * dotV;
            velocity.y = velocity.y - bestNormal.y * dotV;
        }

        bounds_dirty = true;
        return (on_floor || on_wall || on_ceiling);
    }

    void Scene::initCollision(Rectangle worldBounds)
    {
        staticTree = new Quadtree(worldBounds);
        updateCollision();
    }

    void Scene::updateCollision()
    {

        staticTree->clear();
        staticEntities.clear();
        dynamicEntities.clear();

        for (int l = 0; l < MAX_LAYERS; l++)
        {
            Layer &layer = layers[l];
            for (size_t i = 0; i < layer.nodes.size(); i++)
            {
                Entity *e = layer.nodes[i];
                if (!e->shape)
                    continue;
                // Ignora entities mortas, congeladas ou sem collision
                if (e->flags & B_DEAD)
                {
                    removeEntity(e);
                    continue;
                }
                if (!(e->flags & B_COLLISION))
                    continue;
                if (e->flags & B_FROZEN)
                    continue;

                e->updateBounds();
                if (e->flags & B_STATIC)
                {
                    staticTree->insert(e);
                    staticEntities.push_back(e);
                }
                else
                {
                    dynamicEntities.push_back(e);
                }
            }
        }
        if (!onCollision)
            return;

        checkCollisions();
    }

    void Scene::checkCollisions()
    {
        if (!onCollision)
            return;

        for (size_t i = 0; i < dynamicEntities.size(); i++)
        {
            Entity *dynamic = dynamicEntities[i];
            if (!dynamic || !dynamic->shape)
                continue;

            dynamic->updateBounds();

            std::vector<Entity *> candidates;
            staticTree->query(dynamic->bounds, candidates);

            // Layer &l = layers[dynamic->layer];
            // int screenX = dynamic->bounds.x;
            // int screenY = dynamic->bounds.y;
            // screenX -= l.scroll_x;
            // screenY -= l.scroll_y;

            // DrawRectangleLines(screenX, screenY  , dynamic->bounds.width, dynamic->bounds.height, MAGENTA);

            for (size_t j = 0; j < candidates.size(); j++)
            {

                Entity *other = candidates[j];

                if (!other || !other->shape)
                    continue;

                if (dynamic == other)
                    continue;

                if (!dynamic->canCollideWith(other) && !other->canCollideWith(dynamic))
                    continue;

                if (dynamic->collide(other))
                {
                    onCollision(dynamic, other, collisionUserData);
                }
            }
        }

        // Dinâmicas vs Dinâmicas (brute force)
        for (size_t i = 0; i < dynamicEntities.size(); i++)
        {
            Entity *a = dynamicEntities[i];
            for (size_t j = i + 1; j < dynamicEntities.size(); j++)
            {
                Entity *b = dynamicEntities[j];

                if (!a || !b || !a->shape || !b->shape)
                    continue;

                // Verifica se podem colidir (bidirecional)
                if (!a->canCollideWith(b) && !b->canCollideWith(a))
                    continue;

                if (a->collide(b))
                {
                    onCollision(a, b, collisionUserData);
                }
            }
        }
    }