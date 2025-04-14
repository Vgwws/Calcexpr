#ifndef CALCEXPR_H
#define CALCEXPR_H
#include <stdlib.h>
typedef enum {
  TOKEN_NUMBER,
  TOKEN_PLUS,
  TOKEN_MINUS,
  TOKEN_STAR,
  TOKEN_SLASH,
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
  size_t size;
} Parser;
typedef enum {
  AST_NUMBER,
  AST_PLUS,
  AST_MINUS,
  AST_STAR,
  AST_SLASH
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
Parser* create_parser(Token* tokens, size_t size);
void lexer_step(Lexer* lexer, const char* source, size_t inc);
AST* main_parse(Parser* parser, Lexer* lexer, Token* tokens, size_t size);
double interpret_ast(AST* ast);
void clean_tools(Lexer* lexer, Parser* parser, AST* ast);
#endif
