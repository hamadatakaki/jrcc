#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "invalid arguments error\n");
    return 1;
  }
  char *p = argv[1];
  int c = 1;

  printf("define i32 @main() {\n");
  printf("  %%%d = alloca i32, align 4\n", c);
  printf("  store i32 %ld, i32* %%%d, align 4\n", strtol(p, &p, 10), c);
  printf("  %%%d = load i32, i32* %%%d, align 4\n", c+1, c);

  while (*p) {
    if (*p == '+') {
      p++; c++;
      printf("  %%%d = add i32 %%%d, %ld\n", c+1, c, strtol(p, &p, 10));
      continue;
    }

    if (*p == '-') {
      p++; c++;
      printf("  %%%d = sub i32 %%%d, %ld\n", c+1, c, strtol(p, &p, 10));
      continue;
    }

    fprintf(stderr, "予期しない文字です: '%c'\n", *p);
    return 1;
  }

  printf("  ret i32 %%%d\n", c+1);
  printf("}\n");
  return 0;
}