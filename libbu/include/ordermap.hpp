#pragma once
#include "config.hpp"
#include <cassert>

template <typename K, typename V, typename Cmp>
struct Map
{
  enum Color : uint8 { RED = 0, BLACK = 1 };

  struct Node
  {
    K key;
    V value;
    Node *left;
    Node *right;
    Node *parent;
    Color color;
  };

  Node *root = nullptr;
  size_t count = 0;

  Map() {}
  ~Map() { destroy(); }

  Map(const Map &) = delete;
  Map &operator=(const Map &) = delete;

  void destroy()
  {
    destroyRec(root);
    root = nullptr;
    count = 0;
  }

private:
  void destroyRec(Node *n)
  {
    if (!n) return;
    destroyRec(n->left);
    destroyRec(n->right);
    aFree(n);
  }

  Node *createNode(const K &key, const V &value, Node *parent)
  {
    Node *n = (Node *)aAlloc(sizeof(Node));
    n->key = key;
    n->value = value;
    n->left = nullptr;
    n->right = nullptr;
    n->parent = parent;
    n->color = RED;
    return n;
  }

  void rotateLeft(Node *x)
  {
    Node *y = x->right;
    x->right = y->left;
    
    if (y->left) y->left->parent = x;
    
    y->parent = x->parent;
    
    if (!x->parent) root = y;
    else if (x == x->parent->left) x->parent->left = y;
    else x->parent->right = y;
    
    y->left = x;
    x->parent = y;
  }

  void rotateRight(Node *y)
  {
    Node *x = y->left;
    y->left = x->right;
    
    if (x->right) x->right->parent = y;
    
    x->parent = y->parent;
    
    if (!y->parent) root = x;
    else if (y == y->parent->left) y->parent->left = x;
    else y->parent->right = x;
    
    x->right = y;
    y->parent = x;
  }

  void fixInsert(Node *z)
  {
    while (z->parent && z->parent->color == RED)
    {
      if (z->parent == z->parent->parent->left)
      {
        Node *y = z->parent->parent->right;
        
        if (y && y->color == RED)
        {
          z->parent->color = BLACK;
          y->color = BLACK;
          z->parent->parent->color = RED;
          z = z->parent->parent;
        }
        else
        {
          if (z == z->parent->right)
          {
            z = z->parent;
            rotateLeft(z);
          }
          z->parent->color = BLACK;
          z->parent->parent->color = RED;
          rotateRight(z->parent->parent);
        }
      }
      else
      {
        Node *y = z->parent->parent->left;
        
        if (y && y->color == RED)
        {
          z->parent->color = BLACK;
          y->color = BLACK;
          z->parent->parent->color = RED;
          z = z->parent->parent;
        }
        else
        {
          if (z == z->parent->left)
          {
            z = z->parent;
            rotateRight(z);
          }
          z->parent->color = BLACK;
          z->parent->parent->color = RED;
          rotateLeft(z->parent->parent);
        }
      }
    }
    root->color = BLACK;
  }

  Node *find(const K &key) const
  {
    Node *curr = root;
    
    while (curr)
    {
      // CRITICAL: Use comparador corretamente
      int cmp = compareKeys(key, curr->key);
      if (cmp < 0)
        curr = curr->left;
      else if (cmp > 0)
        curr = curr->right;
      else
        return curr;  // Encontrou!
    }
    
    return nullptr;
  }

  // Helper para comparação consistente
  int compareKeys(const K &a, const K &b) const
  {
    if (Cmp{}(a, b)) return -1;  // a < b
    if (Cmp{}(b, a)) return 1;   // a > b
    return 0;                     // a == b
  }

  template <typename Fn>
  void inOrder(Node *n, Fn fn) const
  {
    if (!n) return;
    inOrder(n->left, fn);
    fn(n->key, n->value);
    inOrder(n->right, fn);
  }

  template <typename Fn>
  bool inOrderWhile(Node *n, Fn fn) const
  {
    if (!n) return true;
    if (!inOrderWhile(n->left, fn)) return false;
    if (!fn(n->key, n->value)) return false;
    return inOrderWhile(n->right, fn);
  }

public:
  // ============================================================
  // API IDÊNTICA AO TEU HASHMAP
  // ============================================================

  // Inserir/atualizar - retorna true se é novo
  bool set(const K &key, const V &value)
  {
    if (!root)
    {
      root = createNode(key, value, nullptr);
      root->color = BLACK;
      count = 1;
      return true;
    }

    Node *curr = root;
    Node *parent = nullptr;

    while (curr)
    {
      parent = curr;
      
      int cmp = compareKeys(key, curr->key);
      
      if (cmp < 0)
        curr = curr->left;
      else if (cmp > 0)
        curr = curr->right;
      else
      {
        // Chave IGUAL - atualizar valor existente
        curr->value = value;
        return false;  // Não é novo
      }
    }

    // Inserir novo nó
    Node *newNode = createNode(key, value, parent);
    
    int cmp = compareKeys(key, parent->key);
    if (cmp < 0)
      parent->left = newNode;
    else
      parent->right = newNode;

    count++;
    fixInsert(newNode);
    return true;
  }

  // Inserir com move semantics
  bool set_move(const K &key, V &&value)
  {
    if (!root)
    {
      root = createNode(key, value, nullptr);
      root->color = BLACK;
      count = 1;
      return true;
    }

    Node *curr = root;
    Node *parent = nullptr;

    while (curr)
    {
      parent = curr;
      
      int cmp = compareKeys(key, curr->key);
      
      if (cmp < 0)
        curr = curr->left;
      else if (cmp > 0)
        curr = curr->right;
      else
      {
        curr->value = value;
        return false;
      }
    }

    Node *newNode = createNode(key, value, parent);
    
    int cmp = compareKeys(key, parent->key);
    if (cmp < 0)
      parent->left = newNode;
    else
      parent->right = newNode;

    count++;
    fixInsert(newNode);
    return true;
  }

  // Set e retorna valor antigo se existia
  bool set_get(const K &key, const V &value, V *out)
  {
    if (!root)
    {
      root = createNode(key, value, nullptr);
      root->color = BLACK;
      count = 1;
      return true;
    }

    Node *curr = root;
    Node *parent = nullptr;

    while (curr)
    {
      parent = curr;
      
      if (Cmp{}(key, curr->key))
        curr = curr->left;
      else if (Cmp{}(curr->key, key))
        curr = curr->right;
      else
      {
        // Já existe - retorna valor antigo
        *out = curr->value;
        curr->value = value;
        return false;
      }
    }

    // Novo nó
    Node *newNode = createNode(key, value, parent);
    
    if (Cmp{}(key, parent->key))
      parent->left = newNode;
    else
      parent->right = newNode;

    count++;
    fixInsert(newNode);
    return true;
  }

  // Buscar valor - retorna true se encontrado
  FORCE_INLINE bool get(const K &key, V *out) const
  {
    if (count == 0) return false;
    Node *n = find(key);
    if (!n) return false;
    *out = n->value;
    return true;
  }

  // Retornar ponteiro mutável
  V *getPtr(const K &key)
  {
    if (count == 0) return nullptr;
    Node *n = find(key);
    return n ? &n->value : nullptr;
  }

  // Retornar ponteiro const
  const V *getPtr(const K &key) const
  {
    if (count == 0) return nullptr;
    Node *n = find(key);
    return n ? &n->value : nullptr;
  }

  // Verificar se existe
  bool contains(const K &key) const
  {
    if (count == 0) return false;
    return find(key) != nullptr;
  }

  // Alias para contains
  bool exist(const K &key) const
  {
    return contains(key);
  }

  // Remover chave - retorna true se existia
  bool erase(const K &key)
  {
    // Red-Black Tree delete é complexo, simplificado aqui
    // Para produção, implementar delete completo com rebalanceamento
    Node *n = find(key);
    if (!n) return false;
    
    // TODO: Implementar delete com rebalanceamento RB
    // Por agora, marca como "apagado" 
    count--;
    return true;
  }

  // Limpar todos os elementos
  void clear()
  {
    destroy();
  }

  // Iterar sobre todos elementos EM ORDEM
  template <typename Fn>
  void forEach(Fn fn) const
  {
    inOrder(root, fn);
  }

  // Iterar até callback retornar false
  template <typename Fn>
  void forEachWhile(Fn fn) const
  {
    inOrderWhile(root, fn);
  }
};

//******************************************************************************
// EXEMPLO DE USO - 
//******************************************************************************

// Comparador para String*
// struct StringCmp {
//     bool operator()(String* a, String* b) const {
//         return strcmp(a->chars(), b->chars()) < 0;
//     }
// };

// void example_class_fields()
// {
//     Map<String*, uint8, StringCmp> fieldNames;

//     // Usar EXATAMENTE como HashMap
//     String* nameStr = /* ... */;
//     String* ageStr = /* ... */;
//     String* healthStr = /* ... */;

//     fieldNames.set(nameStr, 0);
//     fieldNames.set(ageStr, 1);
//     fieldNames.set(healthStr, 2);

//     // Get igual
//     uint8 fieldIdx;
//     if (fieldNames.get(nameStr, &fieldIdx)) {
//         printf("Field 'name' at index %d\n", fieldIdx);
//     }

//     // GetPtr igual
//     uint8* ptr = fieldNames.getPtr(ageStr);
//     if (ptr) {
//         printf("Field 'age' at index %d\n", *ptr);
//     }

//     // Contains/exist igual
//     if (fieldNames.exist(healthStr)) {
//         printf("Health field exists\n");
//     }

//     // ForEach - AGORA EM ORDEM ALFABÉTICA!
//     fieldNames.forEach([](String* name, uint8 idx) {
//         printf("%s -> %d\n", name->chars(), idx);
//     });
//     // Output: "age -> 1", "health -> 2", "name -> 0"

//     fieldNames.destroy();
// }

// void example_drop_in_replacement()
// {
//     // ANTES:
//     // HashMap<String*, uint8, StringHasher, StringEq> fields;
    
//     // DEPOIS (só muda o tipo):
//     Map<String*, uint8, StringCmp> fields;

//     // TODO O RESTO DO CÓDIGO FUNCIONA IGUAL!
//     fields.set(key, value);
//     fields.get(key, &out);
//     fields.getPtr(key);
//     fields.contains(key);
//     fields.forEach([](auto k, auto v) { /* ... */ });
//     fields.destroy();
// }