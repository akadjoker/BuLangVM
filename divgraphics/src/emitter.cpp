#include "engine.hpp"
#include <cmath>
#include <cstdlib>

ParticleSystem gParticleSystem;
extern GraphLib gGraphLib;
extern Scene gScene;

// Utility functions
inline float randf()
{
    return (float)rand() / (float)RAND_MAX;
}

inline float randf(float min, float max)
{
    return min + randf() * (max - min);
}

inline Color lerpColor(Color a, Color b, float t)
{
    return Color{
        (unsigned char)((1.0f - t) * a.r + t * b.r),
        (unsigned char)((1.0f - t) * a.g + t * b.g),
        (unsigned char)((1.0f - t) * a.b + t * b.b),
        (unsigned char)((1.0f - t) * a.a + t * b.a)};
}

// ============================================================================
// Emitter Implementation
// ============================================================================

Emitter::Emitter(EmitterType t, int gr, int maxParticles)
    : type(t), graph(gr), layer(0)
{
    // Posição e direção
    pos = {0, 0};
    dir = {1, 0};
    spread = 0.5f;

    // Emissão
    rate = 50.0f;
    speedMin = 50.0f;
    speedMax = 150.0f;
    particleLife = 1.0f;
    lifetime = 0.5f;

    // Aparência
    colorStart = WHITE;
    colorEnd = WHITE;
    sizeStart = 4.0f;
    sizeEnd = 0.0f;

    // Física
    gravity = {0, 0};
    drag = 0.0f;

    // Rotação
    rotationMin = 0.0f;
    rotationMax = 0.0f;
    angularVelMin = 0.0f;
    angularVelMax = 0.0f;

    // Rendering
    blendMode = BLEND_ALPHA;

    // Estado
    active = true;
    finished = false;
    elapsed = 0.0f;
    accumulator = 0.0f;
    aliveCount = 0;
    firstDead = 0;

    // Aloca partículas
    particles.resize(maxParticles);
    for (auto &p : particles)
    {
        p.alive = false;
    }
}

void Emitter::emitAt(int index)
{
    Particle &p = particles[index];

    p.alive = true;
    p.pos = pos;
    p.maxLife = particleLife;
    p.life = particleLife;
    p.startColor = colorStart;
    p.endColor = colorEnd;
    p.startSize = sizeStart;
    p.endSize = sizeEnd;
    p.size = sizeStart;
    p.color = colorStart;
    p.acc = {0, 0};

    // Velocidade com spread
    float baseAngle = atan2f(dir.y, dir.x);
    float angle = baseAngle + (randf() - 0.5f) * spread;
    float speed = randf(speedMin, speedMax);
    p.vel = {cosf(angle) * speed, sinf(angle) * speed};

    // Rotação
    p.rotation = randf(rotationMin, rotationMax);
    p.angularVel = randf(angularVelMin, angularVelMax);

    aliveCount++;
}

void Emitter::emit()
{
    // Otimização: se pool cheio, não procura
    if (aliveCount >= (int)particles.size())
    {
        return;
    }

    // Procura a partir de firstDead
    for (int i = firstDead; i < (int)particles.size(); i++)
    {
        if (!particles[i].alive)
        {
            emitAt(i);
            firstDead = i + 1;
            return;
        }
    }

    // Se não achou, procura do início
    for (int i = 0; i < firstDead; i++)
    {
        if (!particles[i].alive)
        {
            emitAt(i);
            firstDead = i + 1;
            return;
        }
    }
}

// void Emitter::update(float dt)
// {
//     elapsed += dt;

//     // Emissão
//     if (active)
//     {
//         if (type == EMITTER_ONESHOT && elapsed >= lifetime)
//         {
//             active = false;

//         }

//         if (type == EMITTER_CONTINUOUS || elapsed < lifetime)
//         {
//             accumulator += rate * dt;

//             while (accumulator >= 1.0f)
//             {
//                 emit();
//                 accumulator -= 1.0f;
//                 if (type == EMITTER_ONESHOT)
//                 {
//                     printf("ONESHOT: Emitted particle, aliveCount=%d\n", aliveCount);
//                 }
//             }
//         }
//     }

//     // Atualiza partículas
//     firstDead = 0;

//     for (int i = 0; i < (int)particles.size(); i++)
//     {
//         Particle &p = particles[i];

//         if (!p.alive)
//         {
//             if (firstDead == i)
//                 firstDead = i + 1;
//             continue;
//         }

//         p.life -= dt;
//         if (p.life <= 0.0f)
//         {
//             p.alive = false;
//             aliveCount--;
//             if (firstDead == i)
//                 firstDead = i + 1;
//             continue;
//         }

//         float t = 1.0f - (p.life / p.maxLife);
//         p.color = lerpColor(p.startColor, p.endColor, t);
//         p.size = p.startSize + t * (p.endSize - p.startSize);

//         p.vel.x += gravity.x * dt;
//         p.vel.y += gravity.y * dt;
//         p.vel.x += p.acc.x * dt;
//         p.vel.y += p.acc.y * dt;

//         if (drag > 0.0f)
//         {
//             float dragFactor = 1.0f - drag * dt;
//             if (dragFactor < 0.0f)
//                 dragFactor = 0.0f;
//             p.vel.x *= dragFactor;
//             p.vel.y *= dragFactor;
//         }

//         p.pos.x += p.vel.x * dt;
//         p.pos.y += p.vel.y * dt;
//         p.rotation += p.angularVel * dt;
//     }

//     // Marca finished quando ONESHOT terminou e não há partículas
//     if (type == EMITTER_ONESHOT && !active && aliveCount == 0)
//     {
//         if (!finished)
//         {

//         }
//         finished = true;
//     }
// }
void Emitter::update(float dt)
{
    elapsed += dt;

    // ONESHOT: emite tudo no primeiro update
    if (type == EMITTER_ONESHOT && active && elapsed <= dt)
    {
        int burstCount = (int)(rate * lifetime);
        if (burstCount > (int)particles.size())
        {
            burstCount = (int)particles.size();
        }
       // printf("ONESHOT: Bursting %d particles\n", burstCount);
        burst(burstCount);
        active = false;
    }

    // CONTINUOUS: emissão normal
    if (type == EMITTER_CONTINUOUS && active)
    {
        accumulator += rate * dt;
        while (accumulator >= 1.0f)
        {
            emit();
            accumulator -= 1.0f;
        }
    }

    firstDead = 0;

    for (int i = 0; i < (int)particles.size(); i++)
    {
        Particle &p = particles[i];

        if (!p.alive)
        {
            if (firstDead == i)
                firstDead = i + 1;
            continue;
        }

        p.life -= dt;
        if (p.life <= 0.0f)
        {
            p.alive = false;
            aliveCount--;
            if (firstDead == i)
                firstDead = i + 1;
            continue;
        }

        float t = 1.0f - (p.life / p.maxLife);
        p.color = lerpColor(p.startColor, p.endColor, t);
        p.size = p.startSize + t * (p.endSize - p.startSize);

        p.vel.x += gravity.x * dt;
        p.vel.y += gravity.y * dt;

        if (drag > 0.0f)
        {
            float dragFactor = 1.0f - drag * dt;
            if (dragFactor < 0.0f)
                dragFactor = 0.0f;
            p.vel.x *= dragFactor;
            p.vel.y *= dragFactor;
        }

        p.pos.x += p.vel.x * dt;
        p.pos.y += p.vel.y * dt;
        p.rotation += p.angularVel * dt;
    }

    // Marca finished
    if (type == EMITTER_ONESHOT && !active && aliveCount == 0)
    {
        finished = true;
    }
}

void Emitter::draw()
{
    Graph *g = gGraphLib.getGraph(graph);

    Texture2D tex = gGraphLib.textures[g->texture];

    float scroll_x = gScene.layers[layer].scroll_x;
    float scroll_y = gScene.layers[layer].scroll_y;

    // Bounds da tela para culling
    float screenW = (float)gScene.width;
    float screenH = (float)gScene.height;

    // Blend mode
    int raylibBlendMode = BLEND_ALPHA;
    switch (blendMode)
    {
    case BLEND_ADDITIVE:
        raylibBlendMode = BLEND_ADDITIVE;
        break;
    case BLEND_MULTIPLIED:
        raylibBlendMode = BLEND_MULTIPLIED;
        break;
    default:
        raylibBlendMode = BLEND_ALPHA;
        break;
    }

    BeginBlendMode(raylibBlendMode);

    for (auto &p : particles)
    {
        if (!p.alive)
        {
            continue;
        }

        // Vector2 screenPos =
        // {
        //     p.pos.x - scroll_x,
        //     p.pos.y - scroll_y};

        // // Culling simples
        // float margin = p.size * 2.0f;
        // if (screenPos.x < -margin || screenPos.x > screenW + margin ||
        //     screenPos.y < -margin || screenPos.y > screenH + margin)
        // {
        //     continue;
        // }

        DrawTexturePro(tex, g->clip, (Rectangle){p.pos.x - scroll_x, p.pos.y - scroll_y, p.size, p.size},
                       (Vector2){g->clip.width / 2.0f, g->clip.height / 2.0f}, p.rotation, p.color);
    }

    EndBlendMode();
}

void Emitter::burst(int count)
{
    for (int i = 0; i < count; i++)
    {
        emit();
    }
}

void Emitter::stop()
{
    active = false;
}

void Emitter::restart()
{
    active = true;
    finished = false;
    elapsed = 0.0f;
    accumulator = 0.0f;
}

// ============================================================================
// Particle System Implementation
// ============================================================================

ParticleSystem::ParticleSystem()
{
}

ParticleSystem::~ParticleSystem()
{
    clear();
}

Emitter *ParticleSystem::spawn(EmitterType type, int graph, int maxParticles)
{
    Emitter *e = new Emitter(type, graph, maxParticles);
    emitters.push_back(e);
    return e;
}

Emitter *ParticleSystem::createExplosion(Vector2 pos, int graph, Color color)
{
    Emitter *e = spawn(EMITTER_ONESHOT, graph, 50);
    e->pos = pos;
    e->dir = {1, 0};
    e->spread = 2.0f * PI; // 360 graus
    e->rate = 1000.0f;     // Emite tudo de uma vez
    e->speedMin = 100.0f;
    e->speedMax = 300.0f;
    e->particleLife = 0.8f;
    e->lifetime = 0.01f; // Burst rápido

    e->colorStart = color;
    e->colorEnd = ColorAlpha(color, 0.0f);
    e->sizeStart = 8.0f;
    e->sizeEnd = 0.0f;

    e->gravity = {0, 2.0f}; // Queda
    e->drag = 0.5f;

    e->blendMode = BLEND_ADDITIVE;

    return e;
}

Emitter *ParticleSystem::createSmoke(Vector2 pos, int graph)
{
    Emitter *e = spawn(EMITTER_CONTINUOUS, graph, 100);
    e->pos = pos;
    e->dir = {0, -1}; // Para cima
    e->spread = 0.3f;
    e->rate = 20.0f;
    e->speedMin = 20.0f;
    e->speedMax = 50.0f;
    e->particleLife = 2.0f;

    e->colorStart = ColorAlpha(GRAY, 0.6f);
    e->colorEnd = ColorAlpha(DARKGRAY, 0.0f);
    e->sizeStart = 4.0f;
    e->sizeEnd = 12.0f; // Cresce

    e->gravity = {0, -20.0f}; // Sobe
    e->drag = 0.3f;

    e->angularVelMin = -1.0f;
    e->angularVelMax = 1.0f;

    e->blendMode = BLEND_ALPHA;

    return e;
}

Emitter *ParticleSystem::createFire(Vector2 pos, int graph)
{
    Emitter *e = spawn(EMITTER_CONTINUOUS, graph, 80);
    e->pos = pos;
    e->dir = {0, -1}; // Para cima
    e->spread = 0.4f;
    e->rate = 60.0f;
    e->speedMin = 50.0f;
    e->speedMax = 100.0f;
    e->particleLife = 0.6f;

    e->colorStart = (Color){255, 200, 50, 255}; // Amarelo
    e->colorEnd = (Color){255, 50, 0, 0};       // Vermelho -> transparente
    e->sizeStart = 6.0f;
    e->sizeEnd = 2.0f;

    e->gravity = {0, -50.0f}; // Sobe

    e->blendMode = BLEND_ADDITIVE;

    return e;
}

Emitter *ParticleSystem::createSparks(Vector2 pos, int graph, Color color)
{
    Emitter *e = spawn(EMITTER_ONESHOT, graph, 30);
    e->pos = pos;
    e->dir = {0, -1}; // Para cima
    e->spread = PI;   // 180 graus
    e->rate = 1000.0f;
    e->speedMin = 150.0f;
    e->speedMax = 300.0f;
    e->particleLife = 0.5f;
    e->lifetime = 0.01f;

    e->colorStart = color;
    e->colorEnd = ColorAlpha(color, 0.0f);
    e->sizeStart = 3.0f;
    e->sizeEnd = 0.5f;

    e->gravity = {0, 500.0f}; // Queda forte

    e->blendMode = BLEND_ADDITIVE;

    return e;
}

void ParticleSystem::update(float dt)
{
    // Swap-and-pop pattern
    for (size_t i = 0; i < emitters.size();)
    {
        Emitter *e = emitters[i];
        e->update(dt);

        // Remove oneshots finalizados
        if (e->finished && e->type == EMITTER_ONESHOT)
        {
            delete e;
            emitters[i] = emitters.back();
            emitters.pop_back();
            // Não incrementa i
        }
        else
        {
            i++;
        }
    }
}

void ParticleSystem::draw()
{
    for (auto &e : emitters)
    {
        e->draw();
    }
}

void ParticleSystem::clear()
{
    for (auto e : emitters)
    {
        delete e;
    }
    emitters.clear();
}

int ParticleSystem::getTotalParticles() const
{
    int total = 0;
    for (auto e : emitters)
    {
        total += e->getAliveCount();
    }
    return total;
}
