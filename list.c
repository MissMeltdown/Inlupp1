#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "list.h"

typedef struct link link_t;
//typedef void(*list_action)(L elem);

struct list {
  link_t *first;
  link_t *last;
};

struct link {
  link_t *next;
  L data;
};

list_t *list_new() {
  list_t *l = calloc(1, sizeof(list_t));
  return l;
}

void list_append(list_t *list, L elem) {
  link_t *l = malloc(sizeof(link_t));
  if (l) {
    l -> next = NULL;
    l -> data = elem;
  }

  if (list -> first) {
    list -> last -> next = l;
  } else {
    list -> first = l;
  }
  
  list -> last = l;
}

void list_prepend(list_t *list, L elem) {
  link_t *l = malloc(sizeof(link_t));
  if (l) {
    l -> next = NULL;
    l -> data = elem;
  }

  if (list -> first) {
    l -> next = list -> first;
  } else {
    list -> last = l;
  }
  
  list -> first = l;
}

bool list_insert(list_t *list, int index, L elem) {
  if (index < 0) {
    index += list_length(list) +1;
  }
  
  if (index == 0) {
    list_prepend(list,elem);
    return true;
  }
  
  link_t *cursor = list -> first;
  link_t *parent;
  for (int i=0; i<=index; i++) {
    if (i == index) {
      if (cursor) {
	link_t *l = malloc(sizeof(link_t));
	if (l) {
	  l -> next = cursor;
	  l -> data = elem;
	}
	parent -> next = l;
	  
	return true;
      } else
	{
	  list_append(list, elem);
	  
	  return true;
	}
    }
    else if (cursor == NULL) {
      return false;
    }

    parent = cursor;
    cursor = cursor->next;
  }
  return false;
}

bool list_remove(list_t *list, int index, L *elem) {
  if (index < 0) {
    index += list_length(list);
  }
  
  link_t *removeLink = list -> first;
  link_t *parent;
  if (index == 0) {
    if (removeLink) {
      list -> first = removeLink -> next;
    }
    else {
      return false;
    }
    *elem = removeLink -> data;
    free(removeLink);
    return true;
  }
  
  for (int i=0; i<=index; i++) {
    if (removeLink == NULL) {
      return false;
    }
    else if (i == index) {
      parent -> next = removeLink -> next;
	  
      if (removeLink -> next == NULL) {
	list -> last = parent;
      }
      *elem = removeLink -> data;
      free(removeLink);
      return true;
    }

    parent = removeLink;
    removeLink = removeLink -> next;
  }
  return false;
}

L *list_get(list_t *list, int index) {
  if (index < 0) {
    index += list_length(list);
  }
  
  link_t *cursor = list -> first;
  for (int i=0; i<=index; i++) {
    cursor = cursor -> next;
  }
  
  return &(cursor -> data);
}

L *list_first(list_t *list) {
  return &(list -> first -> data);
}

L *list_last(list_t *list) {
  return &(list -> last -> data);
}

int list_length(list_t *list) {
  link_t *cursor = list -> first;
  int i=0;
  for (i=0; cursor; i++) {
    cursor = cursor -> next;
  }
  return i;
}

/*
void list_delete(list_t *list, list_action cleanup) {
  
}

typedef void(*list_action2)(L elem, void *data) {
  
}

void list_apply(list_t *list, list_action2 fun, void *data) {
  
}

int main(int argc, char *argv[]) {
  for (int i = 0; i < argc; i++) {
    printf("%s\n", argv[i]);
  }
  list_t *l = list_new();
  list_append(l, 6);
  return 0; 
}
*/
