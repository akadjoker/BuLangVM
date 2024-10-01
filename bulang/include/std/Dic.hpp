
#pragma once
#include "Config.hpp"


//conteiner with balance factor
//https://en.wikipedia.org/wiki/AVL_tree
//https://en.wikipedia.org/wiki/Red%E2%80%93black_tree
//https://en.wikipedia.org/wiki/Red%E2%80%93black_tree#Algorithms

template <typename K, typename V>
struct NodeDic
{
    K key;
    V value;
    NodeDic<K,V> *left;
    NodeDic<K,V> *right;
    int height;

    NodeDic<K,V>(const K &k, const V &v) : key(k), value(v), left(nullptr), right(nullptr), height(1) {}
};

template <typename K, typename V>
class Dic
{
private:
    int height(NodeDic<K,V> *n) { return n ? n->height : 0; }
    int balanceFactor(NodeDic<K,V> *n) { return n ? height(n->left) - height(n->right) : 0; }
    void updateHeight(NodeDic<K,V> *n) { n->height = 1 + std::max(height(n->left), height(n->right)); }

    NodeDic<K,V> *rotateRight(NodeDic<K,V> *y);
    NodeDic<K,V> *rotateLeft(NodeDic<K,V> *x);
    NodeDic<K,V> *balance(NodeDic<K,V> *n);
    NodeDic<K,V> *insert(NodeDic<K,V> *node, const K &key, const V &value);
    NodeDic<K,V> *findMin(NodeDic<K,V> *node);
    NodeDic<K,V> *removeMin(NodeDic<K,V> *node);
    NodeDic<K,V> *remove(NodeDic<K,V> *node, const K &key);
    void clear(NodeDic<K,V> *node);



    NodeDic<K, V> *createNode(const K &key, const V &value);
    void deleteNode(NodeDic<K, V> *node);


    NodeDic<K,V> *root;

public:
    Dic();
    ~Dic();

    void insert(const K &key, const V &value);
    void remove(const K &key);
    void erase(const K &key);
    bool find(const K &key, V &value) const;
    bool contains(const K &key) const;
    void clear();

    NodeDic<K,V> *getRoot() { return root; }
    const NodeDic<K,V> *getRoot() const { return root; }
};

template <typename K, typename V>
inline void Dic<K, V>::clear(NodeDic<K,V> *node)
{
    if (!node)
        return;
    clear(node->left);
    clear(node->right);
    deleteNode(node);
}



template <typename K, typename V>
inline Dic<K, V>::Dic() : root(nullptr)
{
}

template <typename K, typename V>
inline Dic<K, V>::~Dic()
{
    clear(root);
  
}

template <typename K, typename V>
inline void Dic<K, V>::insert(const K &key, const V &value)
{
    root = insert(root, key, value);
}

template <typename K, typename V>
inline void Dic<K, V>::remove(const K &key)
{
    root = remove(root, key);
}

template <typename K, typename V>
inline void Dic<K, V>::erase(const K &key)
{
    remove(key);
}

template <typename K, typename V>
inline bool Dic<K, V>::find(const K &key, V &value) const
{
    NodeDic<K,V> *node = root;
    while (node)
    {
        if (key == node->key)
        {
            value = node->value;
            return true;
        }
        node = key < node->key ? node->left : node->right;
    }
    return false;
}

template <typename K, typename V>
inline bool Dic<K, V>::contains(const K &key) const
{
   NodeDic<K,V> *node = root;
    while (node)
    {
        if (key == node->key)
        {
            return true;
        }
        node = key < node->key ? node->left : node->right;
    }
    return false;
}

template <typename K, typename V>
inline NodeDic<K,V> *Dic<K, V>::rotateRight(NodeDic<K,V> *y)
{
    NodeDic<K,V> *x = y->left;
    NodeDic<K,V> *T2 = x->right;
    x->right = y;
    y->left = T2;
    updateHeight(y);
    updateHeight(x);
    return x;
}

template <typename K, typename V>
inline NodeDic<K,V> *Dic<K, V>::rotateLeft(NodeDic<K,V> *x)
{
    NodeDic<K,V> *y = x->right;
    NodeDic<K,V> *T2 = y->left;
    y->left = x;
    x->right = T2;
    updateHeight(x);
    updateHeight(y);
    return y;
}

template <typename K, typename V>
inline NodeDic<K,V> *Dic<K, V>::balance(NodeDic<K,V> *n)
{
    updateHeight(n);
    if (balanceFactor(n) > 1)
    {
        if (balanceFactor(n->left) < 0)
        {
            n->left = rotateLeft(n->left);
        }
        return rotateRight(n);
    }
    if (balanceFactor(n) < -1)
    {
        if (balanceFactor(n->right) > 0)
        {
            n->right = rotateRight(n->right);
        }
        return rotateLeft(n);
    }
    return n;
}

template <typename K, typename V>
inline NodeDic<K,V> *Dic<K, V>::insert(NodeDic<K,V> *node, const K &key, const V &value)
{
    if (!node)
        return  createNode(key, value);
    if (key < node->key)
        node->left = insert(node->left, key, value);
    else if (key > node->key)
        node->right = insert(node->right, key, value);
    else
        node->value = value; // update value if key is found
    return balance(node);
}

template <typename K, typename V>
inline NodeDic<K,V> *Dic<K, V>::findMin(NodeDic<K,V> *node)
{
    return node->left ? findMin(node->left) : node;
}

template <typename K, typename V>
inline NodeDic<K,V> *Dic<K, V>::removeMin(NodeDic<K,V> *node)
{
    if (!node->left)
        return node->right;
    node->left = removeMin(node->left);
    return balance(node);
}

template <typename K, typename V>
inline NodeDic<K,V> *Dic<K, V>::remove(NodeDic<K,V> *node, const K &key)
{
    if (!node)
        return nullptr;
    if (key < node->key)
        node->left = remove(node->left, key);
    else if (key > node->key)
        node->right = remove(node->right, key);
    else
    {
        NodeDic<K,V> *left = node->left;
        NodeDic<K,V> *right = node->right;
        deleteNode(node);
        if (!right)
            return left;
        NodeDic<K,V> *min = findMin(right);
        min->right = removeMin(right);
        min->left = left;
        return balance(min);
    }
    return balance(node);
}

template <typename K, typename V>
inline NodeDic<K, V> *Dic<K, V>::createNode(const K &key, const V &value)
{
    return new NodeDic<K,V>(key, value);
    //void *ptr = m_arena.Allocate(sizeof(NodeDic<K,V>));
    //return new (ptr) NodeDic<K,V>(key, value);
}

template <typename K, typename V>
inline void Dic<K, V>::deleteNode(NodeDic<K, V> *node)
{
    delete node;
    //node->~NodeDic<K,V>();
   // m_arena.Free(node, sizeof(NodeDic<K,V>));
}
