#include "engine.hpp"
#include <math.h>

extern GraphLib gGraphLib;
extern Scene gScene;

inline double cos_deg(int64 angle)
{
    return cos(angle * M_PI / 180000.0);
}

inline double sin_deg(int64 angle)
{
    return sin(angle * M_PI / 180000.0);
}

int getAngle(float x1, float y1, float x2, float y2)
{
    float dx = x2 - x1;
    float dy = y2 - y1;

    if (dx == 0)
        return (dy > 0) ? 270000 : 90000;

    int angle = (int)(atan(dy / dx) * 180000.0 / M_PI);
    return (dx > 0) ? -angle : -angle + 180000;
}

void rotatePoint(float &px, float &py, float angle)
{
    if (angle == 0)
        return;

    float rad = angle * M_PI / -180000.0f;
    float cos_a = cosf(rad);
    float sin_a = sinf(rad);

    float rx = px * cos_a - py * sin_a;
    float ry = px * sin_a + py * cos_a;
    px = rx;
    py = ry;
}

Vector2 Entity::getRealPoint(int pointIdx) const
{
    Graph *g = gGraphLib.getGraph(graph);
    if (!g || pointIdx < 0 || pointIdx >= (int)g->points.size())
        return {0, 0};

    Vector2 point = g->points[pointIdx];
    return getRealPointCustom(point.x, point.y);
}

bool Entity::collide(Entity *other)
{

    if (other == nullptr)
        return false;
    if (!shape || !other->shape)
        return false;

    return shape->collide(other->shape,
                          x, y, angle, flags & B_HMIRROR, flags & B_VMIRROR,
                          other->x, other->y, other->angle,
                          other->flags & B_HMIRROR, other->flags & B_VMIRROR);
}

static uint32 EIDS = 0;

Entity::Entity()
{
    shape = nullptr;
    id = 0;
    graph = 0;
    layer = 0;
    x = 0;
    y = 0;
    last_x = 0;
    last_y = 0;
    flip_x = false;
    flip_y = false;
    angle = 0;

    size_x = 100;
    size_y = 100;
    center_x = -1; // POINT_UNDEFINED
    center_y = -1; // POINT_UNDEFINED
    angle = 0;
    color = WHITE;
    flags = B_VISIBLE | B_COLLISION;
    resolution = 1;
    bounds_dirty = true;
    collision_layer = 1;         // Layer 1 por default
    collision_mask = 0xFFFFFFFF; // Colide com todas por default
}

Entity::~Entity()
{
    if (shape)
        delete shape;
}

void Entity::moveBy(double x, double y)
{
    // Guarda última posição
    last_x = this->x;
    last_y = this->y;

    int moveX = (int)round(x);
    int moveY = (int)round(y);

    // Sem shape/collision? Move direto
    if (!shape || !(flags & B_COLLISION))
    {
        this->x += moveX;
        this->y += moveY;
        bounds_dirty = true;
        return;
    }

    // Query área do movimento
    updateBounds();
    Rectangle moveBounds = bounds;
    moveBounds.x += fmin(0, moveX);
    moveBounds.y += fmin(0, moveY);
    moveBounds.width += fabs(moveX);
    moveBounds.height += fabs(moveY);

    std::vector<Entity *> nearby;
    gScene.staticTree->query(moveBounds, nearby);
    for (Entity *dyn : gScene.dynamicEntities)
        if (dyn != this)
            nearby.push_back(dyn);

    // Move X pixel-a-pixel
    if (moveX != 0)
    {
        int sign = (moveX > 0) ? 1 : -1;
        while (moveX != 0)
        {
            this->x += sign;
            updateBounds();

            for (Entity *other : nearby)
            {
                if (!other->shape || !(other->flags & B_COLLISION))
                    continue;
                if (!canCollideWith(other))
                    continue;
                if (collide(other))
                {
                    this->x -= sign;
                    
                }
            }
            moveX -= sign;
        }
 
    }

    // Move Y pixel-a-pixel
    if (moveY != 0)
    {
        int sign = (moveY > 0) ? 1 : -1;
        while (moveY != 0)
        {
            this->y += sign;
            updateBounds();

            for (Entity *other : nearby)
            {
                if (!other->shape || !(other->flags & B_COLLISION))
                    continue;
                if (!canCollideWith(other))
                    continue;
                if (collide(other))
                {
                    this->y -= sign;
                    
                }
            }
            moveY -= sign;
        }
 
    }

    bounds_dirty = true;
}

Vector2 Entity::getRealPointCustom(double point_x, double point_y) const
{
    Graph *b = gGraphLib.getGraph(graph);
    if (!b)
        return {0, 0};

    // 1. Coordenadas COM resolution aplicada
    double px = x;
    double py = y;

    if (resolution > 0)
    {
        px /= resolution;
        py /= resolution;
    }
    else if (resolution < 0)
    {
        px *= -resolution;
        py *= -resolution;
    }

    // 2. Determinar o centro/pivot
    double centerx, centery;

    if (center_x == -1 || center_y == -1)
    {
        // Usa centro do graph (points[0])
        centerx = b->points[0].x;
        centery = b->points[0].y;
    }
    else
    {
        centerx = center_x;
        centery = center_y;
    }

    // 3. Escala (NÃO afetada por resolution!)
    double scale_x = size_x / 100.0;
    double scale_y = size_y / 100.0;

    // 4. Offset do ponto relativo ao centro (COM escala)
    double dx = (point_x - centerx) * scale_x;
    double dy = (point_y - centery) * scale_y;

    // 5. Aplicar mirror
    if (flags & B_HMIRROR)
        dx = -dx;
    if (flags & B_VMIRROR)
        dy = -dy;

    // 6. Aplicar rotação
    double cos_angle = cos_deg(angle);
    double sin_angle = sin_deg(angle);

    double rotated_x = dx * cos_angle - dy * sin_angle;
    double rotated_y = dx * sin_angle + dy * cos_angle;

    // 7. Posição final (já com resolution aplicada no px/py)
    double final_x = px + rotated_x;
    double final_y = py + rotated_y;

    // 8. Aplicar resolution de volta para world space
    if (resolution > 0)
    {
        final_x *= resolution;
        final_y *= resolution;
    }
    else if (resolution < 0)
    {
        final_x /= -resolution;
        final_y /= -resolution;
    }

    return {(float)final_x, (float)final_y};
}

int64 Entity::getAngle(const Entity *other) const
{
    if (!other)
        return -1;

    double dx = other->x - x;
    double dy = other->y - y;

    if (dx == 0)
    {
        return (dy > 0) ? 270000L : 90000L;
    }

    int64 angle = (int64)(atan(dy / dx) * 180000.0 / M_PI);

    return (dx > 0) ? -angle : -angle + 180000L;
}
double Entity::getDistance(const Entity *other) const
{
    if (!other)
        return -1;

    double x1 = x, y1 = y;
    double x2 = other->x, y2 = other->y;

    // Aplicar resolução nas coordenadas
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

    double dx = (x2 - x1) * (x2 - x1);
    double dy = (y2 - y1) * (y2 - y1);

    double ret = sqrt(dx + dy);

    // Aplicar resolução ao resultado
    if (resolution > 0)
    {
        ret *= resolution;
    }
    else if (resolution < 0)
    {
        ret /= -resolution;
    }

    return ret;
}

Rectangle Entity::getBounds()
{
    if (bounds_dirty)
        updateBounds();
    return bounds;
}
Vector2 Entity::getPoint(int pointIdx) const
{
    Graph *g = gGraphLib.getGraph(graph);
    if (!g || pointIdx < 0 || pointIdx >= (int)g->points.size())
        return {0, 0};

    Vector2 &p = g->points[pointIdx];
    return {(float)p.x, (float)p.y};
}
void Entity::render()
{
    Graph *g = gGraphLib.getGraph(graph);
    if (!g)
        return;

    Texture2D tex = gGraphLib.textures[g->texture];

    double screenX = x;
    double screenY = y;

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

    // 1. Posição final com scroll
    Layer &l = gScene.layers[layer];
    float finalX = (float)(screenX - l.scroll_x);
    float finalY = (float)(screenY - l.scroll_y);

    // 2. Determinar pivot
    float pivotX, pivotY;

    if (center_x == -1 || center_y == -1)
    {
        // Usa sempre g.points[0] (centro do graph)
        pivotX = g->points[0].x;
        pivotY = g->points[0].y;
    }
    else
    {
        // Usa pivot custom da entity
        pivotX = (float)center_x;
        pivotY = (float)center_y;
    }

    // 3. Determinar escala final
    float scale_x_final = (float)size_x / 100.0f;
    float scale_y_final = (float)size_y / 100.0f;

    // 5. Ângulo e flags
    float angleDeg = (float)-angle / 1000.0f;
    bool flipX = (flags & B_HMIRROR) != 0;
    bool flipY = (flags & B_VMIRROR) != 0;

    // DrawCircleLines((int)finalX, (int)finalY, 5, RED);

    // DrawRectangleLines((int)finalX, (int)finalY, (int)scale_x_final, (int)scale_y_final, RED);

    // 6. Renderizar
    RenderTexturePivotRotateSizeXY(
        tex,
        (int)pivotX,
        (int)pivotY,
        g->clip,
        finalX,
        finalY,
        angleDeg,
        scale_x_final,
        scale_y_final,
        flipX,
        flipY,
        color);
}

void Entity::advance(double distance)
{
    if (isFrozen())
        return;
    x += cos_deg(angle) * distance;
    y -= sin_deg(angle) * distance;
}

void Entity::xadvance(int64 angle_param, double distance)
{
    if (isFrozen())
        return;
    x += cos_deg(angle_param) * distance;
    y -= sin_deg(angle_param) * distance;
}

// Normaliza ângulo para 0-360000
static inline int64 normalizeAngle(int64 angle)
{
    angle = angle % 360000;
    if (angle < 0)
        angle += 360000;
    return angle;
}

// Calcula menor diferença angular entre dois ângulos
static inline int64 angleDifference(int64 from, int64 to)
{
    from = normalizeAngle(from);
    to = normalizeAngle(to);

    int64 diff = to - from;

    // Normaliza para -180000 a 180000 (menor caminho)
    if (diff > 180000)
    {
        diff -= 360000;
    }
    else if (diff < -180000)
    {
        diff += 360000;
    }

    return diff;
}

// Rotaciona gradualmente em direção a um ponto
void Entity::turnTowards(double targetX, double targetY, int64 maxTurn)
{
    double dx = targetX - x;
    double dy = targetY - y;

    if (dx == 0 && dy == 0)
        return;

    // Calcula ângulo alvo
    int64 targetAngle;
    if (dx == 0)
    {
        targetAngle = (dy > 0) ? 270000L : 90000L;
    }
    else
    {
        targetAngle = (int64)(atan(dy / dx) * 180000.0 / M_PI);
        targetAngle = (dx > 0) ? -targetAngle : -targetAngle + 180000L;
    }

    // Calcula diferença angular (menor caminho)
    int64 diff = angleDifference(angle, targetAngle);

    // Limita a rotação ao máximo permitido
    if (abs(diff) <= maxTurn)
    {
        angle = targetAngle;
    }
    else
    {
        if (diff > 0)
        {
            angle += maxTurn;
        }
        else
        {
            angle -= maxTurn;
        }
    }

    // Normaliza o ângulo final
    angle = normalizeAngle(angle);
}

bool Entity::moveTowards(double targetX, double targetY, double speed, double stopDistance)
{
    double dx = targetX - x;
    double dy = targetY - y;

    // Calcula distância atual
    double distance = sqrt(dx * dx + dy * dy);

    // Se já chegou, retorna true
    if (distance <= stopDistance)
    {
        return true;
    }

    // Se a velocidade levaria além do alvo, vai direto para o alvo
    if (distance <= speed)
    {
        x = targetX;
        y = targetY;
        return true;
    }

    // Move na direção do alvo
    double ratio = speed / distance;
    x += dx * ratio;
    y += dy * ratio;

    return false;
}
void Entity::setRectangleShape(int x, int y, int w, int h)
{
    if (shape)
    {
        delete shape;
    }
    shape = new RectangleShape(x, y, w, h);
}

void Entity::setCircleShape(float radius)
{
    if (shape)
    {
        delete shape;
    }
    shape = new CircleShape();
    ((CircleShape *)shape)->radius = radius;
}

void Entity::setShape(Vector2 *points, int n)
{
    if (shape)
    {
        delete shape;
    }
    shape = new PolygonShape(n);

    PolygonShape *polygon = (PolygonShape *)shape;
    for (int i = 0; i < n; i++)
    {
        polygon->points[i] = points[i];
    }
    polygon->calcNormals();
}