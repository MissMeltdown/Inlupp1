#include "utils.h"

void print(char *str) {
  for (int i=0; str[i] != '\0'; i++)
    putchar(str[i]);
}

void println(char *str) {
  print(str);
  print("\n");
}

int read_string(char *buf, int buf_siz) {
  int count = 0;
  int c;
  do {
    c = getchar();
    buf[count] = c;
    count++;
  } while (c != '\n' && c != EOF && count < buf_siz);

  while (c != '\n' && c != EOF) {
    c = getchar();
  }
  buf[count-1] = '\0';
  return count-1;
}

bool is_number(char *str) {
  int x = strlen(str);
  
  if (!isdigit(str[0]) && str[0] != '-') return false;
  
  for (int i=1; i<x; i++) {
    if (!isdigit(str[i])) return false;
  }
  return true;
}

// Hjälpfunktion till ask_question_string
bool not_empty(char *str) {
  return strlen(str) > 0;
}

bool is_char (char *str) {
  if (strlen(str) != 1) {
    return false;
  } else if (is_number(str)) {
    return false;
  } else {
    return true;
  }
}

bool shelf_is_valid(char *str) {
  int x = strlen(str);
  if (x < 2) return false;
  if (isdigit(str[0])) return false;
  for (int i=1; i<x; i++) if (!isdigit(str[i])) return false;

  return true;
}

answer_t ask_question(char *question, check_func check, convert_func convert) {
  char result[255];
  int l = 0;
  print(question);
  l = read_string(result, 255);
  
  while (l<1 || !check(result)) {
    printf("Felaktig inmatning: '%s'\n", result);
    print(question);
    l = read_string(result, 255);
  }
  
  return convert(result);
}

char *ask_question_string(char *question) {
  return ask_question(question, not_empty, (convert_func) strdup).s;
}

int ask_question_int(char *question) {
  return ask_question(question, is_number, (convert_func) atoi).i; 
}

char str_to_char(char *str) {
  return str[0];
}

char ask_question_char(char *question) {
  return ask_question(question, is_char, (convert_func) str_to_char).c;
}

char *ask_question_shelf(char *question) {
  return ask_question(question, shelf_is_valid, (convert_func) strdup).s;
}
