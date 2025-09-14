#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Kinds of Token
typedef enum {
  TK_RESERVED, // simbols
  TK_NUM, 	// integer token
  TK_EOF,		// End of imput token
} TokenKind;

typedef struct Token Token;

// Token type
struct Token {
  TokenKind kind;	// type of token
  Token *next;	// next input token
  int val;	// if kind = TK_NUM, its number
  char *str;	// string of token
};

// Currently focused token
Token *token;

// Function to report error
// takes same args as printf
void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// If the next token is the expected simbol, read one more token
// and returns true.  Otherwise, returns False.
bool consume(char op) {
  if (token->kind != TK_RESERVED || token->str[0] != op)
    return false;
  token = token->next;
  return true;
}

// If the next token is the expected simbole, read one more token.
// Otherwise, return error.
void expect(char op) {
  if (token->kind != TK_RESERVED || token->str[0] != op)
    error("'Not %c'", op);
  token = token->next;
}

// If the next token is number, read one more token and return it.
// Otherwise, return error.
int expect_number() {
  if (token->kind != TK_NUM)
    error("Not number.");
  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof() {
  return token->kind == TK_EOF;
}

// Generate new token and link to cur
Token *new_token(TokenKind kind, Token *cur, char *str) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  cur->next = tok;
  return tok;
}

// Tokenize the input string p and return it
Token *tokenize(char *p) {
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while (*p) {
    // Skip blank
    if (isspace(*p)) {
      p++;
      continue;
    }

    if (*p == '+' || *p == '-') {
      cur = new_token(TK_RESERVED, cur, p++);
      continue;
    }

    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p);
      cur->val = strtol(p, &p, 10);
      continue;
    }

    error("Cannot tokenize.");
  }

  new_token(TK_EOF, cur, p);
  return head.next;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Incorrect number of argc");
    return 1;
  }

  // Tokenize
  token = tokenize(argv[1]);

  // Output the first half of assembly
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // The first expression should be number.  Check it
  // and output the first mov command.
  printf("  mov rax, %d\n", expect_number());

  // Output assembly consuming token '+ <num>' or '- <num>'

  while (!at_eof()) {
    if (consume('+')) {
      printf("  add rax, %d\n", expect_number());
      continue;
    }

    expect('-');
    printf("  sub rax, %d\n", expect_number());
  }

  printf("  ret\n");
  printf(".section .note.GNU-stack,\"\",@progbits\n");
  return 0;
}

