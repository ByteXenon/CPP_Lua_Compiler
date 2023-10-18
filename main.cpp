#include <fstream>
#include <iostream>
#include <memory>
#include <stdio.h>
#include <cstring>

#include "lexer.cpp"
#include "parser.cpp"
#include "astExecutor.cpp"


int main() {
  std::string file_contents = "warn('Hello!', 'World!')";
  
  Lexer a(file_contents);
  Parser b(a.getTokens());
  AstExecutor c(std::move(b.getCodeBlock()));
  c.run();
  return 0;
}