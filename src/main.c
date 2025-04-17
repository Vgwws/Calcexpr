#include <stdio.h>
#include <stdlib.h>
#include "../include/calcexpr.h"
#define SIZE 100
void show_ast(AST* ast, int depth){
  if(!ast) return;
  for(int i = 0; i < depth; i++){
    printf(" ");
  }
  printf("|-> ");
  switch(ast->node.type){
    case AST_PLUS:
      printf("+\n");
      break;
    case AST_MINUS:
      printf("-\n");
      break;
    case AST_STAR:
      printf("*\n");
      break;
    case AST_SLASH:
      printf("/\n");
      break;
    case AST_NUMBER:
      printf("%s\n", ast->node.value);
      break;
  }
  show_ast(ast->left, depth + 1);
  show_ast(ast->right, depth + 1);
}
int main(){
  printf("CALCULATOR\n");
  printf("Press `q` to quit program\n");
  Token* tokens = malloc(SIZE * sizeof(Token));
  for(int i = 0; i < SIZE; i++){
    tokens[i].value = calloc(SIZE, sizeof(char));
  }
  while(1){
    printf("Expression: ");
    char source[SIZE];
    fgets(source, SIZE, stdin);
    if(source[0] == 'q'){
      break;
    }
    Lexer* lexer = create_lexer(source, SIZE);
    lexer->token.type = TOKEN_NUMBER;
    for(int i = 0; lexer->token.type != TOKEN_EOF; i++){
      lexer_step(lexer, source);
      tokens[i].type = lexer->token.type;
      strcpy(tokens[i].value, lexer->token.value);
    }
    if(lexer->error_flag){
      clean_tools(lexer, NULL, NULL);
      continue;
    }
    Parser* parser = create_parser(tokens);
    AST* ast = parse_expr(parser, tokens);
    int error_flag = 0;
    double result = interpret_ast(ast, &error_flag);
    if(!error_flag){
      printf("Result: %lf\n", result);
    }
    clean_tools(lexer, parser, ast);
  }
  for(int i = 0; i < SIZE; i++){
    free(tokens[i].value);
  }
  free(tokens);
  return 0;
}
