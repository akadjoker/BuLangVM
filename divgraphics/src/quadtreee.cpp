#include "engine.hpp"
extern Scene gScene;

Quadtree::Quadtree(Rectangle world_bounds)
{
    root = new QuadtreeNode(world_bounds, 0);
}
Quadtree::~Quadtree()
{
    delete root;
}

void Quadtree::clear() { root->clear(); }
void Quadtree::draw()
{

    root->draw();
}
void Quadtree::insert(Entity *entity) { root->insert(entity); }
void Quadtree::query(Rectangle area, std::vector<Entity *> &result)
{
    root->query(area, result);
}

bool QuadtreeNode::overlapsRect(Rectangle other)
{
    return !(bounds.x + bounds.width < other.x ||
             other.x + other.width < bounds.x ||
             bounds.y + bounds.height < other.y ||
             other.y + other.height < bounds.y);
}

void QuadtreeNode::split()
{
    float hw = bounds.width / 2.0f;
    float hh = bounds.height / 2.0f;

    children[0] = new QuadtreeNode({bounds.x, bounds.y, hw, hh}, depth + 1);
    children[1] = new QuadtreeNode({bounds.x + hw, bounds.y, hw, hh}, depth + 1);
    children[2] = new QuadtreeNode({bounds.x, bounds.y + hh, hw, hh}, depth + 1);
    children[3] = new QuadtreeNode({bounds.x + hw, bounds.y + hh, hw, hh}, depth + 1);
}

void QuadtreeNode::draw()
{
    float screenX = bounds.x;
    float screenY = bounds.y;

     Layer &l = gScene.layers[0];  // Quadtree é da layer 0
     screenX -= l.scroll_x;
     screenY -= l.scroll_y;

 


     

    DrawRectangleLines((int)screenX, (int)screenY,
                       (int)bounds.width, (int)bounds.height, GREEN);

    if (items.size() > 0)
    {
        DrawText(TextFormat("%d", (int)items.size()),
                 (int)(screenX + (bounds.width/2)), (int)(screenY + (bounds.height/2)), 12, RED);
    }

    // Desenha filhos (também com culling)
    if (children[0])
    {
        for (int i = 0; i < 4; i++)
        {
            children[i]->draw();
        }
    }
}

void QuadtreeNode::insert(Entity *entity)
{
    Rectangle entity_bounds = entity->getBounds();

    // Se tem filhos, insere neles
    if (children[0])
    {
        for (int i = 0; i < 4; i++)
        {
            if (children[i]->overlapsRect(entity_bounds))
            {
                children[i]->insert(entity);
            }
        }
        return;
    }

    // Adiciona ao vetor
    items.push_back(entity);

    // Se ultrapassou limite e não tá no max depth, faz split
    if ((int)items.size() > MAX_ITEMS && depth < MAX_DEPTH)
    {
        split();

        // Re-insere items nos filhos
        for (Entity *e : items)
        {
            Rectangle eb = e->getBounds();
            for (int i = 0; i < 4; i++)
            {
                if (children[i]->overlapsRect(eb))
                {
                    children[i]->insert(e);
                }
            }
        }

        items.clear(); // Libera memória do vetor
    }
}

void QuadtreeNode::query(Rectangle area, std::vector<Entity *> &result)
{
    if (!overlapsRect(area))
        return;

    for (Entity *e : items)
    {
        result.push_back(e);
    }
    if (children[0])
    {
        for (int i = 0; i < 4; i++)
        {
            children[i]->query(area, result);
        }
    }
   

}

void QuadtreeNode::clear()
{
    items.clear();

    if (children[0])
    {
        for (int i = 0; i < 4; i++)
        {
            delete children[i];
            children[i] = nullptr;
        }
    }
}

void Quadtree::rebuild(Scene *scene)
{
    clear();

    // Insere todas entities de todas layers
    for (int i = 0; i < MAX_LAYERS; i++)
    {
        for (Entity *e : scene->layers[i].nodes)
        {
            if (e->shape) // Só insere se tem collision shape
            {
                e->updateBounds();
                insert(e);
            }
        }
    }
}