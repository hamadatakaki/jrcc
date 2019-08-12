#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
  TK_RESERVED,
  TK_NUM,
  TK_EOF
} TokenKind;

typedef struct Token Token;

struct Token {
  TokenKind kind;
  Token *next;
  int val;
  char *str;
};

Token *token;

void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

bool consume(char op) {
  if(token->kind != TK_RESERVED || token->str[0] != op) {
    return false;
  }
  token = token->next;
  return true;
}

void expect(char op) {
  if(token->kind != TK_RESERVED || token->str[0] != op) {
    error("'%c' ではありません", op);
  }
  token = token->next;
}

int expect_number() {
  if(token->kind != TK_NUM) {
    error("数ではありません");
  }
  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof() {
  return token->kind == TK_EOF;
}

Token *new_token(TokenKind kind, Token *cur, char *str) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  cur->next = tok;
  return tok;
}

Token *tokenize(char *p) {
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while(*p) {
    if(isspace(*p)) {
      p++;
      continue;
    }

    if(*p == '+' || *p == '-') {
      cur = new_token(TK_RESERVED, cur, p++);
      continue;
    }

    if(isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p);
      cur->val = strtol(p, &p, 10);
      continue;
    }

    error(p, "トークナイズできません");
  }

  new_token(TK_EOF, cur, p);
  return head.next;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "invalid arguments error\n");
    return 1;
  }

  token = tokenize(argv[1]);

  int c = 1;

  printf("define i32 @main() {\n");
  printf("  %%%d = alloca i32, align 4\n", c);
  printf("  store i32 %d, i32* %%%d, align 4\n", expect_number(), c);
  printf("  %%%d = load i32, i32* %%%d, align 4\n", c+1, c);

  while (!at_eof()) {
    if(consume('+')) {
      c++;
      printf("  %%%d = add i32 %%%d, %d\n", c+1, c, expect_number());
      continue;
    }

    if(consume('-')) {
      c++;
      printf("  %%%d = sub i32 %%%d, %d\n", c+1, c, expect_number());
      continue;
    }
  }

  printf("  ret i32 %%%d\n", c+1);
  printf("}\n");
  return 0;
}