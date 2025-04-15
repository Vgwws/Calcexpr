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
  Token* tokens = malloc(SIZE * sizeof(Token));
  for(int i = 0; i < SIZE; i++){
    tokens[i].value = calloc(SIZE, sizeof(char));
  }
  int i = 0;
  do{
    lexer_step(lexer, source);
    tokens[i].type = lexer->token.type;
    strcpy(tokens[i++].value, lexer->token.value);
  }while(lexer->token.type != TOKEN_EOF);
  Parser* parser = create_parser(tokens);
  AST* ast = parse_expr(parser, tokens);
  printf("Result: %lf\n", interpret_ast(ast));
  clean_tools(lexer, parser, ast);
  free(tokens);
  return 0;
}
