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
std::string cleanDblQuotes(std::string str) {
  std::string out;
  for (size_t i = 0; i < str.size(); i++) {
    if (str[i] != '"') {
      out.push_back(str[i]);
    }
  }
  return out;
}

void printTok(tok &token) {
  std::cout << "(L: " << token.linePos << "|C: " << token.charPos << ") ";
  std::cout << scanner.typeToString(token.tokenType) << "\t[";
  std::cout << token.name << "]";
  switch (token.tokenType) {
    case type_int:
      std::cout << "-> " << token.i;
      break;
    case type_float:
      std::cout << "-> " << token.f;
      break;
    case type_char:
      std::cout << "-> " << token.c;
      break;
    case type_symb:
      std::cout << "-> " << scanner.symbToString(token.s);
      break;
  }
  std::cout << "\n";
}

bool getTok(tok &token) {
  if (!scanner.next_tok(token)) {
    genCode = false;
  } else {
    if (token.tokenType == type_string) {
      token.name = cleanDblQuotes(token.name);
    }
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
  std::cout << "Compiler start" << "\n";
  
  if (argc == 2) {
    scanner.init(argv[1], &errList);
    tok token;
    
    int debugCnt = 50;
    while (debugCnt > 0) {
      if (getTok(token)) {
        printTok(token);
      } else {
        break;
      }
      debugCnt--;
    }
    
    scanner.deinit();
  } else {
    std::cout << "Too many or too little args!";
  }

  return 0;
}
