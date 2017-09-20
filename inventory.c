#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>
#include "utils.h"
#include "tree.h"
#include "list.h"

typedef struct shelf shelf_t;
typedef struct item item_t;

struct shelf {
  char *shelfname;
  int amount;
};

struct item {
  char *name;
  char *desc;
  int price;
  list_t *shelfs;
};

void print_menu() {
  printf("\n[L]ägg till en vara\n"
	 "[T]a bort en vara\n"
	 "[R]edigera en vara\n"
	 "Ån[g]ra senaste ändringen\n"
	 "Lista [h]ela varukatalogen\n"
	 "[A]vsluta\n\n");
}


char ask_question_menu() {
  print_menu();
  char input = ask_question_char("Vad vill du göra? ");
  
  while (strchr("LlTtRrGgHhAa", input) == NULL) {
    printf("Felaktig inmatning: '%c'\n", input);
    input = ask_question_char("Vad vill du göra? ");
  }
  return toupper(input);
}

item_t make_item(char *name, char *desc, int price, char *shelfname, int amount) {
  list_t *shelflist = list_new();
  shelf_t shelf = {.shelfname = shelfname, .amount = amount};
  list_append(shelflist, &shelf);
  return (item_t) {.name = name, .desc = desc, .price = price, .shelfs = shelflist}; 
}

bool shelf_is_valid(char *str) {
  int x = strlen(str);
  if (x < 2) return false;
  if (isdigit(str[0])) return false;
  for (int i=1; i<x; i++) if (!isdigit(str[i])) return false;

  return true;
}

char *ask_question_shelf(char *question) {
  return ask_question(question, shelf_is_valid, (convert_func) strdup).s;
}

bool shelf_exists(char* str) {
  return false;
}

void print_item(item_t *item) {
  char *name = item -> name;
  char *desc = item -> desc;
  int price = item -> price;
  list_t *list = item -> shelfs;
  printf("\nNamn: %s\n", name);
  printf("Beskrivning: %s\n", desc);
  printf("Pris: %d\n", price);

  int l = list_length(list);
  
  for (int i=0; i<l; i++) {
    shelf_t *p = *list_get(list, i);
    char *shelf = p -> shelfname;
    int antal = p -> amount;
    printf("Hylla: %s, Antal: %d\n", shelf, antal);
  }
}

void add_item_to_db(tree_t *db) {
  char *desc;
  char *shelfname;
  int amount;
  int price;
  item_t *item;
  list_t *list;
  println("\nLägg till en ny vara\n");
  char *name = ask_question_string("Namn: ");
  if (tree_has_key(db, name)) {
    printf("%s finns redan i databasen.\n"
	   "Använder samma pris och beskrivning.\n", name);
    item = tree_get(db, name);
    price = item -> price;
    desc = item -> desc;
    list = item -> shelfs;
    
    shelfname = ask_question_shelf("Hylla: ");
    
    while (shelf_exists(shelfname)) {
      printf("Hyllan %s är upptagen, vänligen välj ny hylla\n", shelfname);
      shelfname = ask_question_shelf("Hylla: ");      
    }
    amount = ask_question_int("Antal: ");
    shelf_t elem = {.shelfname = shelfname, .amount = amount}; 
    
    list_append(list, &elem); //---------------det blir segmentation fault 11 i list_append...
    
  } else {
    desc  = ask_question_string("Beskrivning: ");
    price = ask_question_int("Pris: ");
    shelfname = ask_question_shelf("Hylla: ");

    while (shelf_exists(shelfname)) {
      printf("Hyllan %s är upptagen, vänligen välj ny hylla\n", shelfname);
      shelfname = ask_question_shelf("Hylla: ");      
    }
    
    amount = ask_question_int("Antal: ");
    item_t newitem = make_item(name, desc, price, shelfname, amount);
    char jnr = 'k';
    do {
      print_item(&newitem);
      jnr = toupper(ask_question_char("Vill du lägga till varan? ([J]a, [N]ej, [R]edigera) "));
      printf("%c\n", jnr);
      if (jnr == 'J') {
        tree_insert(db, name, &newitem);
      } else if (jnr == 'N') {
        return;
      } else if (jnr == 'R') {
        //newitem = edit_item(newitem);
      }
    } while (strchr("JNR", jnr) == NULL);
    
      //tree_insert(db, name, &newitem);
  }
}


void remove_item_from_db(tree_t *db) {
  if (tree_size(db) < 1) {
    printf("Databasen är tom, det finns inga varor att ta bort\n");
  } else {
    char *key = ask_question_string("Vilken vara vill du ta bort? \n");
    tree_remove(db, key);
  }
  //return tree_remove(db, key); --------------------------FIX LATER!
}

void funktion(K key, T elem, void *data) {
  print_item(elem);
  printf("\n");
}

void list_db(tree_t *db) {
  if (tree_size(db) < 1) {
    printf("Databasen är tom, det finns inga varor att lista\n");
  } else {
    tree_apply(db, inorder, funktion, NULL);
  }
}

void event_loop(tree_t *db) {
  while (true) {
    char command = ask_question_menu();
    if (command == 'L') {
      add_item_to_db(db);
    } else if (command == 'T') {
      remove_item_from_db(db); 
    } else if (command == 'R') {
      //edit_db(db);
    } else if (command == 'G') {
      //undo();
    } else if (command == 'H') {
      list_db(db);
    } else if (command == 'A'){
      printf("Avslutar\n");
      return; 
    }
  }
}


int main(void) {
  println("\nVälkommen till lagerhantering 1.0");
  println("=================================");
  tree_t *db = tree_new();
  event_loop(db);

}
