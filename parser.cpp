#pragma once

#include <algorithm>
#include <cctype>
#include <cstdarg>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

#include "lexer.cpp"

class Node {
public:
  std::string type;
  Node* parent;

  Node(const std::string &type, Node *parent = nullptr)
      : type(type), parent(parent) {}
};

class FunctionCall : public Node {
public:
  std::string function;
  std::vector<std::string> arguments;

  FunctionCall(std::string function, std::vector<std::string> arguments,
               Node *parent = nullptr)
      : Node("FunctionCall", parent), function(function), arguments(arguments) {}
};

class DoBlock : public Node {
public:
  std::vector<std::unique_ptr<Node>> codeBlock;

  DoBlock(std::vector<std::unique_ptr<Node>> codeBlock, Node *parent = nullptr)
      : Node("DoBlock", parent), codeBlock(std::move(codeBlock)) {}
};

class Parser {
#define SAFELY_GET(n)                                                          \
  tokenStream.size() > (long unsigned int)(n) ? tokenStream[n] : EOF_TOKEN

private:
  Token peek(size_t n = 1) { return SAFELY_GET(curTokenIndex + n); }
  Token consume(size_t n = 1) {
    curTokenIndex += n;
    curToken = SAFELY_GET(curTokenIndex);
    return curToken;
  }
  Token expect(TokenType type, std::string value = "", bool consumeCurToken = true) {
    if (curToken.TYPE != type || (!value.empty() && curToken.Value != value)) {
      throw std::runtime_error(
          "Unexpected Token, Expected: {TYPE: " + std::to_string(type) +
          ", Value: " + value + "}" + ", Got: {TYPE: " +
          std::to_string(curToken.TYPE) + ", Value: " + curToken.Value + "}");
    }
    if (consumeCurToken)
      consume();
    return curToken;
  }

public:
  std::vector<Token> tokenStream;
  Token curToken;
  int curTokenIndex = 0;
  
  Parser(std::vector<Token> tokens)
      : tokenStream(tokens), curToken(tokenStream[0]) {}

  std::unique_ptr<Node> getNode(const std::vector<std::string> &stopKeywords = {}) {
    switch (curToken.TYPE) {
    case IDENTIFIER: {
      std::string function = curToken.Value;
      consume(); // Consume curToken
      consume(); // Consume "("
      std::vector<std::string> arguments;
      while (curToken.TYPE == STRING) {
        arguments.push_back(curToken.Value);
        Token nextChar = consume();
        if (nextChar.TYPE == CHARACTER && nextChar.Value == ",") {
          consume();
          expect(STRING, "", false);
        } else if (nextChar.TYPE == CHARACTER && nextChar.Value == ")") {
          break;
        } else {
          throw std::runtime_error("Error");
        }
      }
      return std::make_unique<FunctionCall>(function, arguments);
    }
    case KEYWORD: {
      if (std::find(stopKeywords.begin(), stopKeywords.end(), curToken.Value) !=
          stopKeywords.cend()) {
        return nullptr;
      }
      if (curToken.Value == "do") {
        consume();
        return std::make_unique<DoBlock>(getCodeBlock({"end"}));
      }
      break;
    }
    case EOF:
      break;
    default:
      throw std::runtime_error("Unexpected token");
    }
    return nullptr;
  };
  
  std::vector<std::unique_ptr<Node>> getCodeBlock(const std::vector<std::string> &stopKeywords = {}) {
    std::vector<std::unique_ptr<Node>> nodes;
    while (curToken.TYPE != EOF) {
      nodes.push_back(getNode(stopKeywords));
      consume();
    }
    return nodes;
  }
};
