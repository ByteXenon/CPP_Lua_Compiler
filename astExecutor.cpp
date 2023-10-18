#pragma once

#include <algorithm>
#include <cctype>
#include <cstdarg>
#include <cstring>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

#include "lexer.cpp"
#include "parser.cpp"

void LUA_print(std::vector<std::string> arguments) {
  for (auto &argument : arguments) {
    std::cout << argument;
  }
  std::cout << std::endl;
}
void LUA_warn(std::vector<std::string> arguments) {
  std::cout << "\033[33m";
  LUA_print(arguments);
  std::cout << "\033[0m";
}

class AstExecutor {
private:
  std::unordered_map<std::string, std::function<void(std::vector<std::string>)>>
      functions;

public:
  std::vector<std::unique_ptr<Node>> ast;
  AstExecutor(std::vector<std::unique_ptr<Node>> &&ast) : ast(std::move(ast)) {
    #define ADD_FUNCTION(map, func) map[#func] = LUA_##func

    // Map standard library functions
    ADD_FUNCTION(functions, print);
    ADD_FUNCTION(functions, warn);
  };

  void run() {
    for (const auto &node : ast) {
      if (node->type == "FunctionCall") {
        FunctionCall *functionCall = (static_cast<FunctionCall *>(node.get()));

        std::string functionName = functionCall->function;
        std::vector<std::string> arguments = functionCall->arguments;

        if (functions.find(functionName) != functions.end()) {
          functions[functionName](arguments);
        } else {
          std::cout << "Error: Function '" << functionName << "' not found."
                    << std::endl;
        }
      } else {
        throw std::runtime_error("Unsupported node type: " + node->type);
      }
    }
  }
};
