#pragma once

#include <algorithm>
#include <cctype>
#include <cstdarg>
#include <cstring>
#include <iostream>
#include <memory>
#include <optional>
#include <vector>

typedef enum {
#undef EOF
  EOF,
  IDENTIFIER,
  KEYWORD,
  NUMBER,
  STRING,
  CHARACTER
} TokenType;

struct Token {
  int TYPE;
  std::string Value;

  Token(int type) : TYPE(type){};
  Token(int type, std::string val) : TYPE(type), Value(val){};
};

Token EOF_TOKEN = Token(EOF);
std::vector<std::string> KEYWORD_STRINGS = {"do", "end", "while"};

class Lexer {
private:
  char peek(size_t n = 1) { return charStream[curCharIndex + n]; }

  char consume(size_t n = 1) {
    curCharIndex += n;
    return curChar = charStream[curCharIndex];
  }

  std::string getCharRange(size_t min = 0, size_t max = 0) {
    return charStream.substr(curCharIndex + min, max - min + 1);
  }

  bool compareCharRange(const std::string &str, size_t offset = 0) {
    return str == getCharRange(offset, offset + str.size() - 1);
  }

  std::string readWhileCharacter(char character, int offset = 0) {
    size_t start = curCharIndex + offset;
    size_t end = start;
    while (charStream[end] == character) {
      ++end;
    }
    return charStream.substr(start, end - start);
  }

  char expectAndConsume(char character) {
    if (curChar != character) {
      std::cerr << "Unexpected character: " << curChar
                << ", expected: " << character << '\n';
      exit(1);
    }
    return consume();
  }

  bool isString() {
    return curChar == '\'' || curChar == '"' ||
           (curChar == '[' && (peek() == '=' || peek() == '['));
  }

  bool isComment() { return curChar == '-' && (peek() == '-'); }

  inline bool isIdentifier(char character) {
    return std::isalpha(character) || character == '_';
  }
  inline bool isIdentifier() { return isIdentifier(curChar); }

  inline bool isDigit(char character) { return std::isdigit(character); }
  inline bool isDigit() { return isDigit(curChar); }

  inline bool isWhitespace(char character) { return std::isspace(character); }
  inline bool isWhitespace() { return isWhitespace(curChar); }

  std::string readComment() {
    std::string comment;
    if (consume(2) != '[') {
    }

    return comment;
  }

  std::string readIdentifier() {
    std::string identifier;
    identifier += curChar;

    char nextChar = peek();
    while (std::isalnum(nextChar) || nextChar == '_') {
      identifier += consume();
      nextChar = peek();
    }

    return identifier;
  }

  std::string readComplexString() {
    std::string result;

    int depth = 0;
    expectAndConsume('[');
    while (curChar == '=') {
      depth++;
      consume();
    }
    expectAndConsume('[');

    std::string closingPattern = std::string(depth, '=') + ']';
    for (;;) {
      if (!curChar) {
        throw std::runtime_error("Unexpected end of string");
      }
      if (curChar == ']' && compareCharRange(closingPattern, 1)) {
        consume(closingPattern.length());
        break;
      }
      result += curChar;
      consume();
    }
    return result;
  }

  std::string readSimpleString() {
    std::string newString;

    char startChar = curChar;
    consume();

    while (curChar != startChar) {
      newString += curChar;
      consume();
    }
    return newString;
  }

  std::string readString() {
    if (curChar == '\'' || curChar == '"')
      return readSimpleString();
    return readComplexString();
  }

  std::string readNumber() {
    std::string numStr;

    while (isdigit(curChar)) {
      numStr += curChar;
      if (isDigit(peek())) {
        consume();
      } else {
        break;
      }
    }
    return numStr;
  }

  void readWhitespace() {
    if (isWhitespace()) {
      while (isWhitespace(peek())) {
        consume();
      }
    }
  }

public:
  std::string charStream;
  size_t curCharIndex;
  char curChar;
  Lexer(std::string stream)
      : charStream(stream), curCharIndex(0),
        curChar(stream.empty() ? '\0' : stream[0]) {}

  std::optional<Token> getToken() {
    if (isWhitespace()) {
      readWhitespace();
      return std::nullopt;
    } else if (isIdentifier()) {
      std::string newIdentifier = readIdentifier();
      if (std::find(KEYWORD_STRINGS.begin(), KEYWORD_STRINGS.end(),
                    newIdentifier) != KEYWORD_STRINGS.end()) {
        return Token(KEYWORD, newIdentifier);
      }
      return Token(IDENTIFIER, newIdentifier);
    } else if (isDigit()) {
      return Token(NUMBER, readNumber());
    } else if (isString()) {
      return Token(STRING, readString());
    } else {
      return Token(CHARACTER, std::string(1, curChar));
    }
  }

  std::vector<Token> getTokens() {
    std::vector<Token> tokens;
    while (curChar != '\0') {
      std::optional<Token> nextToken = getToken();
      if (nextToken) {
        tokens.push_back(*nextToken);
      }
      consume();
    }
    return tokens;
  }

  void printTokens() {
    std::vector<Token> tokens = getTokens();
    for (const auto &token : tokens) {
      std::cout << "Token: '" << token.Value << "', Type: " << token.TYPE
                << std::endl;
    }
  }
};