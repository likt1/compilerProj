// MAIN.CPP for compiler

#include "lexer.h"

#include <stack>

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
symbol_table globalST;
std::stack<symbol_table> localSTs;
// error list
error_list errList;
// error flags (parse error list and if there is a large error then disable
bool genCode = true;
// lexer
lexer scanner;

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
    symb_type s;
    key_type r;
  };
  int linePos; // line of first char
  int charPos; // loc of first char
  */
}

bool getTok(tok &token) {
  if (!scanner.next_tok(token)) {
    genCode = false;
  }
  if (scanner.fileEnd) { // end of file abort
    std::cout << "End of file reached";
    return false;
  }
  return true;
}

// Main
// =====================================================
int main(int argc, const char *argv[]) {
  std::cout << "hello world" << "\n";
  
  if (argc == 2) {
    scanner.init(argv[1], &errList);
    tok token;
    
    int debugCnt = 50;
    while (debugCnt > 0) {
      if (getTok(token)) {
        printTok(token);
      }
      debugCnt--;
    }
    
    scanner.deinit();
  } else {
    std::cout << "Too many or too little args!";
  }

  return 0;
}
