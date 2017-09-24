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

void delete_shelf(shelf_t *shelf) //cleanup funktion
{
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
  list_delete(list,  (list_action) delete_shelf);
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

bool shelf_exists(tree_t *db, char* str) {
	/*
	kollar igenom hela trädet och alla listor och returnerar false om str inte hittas som shelfname. Returnerar true om den existerar. 
	*/
	int size = tree_size(db);
	item_t *elements = *tree_elements(db);
  
	for (int i = 0; i < size; i++){
		item_t shelfs = elements[i] -> shelfs;
		int l = list_length(shelfs);
		for (int a = 0; a < l; a++) {
			shelf_t link = list_get(shelfs, a);
			if (link -> shelfname == str) {
				return true;
			}				; 
		}
	}	
   
	return false;
}

void print_item(item_t *item) {
  char *name = item -> name;
  char *desc = item -> desc;
  int price = item -> price;
  list_t *list = item -> shelfs;
  printf("\nNamn: %s\n", name);
  printf("Beskrivning: %s\n", desc);

  int kr = price / 100;
  int ore = price % 100;
  printf("Pris: %d.%d kr\n", kr, ore);

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
    
		while (shelf_exists(db, shelfname)) {
			printf("Hyllan %s är upptagen, vänligen välj ny hylla\n", shelfname);
			shelfname = ask_question_shelf("Hylla: ");      
		}
	
		amount = ask_question_int("Antal: ");
		shelf_t *elem = make_shelf(shelfname, amount);
		list_append(list, elem);
    
	} else {
		desc  = ask_question_string("Beskrivning: ");
		price = ask_question_int("Pris: ");
		shelfname = ask_question_shelf("Hylla: ");

		while (shelf_exists(db, shelfname)) {
			printf("Hyllan %s är upptagen, vänligen välj ny hylla\n", shelfname);
			shelfname = ask_question_shelf("Hylla: ");      
		}
    
		amount = ask_question_int("Antal: ");
		item_t *newitem = make_item(name, desc, price, shelfname, amount);
		char jnr = 'k';
		
		while (true) {
			do {
				print_item(newitem);
				jnr = toupper(ask_question_char("Vill du lägga till varan? ([J]a, [N]ej, [R]edigera) "));
				if (jnr == 'J') {
					tree_insert(db, name, newitem);
					return;
				} else if (jnr == 'N') {
					return;
				} else if (jnr == 'R') {
					char val;
					do {
						val = toupper(ask_question_char("Vilken del vill du redigera?"
														"\n[N]amn\n"
														"\n[B]eskrivning\n"
														"[P]ris\n"
														"[L]agerhylla\n"
														"An[t]al\n"
														"\nVälj rad eller [a]vbryt: "));
					}while (strchr("NBPLTA", val) == NULL);
					
					shelf_t *data = *list_first(newitem -> shelfs);
					
					if (val == 'N') {
						printf("\nNuvarande namn: %s\n", newitem -> name);
						printf("-----------------------------------------------\n");
						newitem -> name = ask_question_string("Nytt namn: ");
						printf("\nNamnet har ändrats\n");
						
					} else if (val == 'B') {
						printf("\nNuvarande beskrivning: %s\n", newitem -> desc);
						printf("-----------------------------------------------\n");
						newitem -> desc = ask_question_string("Ny beskrivning: ");
						printf("\nBeskrivningen har ändrats\n");
				
					} else if (val == 'P') {
						int price = newitem -> price;
						int kr = price / 100;
						int ore = price % 100;
						printf("Nuvarande pris: %d.%d kr\n", kr, ore);
						printf("-----------------------------------------------\n");
						newitem -> price = ask_question_int("Nytt pris i ören: ");
						printf("\nPriset har ändrats\n");
			
					} else if (val == 'L') {
						printf("\nNuvarande hylla: %s\n", data -> shelfname);
						printf("-----------------------------------------------\n");
						data -> shelfname = ask_question_string("Ny hylla: ");
						printf("\nLagerhyllan har ändrats/n");
				
					} else if (val == 'T') {
						printf("\nNuvarande antal: %d\n", data -> amount);
						printf("-----------------------------------------------\n");
						data -> amount = ask_question_int("Nytt antal: ");
						printf("\nAntal har ändrats/n");
			
					} else if (val == 'A') {
						break;
					}
				}
			} while (jnr == 'R');
		}
	}
	return;
}

void remove_item_from_db(tree_t *db) {
  if (tree_size(db) < 1) {
    printf("Databasen är tom, det finns inga varor att ta bort\n");
  } else {
    char *key = ask_question_string("Vilken vara vill du ta bort? ");
    if (tree_has_key(db, key)) {
      tree_remove(db, key);
      printf("Vara '%s' är nu borttagen", key);
    } else {
      printf("Varan finns inte i databasen\n");
    }
  }
  //return tree_remove(db, key); --------------------------FIX LATER!
}


void list_db_2(K key, T elem, void *data) {
  print_item(elem);
  printf("\n");
}

void list_db(tree_t *db) {
  int length = tree_size(db);
  int sida = 1;
  int per_sida = 4;
  
  if (length < 1) {
    printf("Databasen är tom, det finns inga varor att lista\n");
  } else {
    char **namn = tree_keys(db);
    int i=1;
    while (true) {
      printf("\nAlla varor, sida %d\n", sida);
      
      for (i=1; (sida-1)*per_sida + i-1<length && i<=per_sida; i++) {
        printf("\n%d. %s", i, namn[(sida-1)*per_sida + i-1]);
      }
      
      printf("\n\n[S]e en vara\n");
      
      if (length > sida*per_sida) { //if (Det finns en till sida)
        printf("[N]ästa sida\n");
      }
      if (sida > 1) { //if (Det finns en föregående sida)
        printf("[F]öregående sida\n");
      }
      printf("[A]vbryt\n");
      char val = toupper(ask_question_char("Vad vill du göra? "));
      if (val == 'S') {
        break;
      } else if (val == 'N' && length > sida*per_sida) {
        sida++;
      } else if (val == 'F' && sida > 1) {
        sida--;
      } else if (val == 'A') {
        free(namn);
        return;
      }
    }
    int vara = ask_question_int("Vilken vara vill du se? Välj siffra: ");
    while ((sida-1)*per_sida + vara-1 >= length || (sida-1)*per_sida + vara-1 < 0) {
      vara = ask_question_int("Vilken vara vill du se? Välj siffra: ");
    }
    char *key = namn[(sida-1)*per_sida + vara-1];
    item_t *item = tree_get(db, key);
    print_item(item);
    
    free(namn);
  }
}

void edit_db(tree_t *db) {
  int length = tree_size(db);
  int sida = 1;
  int per_sida = 4;
  
  if (length < 1) {
    printf("Databasen är tom, det finns inga varor att redigera\n");
  } else {
    char **namn = tree_keys(db);
    int i=1;
    while (true) {
      printf("\nAlla varor, sida %d\n", sida);
      
      for (i=1; (sida-1)*per_sida + i-1<length && i<=per_sida; i++) {
        printf("\n%d. %s", i, namn[(sida-1)*per_sida + i-1]);
      }
      
      printf("\n\n[S]e en vara\n");
      
      if (length > sida*per_sida) { //if (Det finns en till sida)
        printf("[N]ästa sida\n");
      }
      if (sida > 1) { //if (Det finns en föregående sida)
        printf("[F]öregående sida\n");
      }
      printf("[A]vbryt\n");
      char val = toupper(ask_question_char("Vad vill du göra? "));
      if (val == 'S') {
        break;
      } else if (val == 'N' && length > sida*per_sida) {
        sida++;
      } else if (val == 'F' && sida > 1) {
        sida--;
      } else if (val == 'A') {
        free(namn);
        return;
      }
    }
    int vara = ask_question_int("Vilken vara vill du se? Välj siffra: ");
    while ((sida-1)*per_sida + vara-1 >= length || (sida-1)*per_sida + vara-1 < 0) {
      vara = ask_question_int("Vilken vara vill du se? Välj siffra: ");
    }
    char *key = namn[(sida-1)*per_sida + vara-1];
    item_t *item = tree_get(db, key);
    print_item(item);
	printf("-----------------------------------------------\n");
	printf("\n[B]eskrivning\n"
		"[P]ris\n"
		"[L]agerhylla\n"
		"An[t]al\n");
		char val = toupper(ask_question_char("Välj rad eller [a]vbryt: "));
	while (strchr("BPLTA", val) == NULL) {
		val = toupper(ask_question_char("Felaktig inmatning.\n"
										"\n[B]eskrivning\n"
										"[P]ris\n"
										"[L]agerhylla\n"
										"An[t]al\n"
										"\nVälj rad eller [a]vbryt: "));
	}
		if (val == 'B') {
			printf("\nNuvarande beskrivning: %s\n", item -> desc);
			printf("-----------------------------------------------\n");
			item -> desc = ask_question_string("Ny beskrivning: ");
			printf("\nBeskrivningen har ändrats\n");
			
		} else if (val == 'P') {
			int price = item -> price;
			int kr = price / 100;
			int ore = price % 100;
			printf("Nuvarande pris: %d.%d kr\n", kr, ore);
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
				printf("%d. Hylla: %s, Antal: %d\n", i + 1, shelf, antal);
			}	
			do {
				val = (ask_question_int("Vilken hylla vill du ändra?: ") - 1);
			}while (val >= l || val <= 0);
			
			printf("-----------------------------------------------\n");
			shelf_t *p = *list_get(list, val);
			char *newshelf;
			do {
				newshelf = ask_question_string("Ny hylla: ");
			}while (shelf_exists(db, newshelf) && !(shelf_is_valid(newshelf)));
			p -> shelfname = newshelf; 				
			printf("\nLagerhyllan har ändrats/n");
			
		} else if (val == 'T'){
			list_t *list = item -> shelfs;
			int l = list_length(list);
			for (int i = 0; i < l; i++) {
				shelf_t *p = *list_get(list, i);
				char *shelf = p -> shelfname;
				int antal = p -> amount;
				printf("%d. Hylla: %s, Antal: %d\n", i +1 , shelf, antal);
			}	
			do {
				val = (ask_question_int("Vilken hylla vill du ändra antal på?: ") - 1);
			}while (val >= l || val <= 0);
			
			printf("-----------------------------------------------\n");
			shelf_t *p = *list_get(list, val);
			p -> amount = ask_question_int("Nytt antal: ");
			printf("\nAntal har ändrats/n");	
	
		} else {
			free(namn);
			return;
		}
	free(namn);
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
      edit_db(db);
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
