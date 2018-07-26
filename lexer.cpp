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
  
  curLine = 1; curChar = 0;
  storedLine = 0; storedChar = 0;
  tokCursor = 0;
  backtracking = false;
  
  // TODO populate globalT with reserved words

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
  if (backtracking) {
    // grab next tokMem (since cursor is 1 ahead, grab the current tok)
    out = tokMem.at(tokCursor);
    
    curLine = out.linePos;
    curChar = out.charPos;
  }
  else {
    int state = 0;
    bool fileComplete = false;
    bool whitespace = false;
    std::string fullToken;
    do {
      char nxtChar;
      if (!fs.get(nxtChar)) {
        fileComplete = true;
        whitespace = true;
      }
      curChar++;
      
      switch (nxtChar) {
        case '\n':
          curLine++;
          curChar = 0;
        case '\t':
        case ' ':
          whitespace = true;
          break;
      }
      
      // TODO state machine to gen tokens
      
      
      // case statement for states
      switch (state) {
        case 0: // nothing's happened yet
          // if nxtChar is something
          // fullToken.append(nxtChar);
          // set state depending on nxtChar (switch on nxtChar)
          // out.linePos = curLine;
          // out.charPos = curChar;
          break;
        case 1:
          
          break;
        case 2:
          break;
      }
      
      // before moving to ending state -1, set tokenType, name, and union
      /*struct tok {
        token_type tokenType;
        std::string name;
        union {
          int i;
          float f;
          symbol_type s;
          reserved_type r;
        };
        int linePos;
        int charPos;
      };
      */
      
    } while (state != -1);
    
    if (!whitespace) {
      fs.unget();
      curChar--;
    }
    
    // done
    out.name = "temp"; // TODO temporary
    
    // add to tokMem
    tokMem.push_back(out);
  }
  
  tokCursor++;
  
  if (backtracking && tokCursor == tokMem.size()) {
    backtracking == false;
    curLine = storedLine;
    curChar = storedChar;
  }
  
  return out;
}

void lexer::undo() {
  tokCursor--;
  if (tokCursor < 0) {
    std::cout << "tokCursor is negative!!!";
  }
  
  backtracking = true;
  storedLine = curLine;
  storedChar = curChar;
}
