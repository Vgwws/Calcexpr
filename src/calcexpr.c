#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/calcexpr.h"
int is_binop(char ch);
int is_ident(char ch);
int is_ident_part(char ch);
void ncopy(Lexer* lexer, const char* source, int start);
int binop(Token token);
void advance(Parser* parser, Token* tokens);
int match(Parser* parser, TokenType type);
int match_comp(Parser* parser);
AST* parse_comp(Parser* parser, Token* tokens);
AST* parse_expr(Parser* parser, Token* tokens);
AST* parse_term(Parser* parser, Token* tokens);
AST* parse_factor(Parser* parser, Token* tokens);
AST* parse_number(Parser* parser, Token* tokens);
AST* parse_group(Parser* parser, Token* tokens);
int is_ident(char ch){
  return isalpha(ch) || ch == '_';
}
int is_ident_part(char ch){
  return isalnum(ch) || ch == '_';
}
int is_binop(char ch){
  return ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '<' || ch == '=' || ch == '>';
}
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
  if(isdigit(lexer->current_char) || (lexer->current_char == '-' && isdigit(source[lexer->index + 1]))){
    int start = lexer->index;
    if(lexer->current_char == '-'){
      lexer->current_char = source[++lexer->index];
    }
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
  if(is_binop(lexer->current_char)){
    int start = lexer->index;
    char prev = lexer->current_char;
    lexer->current_char = source[++lexer->index];
    switch(prev){
      case '<':
        if(lexer->current_char == '<'){
          lexer->token.type = TOKEN_LSHIFT;
          lexer->index++;
        }
        else if(lexer->current_char == '='){
          lexer->token.type = TOKEN_LE;
          lexer->index++;
        }
        else{
          lexer->token.type = TOKEN_LT;
        }
        break;
      case '=':
        if(lexer->current_char == '='){
          lexer->token.type = TOKEN_EQ;
          lexer->index++;
        }
        break;
      case '>':
        if(lexer->current_char == '>'){
          lexer->token.type = TOKEN_RSHIFT;
          lexer->index++;
        }
        else if(lexer->current_char == '='){
          lexer->token.type = TOKEN_GE;
          lexer->index++;
        }
        else{
          lexer->token.type = TOKEN_GT;
        }
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
    }
    ncopy(lexer, source, start);
    return;
  }
  switch(lexer->current_char){
    case '\0':
      lexer->token.type = TOKEN_EOF;
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
int match_comp(Parser* parser){
  return match(parser, TOKEN_LT) || match(parser, TOKEN_LE) || match(parser, TOKEN_EQ) || match(parser, TOKEN_GE) || match(parser, TOKEN_GT);
}
AST* parse_comp(Parser* parser, Token* tokens){
  AST* left = parse_expr(parser, tokens);
  if(match_comp(parser)){
    AST* ast = malloc(sizeof(AST));
    if(!ast){
      fprintf(stderr, "Malloc failed\n");
      return NULL;
    }
    switch(parser->current_token.type){
      case TOKEN_LT:
        ast->node.type = AST_LT;
        break;
      case TOKEN_LE:
        ast->node.type = AST_LE;
        break;
      case TOKEN_EQ:
        ast->node.type = AST_EQ;
        break;
      case TOKEN_GE:
        ast->node.type = AST_GE;
        break;
      case TOKEN_GT:
        ast->node.type = AST_GT;
        break;
      default:
        break;
    }
    advance(parser, tokens);
    ast->left = left;
    ast->right = parse_comp(parser, tokens);
    return ast;
  }
  return left;
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
    ast->node.value = parser->current_token.value;
    return ast;
  }
  else{
    return NULL;
  }
}
AST* parse_group(Parser* parser, Token* tokens){
  if(match(parser, TOKEN_LPAREN)){
    advance(parser, tokens);
    AST* ast = parse_comp(parser, tokens);
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
double interpret_ast(AST* ast, int* error_flag){
  if(*error_flag){
    return 0;
  }
  if(ast->node.type == AST_NUMBER){
    return atof(ast->node.value);
  }
  else{
    double num1 = interpret_ast(ast->left, error_flag);
    double num2 = interpret_ast(ast->right, error_flag);
    switch(ast->node.type){
      case AST_PLUS:
        return num1 + num2;
      case AST_MINUS:
        return num1 - num2;
      case AST_STAR:
        return num1 * num2;
      case AST_SLASH:
        if(num2 == 0){
          fprintf(stderr, "ERROR: Can't divide by zero!\n");
          *error_flag = 1;
          return 0;
        }
        return num1 / num2;
      case AST_LT:
        return num1 < num2;
      case AST_LE:
        return num1 <= num2;
      case AST_EQ:
        return num1 == num2;
      case AST_GE:
        return num1 >= num2;
      case AST_GT:
        return num1 > num2;
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
