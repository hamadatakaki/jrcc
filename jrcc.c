#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * tokenizer
 */

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
char *user_input;

void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, "");
  fprintf(stderr, "^");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// unused function.
// bool at_eof() {
//   return token->kind == TK_EOF;
// }

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

    // fprintf(stderr, "token: %c\n", *p);  // DEBUG: print tokens

    if(*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')') {
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

/*
 * parser
 */

typedef enum {
  ND_ADD,
  ND_SUB,
  ND_MUL,
  ND_DIV,
  ND_NUM
} NodeKind;

typedef struct Node Node;

struct Node {
  NodeKind kind;
  Node *lhs;
  Node *rhs;
  int val;
};

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
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
    error_at(token->str, "'%c' ではありません", op);
  }
  token = token->next;
}

int expect_number() {
  if(token->kind != TK_NUM) {
    error_at(token->str, "数ではありません");
  }
  int val = token->val;
  token = token->next;
  return val;
}

Node *expr();
Node *mul();
Node *term();

Node *expr() {
  Node *node = mul();

  while(1) {
    if(consume('+'))
      node = new_node(ND_ADD, node, mul());
    else if(consume('-'))
      node = new_node(ND_SUB, node, mul());
    else 
      return node;
  }
}

Node *mul() {
  Node *node = term();

  while(1) {
    if(consume('*'))
      node = new_node(ND_MUL, node, term());
    else if(consume('/'))
      node = new_node(ND_DIV, node, term());
    else
      return node;
  }
}

Node *term() {
  if(consume('(')) {
    Node *node = expr();
    expect(')');
    return node;
  }

  return new_node_num(expect_number());
}

/*
 * asm generate
 */

typedef int ad_ptr_t;
static ad_ptr_t apc = 1; // IR paramater's address counter

#define GEN_STACK_SIZE 256
typedef ad_ptr_t stack_data_t;
stack_data_t paramater_stack[GEN_STACK_SIZE];
int sp = 0; // stack pointer

void push(stack_data_t data) {
  if(sp<GEN_STACK_SIZE) {
    paramater_stack[sp] = data;
    sp++;
  } else {
    perror("stack over flow!\n");
  }
}

void pop(stack_data_t *data) {
  if(sp>0) {
    sp--;
    *data = paramater_stack[sp];
  } else if(sp==0) {
    printf("stack is empty!\n");
    exit(1);
  } else {
    printf("stack pointer is not negative number. but sp = %d\n", sp);
    exit(1);
  }
}

void gen_op(NodeKind kind, ad_ptr_t first, ad_ptr_t second) {
  switch(kind) {
    case ND_ADD:
      printf("  %%%d = add nsw i32 %%%d, %%%d\n", apc, first, second);
      break;
    case ND_SUB:
      printf("  %%%d = sub nsw i32 %%%d, %%%d\n", apc, first, second);
      break;;
    case ND_MUL:
      printf("  %%%d = mul nsw i32 %%%d, %%%d\n", apc, first, second);
      break;
    case ND_DIV:
      printf("  %%%d = sdiv i32 %%%d, %%%d\n", apc, first, second);
      break;
    default:
      printf("the handled kind is not OP.\n");
      exit(1);
  }
  push(apc++);
}

void gen(Node *node) {
  if(node->kind == ND_NUM) {
    printf("  %%%d = alloca i32, align 4\n", apc);
    printf("  store i32 %d, i32* %%%d, align 4\n", node->val, apc);
    printf("  %%%d = load i32, i32* %%%d, align 4\n", apc+1, apc);
    apc++;
    push(apc++);
    return;
  }

  gen(node->lhs);
  gen(node->rhs);

  ad_ptr_t second;
  ad_ptr_t first;
  pop(&second);
  pop(&first);

  gen_op(node->kind, first, second);
}

/*
 * main function
 */

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "invalid arguments error\n");
    return 1;
  }

  user_input = argv[1];
  token = tokenize(user_input);
  Node *node = expr();

  printf("define i32 @main() {\n");
  gen(node);

  printf("  ret i32 %%%d\n", apc-1);
  printf("}\n");
  return 0;
}
