#pragma once

#include <string>
#include <memory>
#include <vector>

class LexerImpl;

struct Token {
    enum class Type {
        // Single-character tokens
        LEFT_PAREN, RIGHT_PAREN,
        LEFT_BRACE, RIGHT_BRACE,
        COMMA, DOT, MINUS, PLUS,
        SEMICOLON, SLASH, STAR,

        // One or two character tokens
        BANG, BANG_EQUAL,
        EQUAL, EQUAL_EQUAL,
        GREATER, GREATER_EQUAL,
        LESS, LESS_EQUAL,

        // Literals
        IDENTIFIER, STRING, NUMBER,

        // Keywords
        AND, CLASS, ELSE, FALSE,
        FUN, FOR, IF, NIL, OR,
        PRINT, RETURN, SUPER, THIS,
        TRUE, VAR, WHILE,

        // End of file
        END_OF_FILE
    };

    Type type;
    std::string lexeme;
    int line;
};

enum class LexingStatus {
    SUCCESS,
    ERROR
};

class Lexer {
public:
    Lexer(const std::string& source_code);
    virtual ~Lexer();

    virtual LexingStatus tokenize();
    virtual void printTokens() const;

private:
    std::unique_ptr<LexerImpl> impl;
};

// LexerImpl class definition
class LexerImpl {
public:
    LexerImpl(const std::string& source_code);
    virtual ~LexerImpl() = default;

    virtual LexingStatus tokenize();

    virtual void printTokens() const;

private:
    const std::string source_code;
    std::size_t current_position;
    int current_line;
    std::vector<Token> tokens;

    char peek() const;
    char advance();
    void skipWhitespace();
    void skipComment();
    void skipLine();
    void skipMultilineComment();
    bool isAtEnd() const;

    void addToken(Token::Type type, const std::string& lexeme = "");

    void error(const std::string& message);

    const std::string scanString();
    const std::string scanNumber();
    const std::string scanIdentifier();

};