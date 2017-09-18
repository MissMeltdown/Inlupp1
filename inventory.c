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
  printf("\nVar god gör ett val ur menyn:\n"
	 "[L]ägg till en vara\n"
	 "[T]a bort en vara\n"
	 "[R]edigera en vara\n"
	 "Ån[g]ra senaste ändringen\n"
	 "Lista [h]ela varukatalogen\n"
	 "[A]vsluta\n\n");
}


char ask_question_menu() {
  int bufSize = 100;
  char buf[bufSize];
  print_menu();
  int x = read_string(buf, bufSize);
  char choice = toupper(buf[0]);

  while (x != 1 || strchr("LTRGHA", choice) == NULL) {
    printf("Felaktig inmatning: '%s'\n", buf);
    print_menu();
    x = read_string(buf, bufSize);
    choice = toupper(buf[0]);
  }
  
  return choice;
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
  for (int i=1; i<x; i++) if (!isdigit(str[i])) return false;

  return true;
}

char *ask_question_shelf(char *question) {
  return ask_question(question, shelf_is_valid, (convert_func) strdup).s;
}

bool shelf_exists(char* str) {
  return false;
}

void add_item_to_db(tree_t *db) {
  char *desc;
  char *shelfname;
  int amount;
  int price;
  item_t *item;
  list_t *list;
  println("Lägg till en ny vara");
  char *name = ask_question_string("Namn: ");
  if (tree_has_key(db, name)) {
    printf("%s finns redan i databasen.\n"
	   "Använder samma pris och beskrivning\n", name);
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
    list_append(list, &elem); 
    
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
    tree_insert(db, name, &newitem);
  }
}


void remove_item_from_db(tree_t *db) {
  char *key = ask_question_string("Vilken vara vill du ta bort? \n"); 
  tree_remove(db, key);
  //return tree_remove(db, key); --------------------------FIX LATER!
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
      //list_db(db);
    } else {
      printf("Avslutar\n");
      return; 
    }
  }
}


int main(void) {
  println("Välkommen till lagerhantering 1.0");
  println("=================================");
  tree_t *db = tree_new();
  event_loop(db);

}
