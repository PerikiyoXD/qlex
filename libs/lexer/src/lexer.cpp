#include "lexer.hpp"

#include <iostream>

Lexer::Lexer(const std::string &source_code)
    : impl(std::make_unique<LexerImpl>(source_code)) {}

Lexer::~Lexer() = default;

LexingStatus Lexer::tokenize() { return impl->tokenize(); }

void Lexer::printTokens() const { impl->printTokens(); }

LexerImpl::LexerImpl(const std::string &source_code)
    : source_code(source_code), current_position(0), current_line(1) {}

LexingStatus LexerImpl::tokenize() {
  LexingStatus status = LexingStatus::ERROR;

  while (!isAtEnd()) {
    skipWhitespace();
    char c = advance();
    switch (c) {
    case '(':
      addToken(Token::Type::LEFT_PAREN);
      break;
    case ')':
      addToken(Token::Type::RIGHT_PAREN);
      break;
    case '{':
      addToken(Token::Type::LEFT_BRACE);
      break;
    case '}':
      addToken(Token::Type::RIGHT_BRACE);
      break;
    case ',':
      addToken(Token::Type::COMMA);
      break;
    case '.':
      addToken(Token::Type::DOT);
      break;
    case '-':
      addToken(Token::Type::MINUS);
      break;
    case '+':
      addToken(Token::Type::PLUS);
      break;
    case ';':
      addToken(Token::Type::SEMICOLON);
      break;
    case '*':
      addToken(Token::Type::STAR);
      break;
    case '!':
      addToken(peek() == '=' ? Token::Type::BANG_EQUAL : Token::Type::BANG);
      break;
    case '=':
      addToken(peek() == '=' ? Token::Type::EQUAL_EQUAL : Token::Type::EQUAL);
      break;
    case '<':
      addToken(peek() == '=' ? Token::Type::LESS_EQUAL : Token::Type::LESS);
      break;
    case '>':
      addToken(peek() == '=' ? Token::Type::GREATER_EQUAL
                             : Token::Type::GREATER);
      break;
    case '/':
      if (peek() == '/') {
        skipComment();
      } else if (peek() == '*') {
        skipMultilineComment();
      } else {
        addToken(Token::Type::SLASH);
      }
      break;
    case ' ':
    case '\r':
    case '\t':
      break;
    case '\n':
      current_line++;
      break;
    case '"':
      addToken(Token::Type::STRING, scanString());
      break;
    default:
      if (std::isdigit(c)) {
        addToken(Token::Type::NUMBER, scanNumber());
      } else if (std::isalpha(c)) {
        addToken(Token::Type::IDENTIFIER, scanIdentifier());
      } else {
        error("Unexpected character");
      }
      break;
    }
  }

  status = LexingStatus::SUCCESS;

  return status;
}

char LexerImpl::peek() const {
  if (current_position < source_code.size()) {
    return source_code[current_position];
  }
  return '\0';
}

char LexerImpl::advance() {
  if (current_position < source_code.size()) {
    return source_code[current_position++];
  }
  return '\0';
}

void LexerImpl::skipWhitespace() {
  while (true) {
    char c = peek();
    switch (c) {
    case ' ':
    case '\r':
    case '\t':
      advance();
      break;
    case '\n':
      current_line++;
      advance();
      break;
    case '/':
      if (peek() == '/') {
        skipLine();
      } else if (peek() == '*') {
        skipMultilineComment();
      } else {
        return;
      }
      break;
    default:
      return;
    }
  }
}

void LexerImpl::skipComment() {
  while (peek() != '\n' && !isAtEnd()) {
    advance();
  }
}

void LexerImpl::skipLine() {
  while (peek() != '\n' && !isAtEnd()) {
    advance();
  }
}

void LexerImpl::skipMultilineComment() {
  while (true) {
    if (peek() == '*' && peek() == '/') {
      advance();
      advance();
      return;
    }
    if (peek() == '\n') {
      current_line++;
    }
    advance();
  }
}

bool LexerImpl::isAtEnd() const {
  return current_position >= source_code.size();
}

void LexerImpl::addToken(Token::Type type, const std::string &lexeme) {
  std::string token_lexeme = lexeme.empty() ? std::string(1, peek()) : lexeme;
  tokens.push_back(Token{type, token_lexeme, current_line});
}

void LexerImpl::error(const std::string &message) {
  std::cerr << "Lexer error: " << message << " at line " << current_line
            << std::endl;
  exit(1);
}

const std::string LexerImpl::scanString() {
  int start = current_position;

  while (peek() != '"' && !isAtEnd()) {
    if (peek() == '\n') {
      current_line++;
    }
    advance();
  }

  if (isAtEnd()) {
    error("Unterminated string");
  }

  advance(); // Consume closing "

  return source_code.substr(start + 1, current_position - start - 2);
}

const std::string LexerImpl::scanNumber() {
  int start = current_position;

  while (std::isdigit(peek())) {
    advance();
  }

  return source_code.substr(start, current_position - start);
}

const std::string LexerImpl::scanIdentifier() {
  int start = current_position;

  while (std::isalnum(peek())) {
    advance();
  }

  return source_code.substr(start, current_position - start);
}

void LexerImpl::printTokens() const {
  for (const auto &token : tokens) {
    std::cout 
        << "Token: " << token.lexeme 
        << " Type: " << static_cast<int>(token.type) 
        << " Line: " << token.line 
        << std::endl;
  }
}