#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/calcexpr.h"
void ncopy(Lexer* lexer, const char* source, int start);
int binop(Token token);
void advance(Parser* parser, Token* tokens);
int match(Parser* parser, TokenType type);
AST* parse_expr(Parser* parser, Token* tokens);
AST* parse_term(Parser* parser, Token* tokens);
AST* parse_factor(Parser* parser, Token* tokens);
AST* parse_number(Parser* parser, Token* tokens);
AST* parse_group(Parser* parser, Token* tokens);
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
void lexer_step(Lexer* lexer, const char* source){
  memset(lexer->token.value, 0, lexer->size * sizeof(char));
  lexer->current_char = source[lexer->index];
  while(lexer->current_char == ' ' || lexer->current_char == '\t' || lexer->current_char == '\n'){
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
          lexer_step(lexer, source);
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
      fprintf(stderr, "Undeclared operator being used: %c\n", lexer->current_char);
      lexer->index++;
      lexer_step(lexer, source);
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
void advance(Parser* parser, Token* tokens){
  parser->current_token = tokens[++parser->index];
}
Parser* create_parser(Token* tokens){
  Parser* parser = malloc(sizeof(Parser));
  parser->index = 0;
  parser->current_token = tokens[0];
  return parser;
}
int match(Parser* parser, TokenType type){
  return parser->current_token.type == type;
}
AST* parse_expr(Parser* parser, Token* tokens){
  AST* left = parse_term(parser, tokens);
  if(match(parser, TOKEN_PLUS) || match(parser, TOKEN_MINUS)){
    AST* ast = malloc(sizeof(AST));
    if(!ast){
      fprintf(stderr, "Malloc failed\n");
      return NULL;
    }
    if(match(parser, TOKEN_PLUS)){
      ast->node.type = AST_PLUS;
    }
    else{
      ast->node.type = AST_MINUS;
    }
    advance(parser, tokens);
    ast->left = left;
    ast->right = parse_expr(parser, tokens);
    return ast;
  }
  else{
    return left;
  }
}
AST* parse_term(Parser* parser, Token* tokens){
  AST* left = parse_factor(parser, tokens);
  if(match(parser, TOKEN_STAR) || match(parser, TOKEN_SLASH)){
    AST* ast = malloc(sizeof(AST));
    if(!ast){
      fprintf(stderr, "Malloc failed\n");
      return NULL;
    }
    if(match(parser, TOKEN_STAR)){
      ast->node.type = AST_STAR;
    }
    else{
      ast->node.type = AST_SLASH;
    }
    advance(parser, tokens);
    ast->left = left;
    ast->right = parse_term(parser, tokens);
    return ast;
  }
  else{
    return left;
  }
}
AST* parse_factor(Parser* parser, Token* tokens){
  AST* ast = parse_number(parser, tokens);
  if(ast){
    advance(parser, tokens);
    return ast;
  }
  return parse_group(parser, tokens);
}
AST* parse_number(Parser* parser, Token* tokens){
  if(match(parser, TOKEN_NUMBER)){
    AST* ast = malloc(sizeof(AST));
    ast->left = NULL;
    ast->right = NULL;
    ast->node.type = AST_NUMBER;
    ast->node.value = tokens[parser->index].value;
    return ast;
  }
  else{
    return NULL;
  }
}
AST* parse_group(Parser* parser, Token* tokens){
  if(match(parser, TOKEN_LPAREN)){
    advance(parser, tokens);
    AST* ast = parse_expr(parser, tokens);
    if(match(parser, TOKEN_RPAREN)){
      advance(parser, tokens);
      return ast;
    }
    else{
      fprintf(stderr, "No closing paranthesses\n");
      clean_tools(NULL, NULL, ast);
      return NULL;
    }
  }
  return NULL;
}
double interpret_ast(AST* ast){
  if(ast->node.type == AST_NUMBER){
    return atof(ast->node.value);
  }
  else{
    double num1 = interpret_ast(ast->left);
    double num2 = interpret_ast(ast->right);
    switch(ast->node.type){
      case AST_PLUS:
        return num1 + num2;
      case AST_MINUS:
        return num1 - num2;
      case AST_STAR:
        return num1 * num2;
      case AST_SLASH:
        return num1 / num2;
      default:
        return 0;
    }
  }
}
void clean_tools(Lexer* lexer, Parser* parser, AST* ast){
  if(lexer){
    free(lexer->token.value);
    free(lexer);
    lexer = NULL;
  }
  if(parser){
    free(parser);
    parser = NULL;
  }
  if(ast){
    clean_tools(lexer, parser, ast->left);
    clean_tools(lexer, parser, ast->right);
    free(ast);
    ast = NULL;
  }
}
