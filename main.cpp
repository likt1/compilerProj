// MAIN.CPP for compiler

#include "lexer.h"

#define symbol_table std::unordered_map<const char*, token_type>
// define a symbol table list to work with scoping?
// we will always need a local scope and global scope, while there is a block
//   list, only check first and last ones

// job object for a sample job TODO
struct record {
  token_type tokenType;
};

// Globals
// =====================================================
// symbol tables
// error list
error_list errList;
// error flags (parse error list and if there is a large error then disable 
// lexer

// Funcs
// =====================================================
void printTok(tok &token) {
  std::cout << token.tokenType << " ";
  std::cout << token.name << "\n";
  std::cout << "L: " << token.linePos << "|C: " << token.charPos << "\n";
  
  /* TODO
  token_type tokenType;
  std::string name;
  union {
    int i;
    float f;
    char c;
    symbol_type s;
    keyword_type r;
  };
  int linePos; // line of first char
  int charPos; // loc of first char
  */
}

// Main
// =====================================================
int main(int argc, const char *argv[]) {
  std::cout << "hello world" << "\n";
  
  if (argc == 2) {
    lexer scanner;
    scanner.init(argv[1], &errList);
    tok token;
    if (scanner.next_tok(token)) {
      // end of file do something
    }
    scanner.deinit();
  } else {
    std::cout << "too many or too little args!";
  }

  return 0;
}
