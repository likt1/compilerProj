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
// symbol tables
// error list
// error flags (parse error list and if there is a large error then disable 
// lexer

int main(int argc, const char * argv[]) {
  std::cout << "hello world" << "\n";

  return 0;
}
