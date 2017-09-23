#include "tree.h"

typedef struct node node_t;

struct node {
  node_t *left;
  node_t *right;
  char *key;
  T data;
};

struct tree {
  node_t *topnode;
};


tree_t *tree_new() {
  tree_t *t = calloc(1, sizeof(tree_t));
  return t;
}

void rec_delete(node_t *node, tree_action cleanup) {
  node_t *leftnode = node -> left;
  node_t *rightnode = node -> right;
  K key = node -> key;
  T data = node -> data;

  if (!cleanup) cleanup(key, data);
  free(node);

  if (leftnode) rec_delete(leftnode, cleanup);
  if (rightnode) rec_delete(rightnode, cleanup);
}

void tree_delete(tree_t *tree, tree_action cleanup) {
  node_t *topnode = tree -> topnode;
  if (topnode) rec_delete(topnode, cleanup);
  free(tree);
}

int rec_size(node_t *node) {
  if (!node) return 0;
  
  node_t *leftnode = node -> left;
  node_t *rightnode = node -> right;

  return 1 + rec_size(leftnode) + rec_size(rightnode);
}

int tree_size(tree_t *tree) {
  node_t *topnode = tree -> topnode;
  return rec_size(topnode);
}

int max2(int a, int b) {
  return a > b ? a : b;
}

int rec_depth(node_t *node) {
  if (!node) return 0;
  
  node_t *leftnode = node -> left;
  node_t *rightnode = node -> right;

  return 1 + max2(rec_depth(leftnode), rec_depth(rightnode));
}

int tree_depth(tree_t *tree) {
  node_t *topnode = tree -> topnode;
  return rec_depth(topnode);
}

bool tree_insert(tree_t *tree, K key, T elem) {
  node_t *cursorNode = tree -> topnode;
  node_t *parent = cursorNode;
  int cmp = 0;

  while (cursorNode) {
    cmp = strcmp(cursorNode -> key, key);
    if (cmp < 0) { //före i bokstavsordning
      parent = cursorNode;
      cursorNode = cursorNode -> left;
    } else if (cmp > 0) {
      parent = cursorNode;
      cursorNode = cursorNode -> right;
    } else {
      return false;
    }
  }

  node_t *n = malloc(sizeof(node_t));
  if (n) {
    n -> left = NULL;
    n -> right = NULL;
    n -> key = key;
    n -> data = elem;
  }
  else return false;

  if (tree -> topnode == NULL) {
    tree -> topnode = n;
  } else if (cmp < 0) {
    parent -> left = n;
  } else {
    parent -> right = n;
  }
  return true;
}

bool tree_has_key(tree_t *tree, K key) {
  node_t *cursorNode = tree -> topnode;

  while (cursorNode) {
      int cmp = strcmp(cursorNode -> key, key);
      if (cmp < 0) { //före i bokstavsordning
        cursorNode = cursorNode -> left;
      } else if (cmp > 0) {
        cursorNode = cursorNode -> right;
      } else {
        return true;
      }
  }
  return false; 
}

T tree_get(tree_t *tree, K key)
{
  node_t *cursorNode = tree -> topnode;
  
  while (cursorNode) {
    int cmp = strcmp(cursorNode -> key, key);
    if (cmp < 0) {//före i bokstavsordning
      cursorNode = cursorNode -> left;
    } else if (cmp > 0) {
      cursorNode = cursorNode -> right;
    } else {
      return cursorNode -> data;
    }
  }
  return NULL;
}

T tree_remove(tree_t *tree, K key) //Trädet får ej vara tomt
{
  node_t *removeNode = tree -> topnode;
  node_t *parent = removeNode;
  bool leftOrRight = true;
  
  while (removeNode) {
    int cmp = strcmp(removeNode -> key, key);
    if (cmp < 0) { //före i bokstavsordning
      parent = removeNode;
      removeNode = removeNode -> left;
      leftOrRight = false;
    } else if (cmp > 0) {
      parent = removeNode;
      removeNode = removeNode -> right;
      leftOrRight = true;
    } else {
      break;
    }
  }
  
  T removedElement = removeNode -> data;
    
  if (removeNode -> right) {
    node_t *leftchild = removeNode -> left;
    node_t *rightchild = removeNode -> right;
    node_t *moveNode = removeNode -> right;
    node_t *moveParent = removeNode;
    
    while (moveNode -> left) {
      moveParent = moveNode;
      moveNode = moveNode -> left;
    }
    
    moveParent -> left = moveNode -> right;
    
    moveNode -> left = leftchild;
    moveNode -> right = rightchild;
    
    if (removeNode == parent) { //topnoden tas bort
      tree -> topnode = moveNode;
    } else {
      if (leftOrRight) {
        parent -> right = moveNode;
      } else {
        parent -> left = moveNode;
      }
    }
    
    free(removeNode);
    
  } else if (removeNode -> left) {
    //länka parent till left, sen free(removeNode)
    
    if (removeNode == parent) { //topnoden tas bort
      tree -> topnode = removeNode -> left;
    } else {
      parent -> left = removeNode -> left;
      
      if (leftOrRight) {
        parent -> right = removeNode -> left;
      } else {
        parent -> left = removeNode -> left;
      }
    }
    free(removeNode);
  } else {
    //länka parent till NULL, sen free(removeNode)
    if (removeNode == parent) {//topnoden tas bort
      tree -> topnode = NULL;
    } else {
      if (leftOrRight) {
        parent -> right = NULL;
      } else {
        parent -> left = NULL;
      }
    }
    free(removeNode);
  }
  
  return removedElement;
}

//-------------Funktioner nedanför ej testade, de komplieras iaf ;)

void tree_keys_rec(node_t *node, K buf[], int *pos) {
  node_t *leftnode = node -> left;
  node_t *rightnode = node -> right;
  K key = node -> key;

  if (rightnode) tree_keys_rec(rightnode, buf, pos);
  
  buf[*pos] = key;
  ++*pos;
  
  if (leftnode) tree_keys_rec(leftnode, buf, pos);
}

K *tree_keys(tree_t *tree) {
  K *buf = calloc(tree_size(tree), sizeof(K));`
  node_t *topnode = tree -> topnode;
  int pos = 0;
  tree_keys_rec(topnode, buf, &pos);
  
  return buf;
  //return (K *) (strdup(*buf));
}

void tree_elements_rec(node_t *node, T buf[], int *pos) {
  node_t *leftnode = node -> left;
  node_t *rightnode = node -> right;
  T elem = node -> data;

  if (leftnode) tree_elements_rec(leftnode, buf, pos);
  
  buf[*pos] = elem;
  ++*pos;
  
  if (rightnode) tree_elements_rec(rightnode, buf, pos);
}

T *tree_elements(tree_t *tree) {
  int size = tree_size(tree);
  T buf[size];
  node_t *topnode = tree -> topnode;
  int pos = 0;
  tree_elements_rec(topnode, buf, &pos);
  
  return (T *) (strdup(*buf));
}

void tree_apply_rec(node_t *node, enum tree_order order, tree_action2 fun, void *data) {
  node_t *leftnode = node -> left;
  node_t *rightnode = node -> right;
  K key  = node -> key;
  T elem = node -> data;

  if (order == preorder) fun(key, elem, data);

  if (leftnode) tree_apply_rec(leftnode, order, fun, data);

  if (order == inorder) fun(key, elem, data);

  if (rightnode) tree_apply_rec(rightnode, order, fun, data);

  if (order == postorder) fun(key, elem, data);
}

void tree_apply(tree_t *tree, enum tree_order order, tree_action2 fun, void *data) {
  node_t *topnode = tree -> topnode;
  tree_apply_rec(topnode, order, fun, data);
}
