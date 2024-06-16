#include <lexer.hpp>

#include <iostream>

int main() {
    Lexer lexer("print \"Hello, World!\";");
    lexer.tokenize();
    lexer.printTokens();

    return 0;
}