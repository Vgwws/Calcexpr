#ifndef CALCEXPR_H
#define CALCEXPR_H
#include <stdlib.h>
typedef enum {
  TOKEN_NUMBER,
  TOKEN_PLUS,
  TOKEN_MINUS,
  TOKEN_STAR,
  TOKEN_SLASH,
  TOKEN_MOD,
  TOKEN_LSHIFT,
  TOKEN_RSHIFT,
  TOKEN_LT,
  TOKEN_LE,
  TOKEN_GE,
  TOKEN_GT,
  TOKEN_EQ,
  TOKEN_NE,
  TOKEN_AND_BITWISE,
  TOKEN_OR_BITWISE,
  TOKEN_XOR_BITWISE,
  TOKEN_LOGICAL_AND,
  TOKEN_LOGICAL_OR,
  TOKEN_LPAREN,
  TOKEN_RPAREN,
  TOKEN_EOF
} TokenType;
typedef struct {
  TokenType type;
  char* value;
} Token;
typedef struct {
  Token token;
  int index;
  char current_char;
  size_t size;
  int error_flag;
} Lexer;
typedef struct {
  int index;
  Token current_token;
} Parser;
typedef enum {
  AST_NUMBER,
  AST_PLUS,
  AST_MINUS,
  AST_STAR,
  AST_SLASH,
  AST_MOD,
  AST_LT,
  AST_LE,
  AST_GE,
  AST_GT,
  AST_EQ,
  AST_NE,
  AST_LSHIFT,
  AST_RSHIFT,
  AST_LOGICAL_AND,
  AST_LOGICAL_OR,
  AST_AND_BITWISE,
  AST_OR_BITWISE,
  AST_XOR_BITWISE
} ASTNodeType;
typedef struct {
  ASTNodeType type;
  char* value;
} ASTNode;
typedef struct AST {
  ASTNode node;
  struct AST* left;
  struct AST* right;
} AST;
Lexer* create_lexer(const char* source, size_t size);
Parser* create_parser(Token* tokens);
void lexer_step(Lexer* lexer, const char* source);
AST* main_parse(Parser* parser, Token* tokens);
double interpret_ast(AST* ast, int* error_flag);
void clean_tools(Lexer* lexer, Parser* parser, AST* ast);
#endif
