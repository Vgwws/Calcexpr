#include <stdio.h>
#include <stdlib.h>
#include "../include/calcexpr.h"
#define SIZE 100
int main(){
  printf("CALCULATOR\n");
  printf("Expression: ");
  char source[SIZE];
  fgets(source, SIZE, stdin);
  Lexer* lexer = create_lexer(source, SIZE);
  lexer->token.type = TOKEN_NUMBER;
  Token* tokens = malloc(SIZE * sizeof(Token));
  for(int i = 0; i < SIZE; i++){
    tokens[i].value = calloc(SIZE, sizeof(char));
  }
  for(int i = 0; lexer->token.type != TOKEN_EOF; i++){
    lexer_step(lexer, source);
    tokens[i].type = lexer->token.type;
    strcpy(tokens[i++].value, lexer->token.value);
  }
  if(lexer->error_flag){
    clean_tools(lexer, NULL, NULL);
    for(int i = 0; i < SIZE; i++){
      free(tokens[i].value);
    }
    free(tokens);
    return 1;
  }
  Parser* parser = create_parser(tokens);
  AST* ast = parse_expr(parser, tokens);
  int error_flag = 0;
  double result = interpret_ast(ast, &error_flag);
  if(!error_flag){
    printf("Result: %lf\n", result);
  }
  clean_tools(lexer, parser, ast);
  for(int i = 0; i < SIZE; i++){
    free(tokens[i].value);
  }
  free(tokens);
  return error_flag;
}
