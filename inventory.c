#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>
#include "utils.h"
#include "tree.h"
#include "list.h"

typedef struct shelf {
  char *shelfname;
  int amount;
} shelf_t;

typedef struct item {
  char *name;
  char *desc;
  int price;
  list_t *shelfs;
} item_t;

typedef struct action {
  enum { NOTHING, ADD, REMOVE, EDIT } type;
  item_t *item;
} action_t;

void print_menu() {
  printf("\nMeny\n"
         "--------------------------\n"
         "[L]ägg till en vara\n"
	 "[T]a bort en vara\n"
	 "[R]edigera en vara\n"
	 "Ån[g]ra senaste ändringen\n"
	 "Lista [h]ela varukatalogen\n"
	 "[A]vsluta\n"
         "--------------------------\n");
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

void delete_shelf(shelf_t *shelf) {
  free(shelf);
}

shelf_t *make_shelf(char *shelfname, int amount) {
  shelf_t *s = malloc(sizeof(shelf_t));
  if (s) {
    s -> shelfname = shelfname;
    s -> amount = amount;
  }
  return s;
}

void delete_item(item_t *item) {
  list_t *list = item -> shelfs;
  list_delete(list, (list_action) delete_shelf);
  free(item);
}

item_t *make_item(char *name, char *desc, int price, char *shelfname, int amount) {
  list_t *shelflist = list_new();
  shelf_t *shelf = make_shelf(shelfname, amount);
  list_append(shelflist, shelf);
  item_t *i = malloc(sizeof(item_t));
  
  if (i) {
    i -> name = name;
    i -> desc = desc;
    i -> price = price;
    i -> shelfs = shelflist;
  }
  return i; 
}

item_t *copy_item(item_t *item) {
  char *name = item -> name; 
  char *desc = item -> desc;
  int price = item -> price;
  list_t *shelfs = item -> shelfs;
  shelf_t *shelf = *list_first(shelfs);
  int amount = shelf -> amount;
  char *shelfname = shelf -> shelfname;

  item_t *copy = make_item(name, desc, price, shelfname, amount);

  int length = list_length(shelfs);
  for (int i=1; i<length; i++) {
    shelf = *list_get(shelfs, i);
    shelf = make_shelf(shelf -> shelfname, shelf -> amount);
    list_insert(copy -> shelfs, i, shelf);
  }
  
  return copy;
}

bool shelf_exists(tree_t *db, char* shelfname) {
  int size = tree_size(db);
  item_t **shelfarray = (item_t **)tree_elements(db);
  
  for (int i = 0; i < size; i++) { //Loopar items
    list_t *shelfs = shelfarray[i] -> shelfs;
    int l = list_length(shelfs);
    
    for (int j = 0; j < l; j++) { //Loopar hyllorna
      shelf_t *link = *list_get(shelfs, j);
      
      if (!strcmp(link -> shelfname, shelfname)) {
        return true;
      }
    }
  }
  return false;
}

void print_item(item_t *item) {
  char *name = item -> name;
  char *desc = item -> desc;
  int price = item -> price;
  list_t *list = item -> shelfs;
  
  printf("\n-----------------------------------------------\n");
  printf("Namn: %s\n", name);
  printf("Beskrivning: %s\n", desc);

  int kr = price / 100;
  int ore = price % 100;
  if (ore > 9) {
    printf("Pris: %d.%d kr\n", kr, ore);
  } else {
    printf("Pris: %d.0%d kr\n", kr, ore);
  }
  int l = list_length(list);
  
  for (int i=0; i<l; i++) {
    shelf_t *p = *list_get(list, i);
    char *shelf = p -> shelfname;
    int antal = p -> amount;
    printf("Hylla: %s, Antal: %d\n", shelf, antal);
  }
  printf("-----------------------------------------------\n");
}

item_t *edit_item(tree_t *db, item_t *item, bool edit_name, action_t *undo) {
  if (item == NULL) return NULL;
  print_item(item);
  printf("\nVilken del vill du redigera?\n");
  if (edit_name) printf("[N]amn\n");
  printf("[B]eskrivning\n"
         "[P]ris\n"
         "[L]agerhylla\n"
         "An[t]al\n");
  char val;

  char *str = "BPLTA";
  if (edit_name) str = "NBPLTA";
  do {
    val = toupper(ask_question_char("Välj rad eller [a]vbryt: "));
  } while (strchr(str, val) == NULL);

  if (val != 'A' && !edit_name) {
    undo -> type = EDIT;
    if (undo -> item) {
      delete_item(undo -> item);
    }
    undo -> item = copy_item(item);
  }
  
  if (val == 'N' && edit_name) {
    printf("\nNuvarande namn: %s\n", item -> name);
    printf("-----------------------------------------------\n");
    item -> name = ask_question_string("Nytt namn: ");

    while (tree_has_key(db, item -> name)) {
      printf("Namnet '%s' är upptaget\n", item -> name);
      item -> name = ask_question_string("Nytt namn: ");
    }
    printf("\nNamnet har ändrats\n");

  } else if (val == 'B') {
    printf("\nNuvarande beskrivning: %s\n", item -> desc);
    printf("-----------------------------------------------\n");
    item -> desc = ask_question_string("Ny beskrivning: ");
    printf("\nBeskrivningen har ändrats\n");
    
  } else if (val == 'P') {
    int price = item -> price;
    int kr = price / 100;
    int ore = price % 100;

    if (ore > 9) {
      printf("\nNuvarande pris: %d.%d kr\n", kr, ore);

    } else {
      printf("\nNuvarande pris: %d.0%d kr\n", kr, ore);
    }
    
    printf("-----------------------------------------------\n");
    item -> price = ask_question_int("Nytt pris i ören: ");
    printf("\nPriset har ändrats\n");
    
  } else if (val == 'L') {
    list_t *list = item -> shelfs;
    int l = list_length(list);

    for (int i=0; i<l; i++) {
      shelf_t *p = *list_get(list, i);
      char *shelf = p -> shelfname;
      int antal = p -> amount;
      printf("%d. Hylla: %s, Antal: %d\n", i+1, shelf, antal);
    }
    
    do {
      val = (ask_question_int("Vilken hylla vill du ändra? ") - 1);
    } while (val >= l || val < 0);
    
    printf("-----------------------------------------------\n");
    shelf_t *p = *list_get(list, val);
    char *newshelf = ask_question_shelf("Ny hylla: ");
          
    while (shelf_exists(db, newshelf)) {
      printf("Hyllan %s är upptagen, vänligen välj ny hylla\n", newshelf);
      newshelf = ask_question_shelf("Ny hylla: ");      
    }
    
    p -> shelfname = newshelf; 				
    printf("\nLagerhyllan har ändrats\n");
    
  } else if (val == 'T'){
    list_t *list = item -> shelfs;
    int l = list_length(list);

    for (int i = 0; i < l; i++) {
      shelf_t *p = *list_get(list, i);
      char *shelf = p -> shelfname;
      int antal = p -> amount;
      printf("%d. Hylla: %s, Antal: %d\n", i+1 , shelf, antal);
    }
    
    do {
      val = (ask_question_int("Vilken hylla vill du ändra antal på? ") - 1);
    } while (val >= l || val < 0);
    
    printf("-----------------------------------------------\n");
    shelf_t *p = *list_get(list, val);
    p -> amount = ask_question_int("Nytt antal: ");

    while (p -> amount <= 0) {
      printf("Du måste lägga till minst en vara\n");
      p -> amount = ask_question_int("Antal: ");     
    }
    printf("\nAntal har ändrats\n");
  }
  
  return item;
}

void add_item_to_db(tree_t *db, action_t *undo) {
  char *desc;
  char *shelfname;
  int amount;
  int price;
  item_t *item;
  list_t *list;
  printf("\nLägg till en ny vara\n");
  printf("====================\n");
  char *name = ask_question_string("Namn: ");
  
  if (tree_has_key(db, name)) {
    printf("%s finns redan i databasen.\n"
           "Använder samma pris och beskrivning.\n", name);
    item = tree_get(db, name);
    price = item -> price;
    desc = item -> desc;
    list = item -> shelfs;

    item_t *olditem = copy_item(item);
    
    shelfname = ask_question_shelf("Hylla: ");
    while (shelf_exists(db, shelfname)) {
      printf("Hyllan %s är upptagen, vänligen välj ny hylla\n", shelfname);
      shelfname = ask_question_shelf("Hylla: ");      
    }
    
    amount = ask_question_int("Antal: ");
    while (amount <= 0) {
      printf("Du måste lägga till minst en vara\n");
      amount = ask_question_int("Antal: ");     
    }
    
    shelf_t *elem = make_shelf(shelfname, amount);
    list_append(list, elem);

    undo -> type = EDIT;
    if (undo -> item) {
      delete_item(undo -> item);
    }
    undo -> item = olditem;
    return;
    
  } else {
    desc  = ask_question_string("Beskrivning: ");
    price = ask_question_int("Pris i öre: ");
    shelfname = ask_question_shelf("Hylla: ");
    
    while (shelf_exists(db, shelfname)) {
      printf("Hyllan %s är upptagen, vänligen välj ny hylla\n", shelfname);
      shelfname = ask_question_shelf("Hylla: ");      
    }
    
    amount = ask_question_int("Antal: ");
    while (amount == 0) {
      printf("Du kan inte lägga till noll varor...\n");
      amount = ask_question_int("Antal: ");     
    }
    
    item_t *newitem = make_item(name, desc, price, shelfname, amount);
    char jnr = 'k';
    
    while (true) {
      print_item(newitem);
      jnr = toupper(ask_question_char("Vill du lägga till varan? ([J]a, [N]ej, [R]edigera) "));
      if (jnr == 'J') {
        tree_insert(db, name, newitem);
        printf("%s ligger nu i databasen\n", name);

        undo -> type = ADD;
        if (undo -> item) {
          delete_item(undo -> item);
        }
        undo -> item = copy_item(newitem);
        return;
        
      } else if (jnr == 'N') {
        delete_item(newitem);
        return;
        
      } else if (jnr == 'R') {
        newitem = edit_item(db, newitem, true, undo);
        name = newitem -> name;
      }
    }
  }
}

item_t *choose_item(tree_t *db, char *text) {
  int length = tree_size(db);
  int sida = 1;
  int per_sida = 20;
  
  if (length < 1) {
    printf("Databasen är tom, det finns inga varor %s\n", text);
    return NULL;
    
  } else {
    char **namn = tree_keys(db);
    int i=1;
    while (true) {
      printf("\nAlla varor, sida %d\n", sida);
      printf("==================\n");
      
      for (i=1; (sida-1)*per_sida + i-1<length && i<=per_sida; i++) {
        printf("%d. %s\n", i, namn[(sida-1)*per_sida + i-1]);
      }
      
      printf("\n[V]älj en vara %s\n", text);
      
      if (length > sida*per_sida) { //if (Det finns en till sida)
        printf("[N]ästa sida\n");
      }
      if (sida > 1) { //if (Det finns en föregående sida)
        printf("[F]öregående sida\n");
      }
      printf("[A]vbryt\n");
      char val = toupper(ask_question_char("Vad vill du göra? "));
      
      if (val == 'V') {
        break;
	
      } else if (val == 'N' && length > sida*per_sida) {
        sida++;
	
      } else if (val == 'F' && sida > 1) {
        sida--;
	
      } else if (val == 'A') {
        free(namn);
        return NULL;
      }
      
    }
    int vara = ask_question_int("Välj vara (siffra): ");
    
    while ((sida-1)*per_sida + vara-1 >= length || (sida-1)*per_sida + vara-1 < 0) {
      vara = ask_question_int("Välj vara (siffra): ");
    }
    
    char *key = namn[(sida-1)*per_sida + vara-1];
    item_t *item = tree_get(db, key);
    free(namn);
    
    return item;
  }
}

void remove_item_from_db(tree_t *db, action_t *undo) {
  item_t *item = choose_item(db, "att ta bort");
  if (item == NULL) {
    return;
  }
  tree_remove(db, item -> name);
  printf("Vara '%s' är nu borttagen\n", item->name);

  undo -> type = REMOVE;
  if (undo -> item) {
    delete_item(undo -> item);
  }
  undo -> item = copy_item(item);
  delete_item(item);
}

void list_db(tree_t *db) {
  item_t *item = choose_item(db, "att se");
  
  if (item) {
    print_item(item);
  }
}

void edit_db(tree_t *db, action_t *undo) {
  item_t *item = choose_item(db, "att ändra");
  if (item == NULL) return;
  edit_item(db, item, false, undo);
}

void undo_action(tree_t *db, action_t *undo) {
  
  if (undo -> type == NOTHING) {
    printf("Det finns inget att ångra\n");
    
  } else if (undo -> type == ADD) {
    char *key = undo -> item -> name;
    item_t *olditem = tree_remove(db, key);
    delete_item(olditem);
    delete_item(undo -> item);

    printf("Du har ångrat att lägga till %s, den är nu borttagen\n", key);
    
    undo -> item = NULL;
    undo -> type = NOTHING;
    
  } else if (undo -> type == REMOVE) {
    char *key = undo -> item -> name;
    tree_insert(db, key, undo -> item);
    
    printf("Du har ångrat att ta bort %s, den är nu tillbakalagd\n", key);
    
    undo -> item = NULL;
    undo -> type = NOTHING;
    
  } else if (undo -> type == EDIT) {
    char *key = undo -> item -> name;
    item_t *olditem = tree_remove(db, key);
    delete_item(olditem);
    tree_insert(db, key, undo -> item);
    
    printf("Du har ångrat ändringen av %s, den är nu återställd\n", key);

    undo -> item = NULL;
    undo -> type = NOTHING;
  } 
}

void event_loop(tree_t *db, action_t *undo) {
  while (true) {
    char command = ask_question_menu();
    
    if (command == 'L') {
      add_item_to_db(db, undo);
      
    } else if (command == 'T') {
      remove_item_from_db(db, undo);
      
    } else if (command == 'R') {
      edit_db(db, undo);
      
    } else if (command == 'G') {
      undo_action(db, undo);
      
    } else if (command == 'H') {
      list_db(db);
      
    } else if (command == 'A'){
      printf("Avslutar\n");
      return; 
    }
  }
}

void delete_item_2(char *key, item_t *item) {
  delete_item(item);
}

int main(void) {
  println("\nVälkommen till lagerhantering 1.0");
  println("=================================");
  tree_t *db = tree_new();
  action_t *undo = malloc(sizeof(action_t));
  if (undo) {
    undo -> type = NOTHING;
    undo -> item = NULL;
  }
  event_loop(db, undo);
  
  if (undo -> item) {
    delete_item(undo -> item);
  }
  free(undo);
  tree_delete(db, (tree_action) delete_item_2);
}
