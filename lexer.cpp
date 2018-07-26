#include "lexer.h"


//====================== Init functions ======================//
lexer::lexer() {
}

lexer::~lexer() {
}

bool lexer::init(char* fn, symbol_table* gbTbl, error_list* el) {
  globalT = gbTbl;
  errL = el;
  fs.open(fn, std::fstream::in);
  
  curLine = 0; curChar = 0;
  tokCursor = 0;

  return true;
}
  
  
bool lexer::deinit() {
  fs.close();
  return true;
}
  
void lexer::reportError(err_type eT, char* msg) {
  error_obj newError;
  newError.errT = eT;
  newError.msg = msg;
  newError.lineNum = curLine;
  newError.charNum = curChar;

  errL->push_back(newError);
  std::cout << "New Error! "; 
  std::cout << newError.errT << " ";
  std::cout << newError.msg << " ";
  std::cout << newError.lineNum << " ";
  std::cout << newError.charNum << "\n";
}
  
tok lexer::next_tok(symbol_table* loc_tbl) {
  tok out;
  if (tokMem.size() > tokCursor) {
    // grab next tokMem (since cursor is 1 ahead, grab the current tok)
    out = tokMem.at(tokCursor);
  }
  else {
    // get next token 
    char nxtChar;
    fs.get(nxtChar);
    
    /*
    struct tok {
      token_type tokenType;
      union {
        int i;
        symbol_type s;
        char* name;
      };
      int linePos;
      int charPos;
    };
    */
    
    // TODO state machine to gen tokens
    
    
    
    out.name = nxtChar; // TODO temporary
    
    // add to tokMem
    tokMem.push_back(out);
  }
  
  tokCursor++;
  return out;
}

void lexer::undo() {
  tokCursor--;
  if (tokCursor < 0) {
    std::cout << "tokCursor is negative!!!";
  }
}
