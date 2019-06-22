#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
  TK_NUM = 256,
  TK_EOF,
};

typedef struct {
  int ty;
  int val;
  char *input;
} Token;

char *user_input;
Token tokens[100];

void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

void error_at(char *loc, char *msg) {
  int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, "");
  fprintf(stderr, "^ %s\n", msg);
  exit(1);
}

void tokenize() {
  char *p = user_input;
  int i = 0;
  while(*p) {
    if(isspace(*p)) {
      p++;
      continue;
    }

    if(*p == '+' || *p == '-') {
      tokens[i].ty = *p;
      tokens[i].input = p;
      i++; p++;
      continue;
    }

    if(isdigit(*p)) {
      tokens[i].ty = TK_NUM;
      tokens[i].input = p;
      tokens[i].val = strtol(p, &p, 10);
      i++;
      continue;
    }

    error_at(p, "トークナイズできません");
  }

  tokens[i].ty = TK_EOF;
  tokens[i].input = p;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "invalid arguments error\n");
    return 1;
  }

  user_input = argv[1];
  tokenize();

  int c = 1;
  int i = 0;

  if (tokens[0].ty != TK_NUM)
    error_at(tokens[0].input, "数ではありません");

  printf("define i32 @main() {\n");
  printf("  %%%d = alloca i32, align 4\n", c);
  printf("  store i32 %d, i32* %%%d, align 4\n", tokens[i++].val, c);
  printf("  %%%d = load i32, i32* %%%d, align 4\n", c+1, c);

  while (tokens[i].ty != TK_EOF) {
    if(tokens[i].ty == '+') {
      c++;
      if (tokens[++i].ty != TK_NUM)
        error_at(tokens[i].input, "数ではありません");
      printf("  %%%d = add i32 %%%d, %d\n", c+1, c, tokens[i++].val);
      continue;
    }

    if(tokens[i].ty == '-') {
      c++;
      if (tokens[++i].ty != TK_NUM)
        error_at(tokens[i].input, "数ではありません");
      printf("  %%%d = sub i32 %%%d, %d\n", c+1, c, tokens[i++].val);
      continue;
    }

    error_at(tokens[i].input, "予期しないトークンです");
  }

  printf("  ret i32 %%%d\n", c+1);
  printf("}\n");
  return 0;
}