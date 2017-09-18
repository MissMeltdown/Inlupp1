#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "tree.h"

typedef struct node node_t;

struct node
{
  node_t *left;
  node_t *right;
  char *key;
  T data;
};

struct tree
{
  node_t *topnode;
};


tree_t *tree_new()
{
  tree_t *t = calloc(1, sizeof(tree_t));
  return t;
}

void rec_delete(node_t *node)
{
  node_t *leftnode = node -> left;
  node_t *rightnode = node -> right;

  free(node);

  if (leftnode) rec_delete(leftnode);
  if (rightnode) rec_delete(rightnode);
}

void tree_delete(tree_t *tree)
{
  node_t *topnode = tree -> topnode;
  if (topnode) rec_delete(topnode);
  free(tree);
}

int rec_size(node_t *node)
{
  if (!node) return 0;
  
  node_t *leftnode = node -> left;
  node_t *rightnode = node -> right;

  return 1 + rec_size(leftnode) + rec_size(rightnode);
}

int tree_size(tree_t *tree)
{
  node_t *topnode = tree -> topnode;
  return rec_size(topnode);
}

int max2(int a, int b)
{
  return a > b ? a : b;
}

int rec_depth(node_t *node)
{
  if (!node) return 0;
  
  node_t *leftnode = node -> left;
  node_t *rightnode = node -> right;

  return 1 + max2(rec_depth(leftnode), rec_depth(rightnode));
}

int tree_depth(tree_t *tree)
{
  node_t *topnode = tree -> topnode;
  return rec_depth(topnode);
}

bool tree_insert(tree_t *tree, K key, T elem)
{
  node_t *cursorNode = tree -> topnode;
  node_t *parent = cursorNode;
  int cmp = 0;

  while (cursorNode)
    {
      cmp = strcmp(cursorNode -> key, key);
      if (cmp < 0) //före i bokstavsordning
        {
          parent = cursorNode;
          cursorNode = cursorNode -> left;
        }
      else if (cmp > 0)
        {
          parent = cursorNode;
          cursorNode = cursorNode -> right;
        }
      else
        {
          return false;
        }
    }

  node_t *n = malloc(sizeof(node_t));
  if (n)
    {
      n -> left = NULL;
      n -> right = NULL;
      n -> key = key;
      n -> data = elem;
    }
  else return false;

  if (tree -> topnode == NULL) tree -> topnode = n;
  else if (cmp < 0) parent -> left = n;
  else parent -> right = n;

  return true;
}

bool tree_has_key(tree_t *tree, K key)
{
  node_t *cursorNode = tree -> topnode;
  while (cursorNode)
    {
      int cmp = strcmp(cursorNode -> key, key);
      if (cmp < 0) //före i bokstavsordning
        {
          cursorNode = cursorNode -> left;
        }
      else if (cmp > 0)
        {
          cursorNode = cursorNode -> right;
        }
      else
        {
          return true;
        }
    }
  return false; 
}

T tree_get(tree_t *tree, K key)
{
  node_t *cursorNode = tree -> topnode;
  while (cursorNode)
    {
      int cmp = strcmp(cursorNode -> key, key);
      if (cmp < 0) //före i bokstavsordning
        {
          cursorNode = cursorNode -> left;
        }
      else if (cmp > 0)
        {
          cursorNode = cursorNode -> right;
        }
      else
        {
          return cursorNode -> data;
        }
    }
  return 0; 
    //(item_t) {.name = NULL, .desc = NULL, .price = NULL, .shelf = NULL}; //-----------------------------FIX!!
}

T tree_remove(tree_t *tree, K key) //Trädet får ej vara tomt
{
  node_t *removeNode = tree -> topnode;
  node_t *parent = removeNode;
  bool leftOrRight = true;
  while (removeNode)
    {
      int cmp = strcmp(removeNode -> key, key);
      if (cmp < 0) //före i bokstavsordning
        {
          parent = removeNode;
          removeNode = removeNode -> left;
          leftOrRight = false;
        }
      else if (cmp > 0)
        {
          parent = removeNode;
          removeNode = removeNode -> right;
          leftOrRight = true;
        }
      else
        {
          break;
        }
    }
  
  T removedElement = removeNode -> data;
    
  if (removeNode -> right)
    {
      node_t *leftchild = removeNode -> left;
      node_t *rightchild = removeNode -> right;
      node_t *moveNode = removeNode -> right;
      node_t *moveParent = removeNode;
      while (moveNode -> left)
        {
          moveParent = moveNode;
          moveNode = moveNode -> left;
        }
      
      moveParent -> left = moveNode -> right;
      
      moveNode -> left = leftchild;
      moveNode -> right = rightchild;

      if (removeNode == parent) //topnoden tas bort
        {
          tree -> topnode = moveNode;
        }
      else
        {
          if (leftOrRight)
            {
              parent -> right = moveNode;
            }
          else{
              parent -> left = moveNode;
            }
        }

      free(removeNode);
      
    }
  else if (removeNode -> left)
    {
      //länka parent till left, sen free(removeNode)
      
      if (removeNode == parent) //topnoden tas bort
        {
          tree -> topnode = removeNode -> left;
        }
      else
        {
          parent -> left = removeNode -> left;

          if (leftOrRight)
            {
              parent -> right = removeNode -> left;
            }
          else
            {
              parent -> left = removeNode -> left;
            }
        }
      free(removeNode);
    }
  else
    {
      //länk parent till NULL, sen free(removeNode)
      if (removeNode == parent) //topnoden tas bort
        {
          tree -> topnode = NULL;
        }
      else
        {
          if (leftOrRight)
            {
              parent -> right = NULL;
            }
          else
            {
              parent -> left = NULL;
            }
        }
      free(removeNode);
    }

  return removedElement;
}
