#include "lexer.h"

//====================== Init functions ======================//
lexer::lexer() {
}

lexer::~lexer() {
}

bool lexer::init(const char* fn, error_list* el) {
  errL = el;
  fs.open(fn, std::fstream::in);
  
  curLine = 1; curChar = 0;
  storedLine = 0; storedChar = 0;
  tokCursor = 0;
  backtracking = false;
  fileEnd = false;
  
  // TODO populate globalT with reserved words
  keywordL.insert({"if", keyword_type::res_if});

  return true;
}
  
  
bool lexer::deinit() {
  fs.close();
  return true;
}
  
void lexer::reportError(err_type eT, const char* msg) {
  error_obj newError;
  newError.errT = eT;
  strcpy(newError.msg, msg);
  newError.lineNum = curLine;
  newError.charNum = curChar;

  errL->push_back(newError);
  std::cout << "New Error! "; 
  std::cout << newError.errT << " ";
  std::cout << newError.msg << " ";
  std::cout << newError.lineNum << " ";
  std::cout << newError.charNum << "\n";
}

bool lexer::next_tok(tok &out) {
  bool illegalChar = false;
  if (backtracking) {
    // grab next tokMem (since cursor is 1 ahead, grab the current tok)
    out = tokMem.at(tokCursor);
    
    curLine = out.linePos;
    curChar = out.charPos;
  }
  else {
    int state = 0;
    bool whitespace = false;
    std::string fullToken;
    do {
      char nxtChar;
      int commentLevel = 0;
      if (!fs.get(nxtChar)) {
        fileEnd = true;
        nxtChar = ' ';
      } else {
        nxtChar = std::tolower(nxtChar);
      }
      curChar++;
      
      bool useIllegals = true; // TODO temporary use illegals
      // check illegal character (not a legal character) TODO
      if (!(nxtChar == '/' || nxtChar == '\\' || nxtChar == ';' ||
            std::isalnum(nxtChar)) && !useIllegals) {
        illegalChar = true;
        reportError(err_type::error, "Illegal character detected");
        nxtChar = ' ';
      }
      
      switch (nxtChar) {
        case '\n':
          curLine++;
          curChar = 0;
        case '\t':
        case ' ':
          whitespace = true;
      }
      
      // TODO state machine to gen tokens
      char peeky;
      switch (state) {
        case 0: // nothing's happened yet
          if (!whitespace) { // if nxtChar is something
            fullToken.push_back(nxtChar);
            out.linePos = curLine;
            out.charPos = curChar;
            switch (nxtChar) { // set state depending on nxtChar
              case '/':
                state = 1;
            }
            state = -1; // TODO testing temporary only, just grab chars
          }
          break;
        case 1: // comment start or divide
            switch (nxtChar) {
              case '*':
                commentLevel++;
              case '/':
                fullToken.clear();
                state = 2;
                break;
              default: // actually a divide
                out.tokenType = token_type::symbol;
                out.s = symbol_type::divider;
                state = -1;
            }
          break;
        case 2: // in comment block
          switch (nxtChar) {
            case '\n':
              if (commentLevel == 0) {
                state = 0;
              }
              break;
            case '/':
              peeky = fs.peek();
              if (peeky == '*') {
                commentLevel++;
                fs.get(nxtChar);
                curChar++;
              }
              break;
            case '*':
              if (commentLevel > 0) {
                peeky = fs.peek();
                if (peeky == '/') {
                  commentLevel--;
                  fs.get(nxtChar);
                  curChar++;
                  if (commentLevel == 0) {
                    state = 0;
                  }
                }
              }
          }
          break;
        case 3:
          break;
      }
      
      // IF the token type ends up being some sort of identifier, compare to 
      //   keyword list to check to see if it's a keyword
      
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
      
    } while (state != -1 && !fileEnd);
    
    if (!whitespace) {
      fs.unget();
      curChar--;
    }
    
    // done
    out.name = fullToken;
    
    // add to tokMem
    tokMem.push_back(out);
  }
  
  tokCursor++;
  
  if (backtracking && tokCursor == tokMem.size()) {
    backtracking == false;
    curLine = storedLine;
    curChar = storedChar;
  }
  
  return !illegalChar;
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
