#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

extern char *strdup(const char *);

typedef union {
  int   i;
  float f;
  char *s;
  char  c; 
} answer_t;

typedef bool(*check_func)(char *);
typedef answer_t(*convert_func)(char *);


void print(char *str);

void println(char *str);

int read_string(char *buf, int buf_siz);

bool is_number(char *str);

bool not_empty(char *str);

answer_t ask_question(char *question, check_func check, convert_func convert);

bool is_char(char *str);

char *ask_question_string(char *question);

int ask_question_int(char *question);

char ask_question_char(char *question);


#endif
