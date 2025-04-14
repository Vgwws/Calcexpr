#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/calcexpr.h"
void ncopy(Lexer* lexer, const char* source, int start);
int binop(Token token);
void advance(Parser* parser, Token* tokens);
int match(Parser* parser, TokenType type);
AST* parse_expr(Parser* parser, Token* tokens, size_t size);
AST* parse_term(Parser* parser, Token* tokens, size_t size);
AST* parse_factor(Parser* parser, Token* tokens, size_t size);
AST* parse_number(Parser* parser, Token* tokens, size_t size);
AST* parse_group(Parser* parser, Token* tokens, size_t size);
void ncopy(Lexer* lexer, const char* source, int start){
  strncpy(lexer->token.value, source + start, lexer->index - start);
  lexer->token.value[lexer->index - start] = '\0';
}
Lexer* create_lexer(const char* source, size_t size){
  Lexer* lexer = malloc(sizeof(Lexer));
  lexer->token.value = calloc(size, sizeof(char));
  lexer->index = 0;
  lexer->current_char = source[0];
  lexer->size = size;
  return lexer;
}
void lexer_step(Lexer* lexer, const char* source, size_t inc){
  memset(lexer->token.value, 0, lexer->size * sizeof(char));
  lexer->current_char = source[lexer->index];
  while(lexer->current_char == ' ' || lexer->current_char == '\t'){
    lexer->current_char = source[++lexer->index];
  }
  if(isdigit(lexer->current_char)){
    int start = lexer->index;
    int dot_count = 0;
    while(isdigit(lexer->current_char) || lexer->current_char == '.'){
      if(lexer->current_char == '.'){
        if(dot_count++){
          lexer->error_flag = 1;
          fprintf(stderr, "Invalid floating point format given\n");
          lexer->index++;
          lexer_step(lexer, source, 0);
          return;
        }
      }
      lexer->current_char = source[++lexer->index];
    }
    ncopy(lexer, source, start);
    lexer->token.type = TOKEN_NUMBER;
    return;
  }
  switch(lexer->current_char){
    case '\0':
      lexer->token.type = TOKEN_EOF;
      break;
    case '+':
      lexer->token.type = TOKEN_PLUS;
      break;
    case '-':
      lexer->token.type = TOKEN_MINUS;
      break;
    case '*':
      lexer->token.type = TOKEN_STAR;
      break;
    case '/':
      lexer->token.type = TOKEN_SLASH;
      break;
    case '(':
      lexer->token.type = TOKEN_LPAREN;
      break;
    case ')':
      lexer->token.type = TOKEN_RPAREN;
      break;
    default:
      lexer->error_flag = 1;
      fprintf(stderr, "Undeclared operator being used\n");
      lexer->index++;
      lexer_step(lexer, source, 0);
      return;
  }
  lexer->token.value[0] = lexer->current_char;
  lexer->token.value[1] = '\0';
  lexer->index++;
  return;
}
/*
  Expr = Term (+|-) Expr | Term
  Term = Factor (*|/) Term | Factor
  Factor = Number | Group
  Number = [0-9]+ ('.' [0-9]*)?
  Group = '(' Expr ')'
  2 + 3 * 5 -> +
              / \
             2   *
                / \
               3   5
*/
int binop(Token token){
  if(token.type == TOKEN_PLUS
  || token.type == TOKEN_MINUS
  || token.type == TOKEN_STAR
  || token.type == TOKEN_SLASH){
    return 1;
  }
  return 0;
}
void advance(Parser* parser, Token* tokens){
  parser->current_token = tokens[++parser->index];
}
Parser* create_parser(Token* tokens, size_t size){
  Parser* parser = malloc(sizeof(Parser));
  parser->index = 0;
  parser->size = size;
  parser->current_token = tokens[0];
  return parser;
}
AST* main_parse(Parser* parser, Lexer* lexer, Token* tokens, size_t size){
  AST* ast = malloc(sizeof(AST));
  parse_expr(parser, tokens, size);
  if(!match(parser, TOKEN_EOF)){
    fprintf(stderr, "Grammar error\n");
    lexer->error_flag = 1;
  }
  return ast;
}
int match(Parser* parser, TokenType type){
  return parser->current_token.type == type;
}
AST* parse_expr(Parser* parser, Token* tokens, size_t size){
  AST* ast = malloc(sizeof(AST));
  ast->left = NULL;
  ast->right = NULL;
  ast->left = parse_term(parser, tokens, size);
  if(match(parser, TOKEN_PLUS) || match(parser, TOKEN_MINUS)){
    advance(parser, tokens);
  }
  else{
    fprintf(stderr, "Invalid grammar\n");
    free(ast);
    return NULL;
  }
  ast->right = parse_expr(parser, tokens, size);
}
