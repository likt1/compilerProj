#include "lexer.h"

// enum state names TODO
enum states {
  unstarted,    // haven't found anything
  div_comment,  // comment start or divide
  comment,      // in a comment
  colon_assign, // colon or assignment
  smaller_eq,   // smaller than or <=
  greater_eq,   // greater than or >=
  character,    // analyzies characters
  stringy,      // analyzes strings
  number,       // a number (int or float)
  decimal,      // decimal stuff in floats
  id_key,       // an identifier or keyword
  done          // tokenizing complete
};

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
  
  // populate globalT with reserved words
  keywordL.insert({"program", key_type::key_program});
  keywordL.insert({"is", key_type::key_is});
  keywordL.insert({"begin", key_type::key_begin});
  keywordL.insert({"end", key_type::key_end});
  keywordL.insert({"global", key_type::key_global});
  keywordL.insert({"procedure", key_type::key_procedure});
  keywordL.insert({"in", key_type::key_in});
  keywordL.insert({"out", key_type::key_out});
  keywordL.insert({"inout", key_type::key_inout});
  keywordL.insert({"integer", key_type::key_integer});
  keywordL.insert({"float", key_type::key_float});
  keywordL.insert({"string", key_type::key_string});
  keywordL.insert({"bool", key_type::key_bool});
  keywordL.insert({"char", key_type::key_char});
  keywordL.insert({"if", key_type::key_if});
  keywordL.insert({"then", key_type::key_then});
  keywordL.insert({"else", key_type::key_else});
  keywordL.insert({"for", key_type::key_for});
  keywordL.insert({"return", key_type::key_return});
  keywordL.insert({"not", key_type::key_not});
  keywordL.insert({"true", key_type::key_true});
  keywordL.insert({"false", key_type::key_false});

  return true;
}
  
  
bool lexer::deinit() {
  fs.close();
  return true;
}

//====================== Report functions ======================//
void lexer::reportError(err_type eT, const char* msg) {
  error_obj newError;
  newError.errT = eT;
  strcpy(newError.msg, msg);
  newError.lineNum = curLine;
  newError.charNum = curChar;

  errL->push_back(newError);
  std::cout  << "[" << newError.errT << "] ";
  std::cout << newError.msg << ": ";
  std::cout << newError.lineNum << " | ";
  std::cout << newError.charNum << "\n";
}

//====================== Tokenizer functions ======================//
bool lexer::next_tok(tok &out) {
  bool illegalChar = false;
  if (backtracking) {
    // grab next tokMem (since cursor is 1 ahead, grab the current tok)
    out = tokMem.at(tokCursor);
    
    curLine = out.linePos;
    curChar = out.charPos;
  }
  else {
    states state = states::unstarted;
    bool whitespace, singleton;
    std::string fullToken;
    do {
      char nxtChar;
      int commentLevel = 0;
      whitespace = false; singleton = false;
      fs.get(nxtChar);
      if (nxtChar == -1) {
        fileEnd = true;
        nxtChar = ' ';
      } else {
        nxtChar = std::tolower(nxtChar);
      }
      curChar++;
      
      bool ignoreIllegals = false; // illegals are ok in comments
      if (state == states::comment) {
        ignoreIllegals = true;
      }
      
      // deal with whitespace
      switch (nxtChar) {
        case '\n':
          curLine++;
          curChar = 0;
        case '\t':
        case ' ':
          whitespace = true;
          break;
      }
      
      // check illegal character (not a legal character)
      // look at symb_type
      //   also includes _
      if (!(nxtChar == '.' || nxtChar == '(' || nxtChar == ';' ||
            nxtChar == ')' || nxtChar == ',' || nxtChar == '[' ||
            nxtChar == ':' || nxtChar == ']' || nxtChar == '-' ||
            nxtChar == '=' || nxtChar == '&' || nxtChar == '|' ||
            nxtChar == '+' || nxtChar == '<' || nxtChar == '>' ||
            nxtChar == '!' || nxtChar == '*' || nxtChar == '/' ||
            nxtChar == '\'' || nxtChar == '"' || nxtChar == '_' ||
            std::isalnum(nxtChar) || whitespace) && !ignoreIllegals) {
        illegalChar = true;
        reportError(err_type::error, "Illegal character detected");
        nxtChar = ' ';
      }
      
      // TODO state machine to gen tokens
      char peeky;
      switch (state) {
        case states::unstarted:
          if (!whitespace) { // if nxtChar is something
            fullToken.push_back(nxtChar);
            out.linePos = curLine;
            out.charPos = curChar;
            
            // set state depending on nxtChar
            if (std::isalnum(nxtChar)) {
              if (std::isdigit(nxtChar)) { // number
                state = states::number;
              } else { // identifier/keyword
                state = states::id_key;
              }
            } else {
              switch (nxtChar) {
                case '/': // handled by div_comment
                  state = states::div_comment;
                  break;
                case ':': // handled by colon_assign
                  state = states::colon_assign;
                  break;
                case '<': // handled by smaller_eq
                  state = states::smaller_eq;
                  break;
                case '=': // handled by greater_eq
                  state = states::greater_eq;
                  break;
                case '!':
                  peeky = fs.peek();
                  if (peeky == '=') {
                    fs.get(nxtChar);
                    curChar++;
                    fullToken.push_back(nxtChar);
                    out.tokenType = token_type::type_symb;
                    out.s = symb_type::symb_not_equals;
                    state = states::done;
                    singleton = true;
                  } else {
                    illegalChar = true;
                    reportError(err_type::error, "Illegal ! detected");
                    fullToken.clear();
                    state = states::unstarted;
                  }
                  break;
                case '\'': // handled by character
                  state = states::character;
                  break;
                case '"': // handled by stringy
                  state = states::stringy;
                  break;
                default:
                  out.tokenType = token_type::type_symb;
                  singleton = true;
                  state = states::done;
                  switch (nxtChar) {
                    case '.':
                      out.s = symb_type::symb_period;
                      break;
                    case '(':
                      out.s = symb_type::symb_op_paren;
                      break;
                    case ';':
                      out.s = symb_type::symb_semicolon;
                      break;
                    case ')':
                      out.s = symb_type::symb_cl_paren;
                      break;
                    case ',':
                      out.s = symb_type::symb_comma;
                      break;
                    case '[':
                      out.s = symb_type::symb_op_bracket;
                      break;
                    case ']':
                      out.s = symb_type::symb_cl_bracket;
                      break;
                    case '-':
                      out.s = symb_type::symb_minus;
                      break;
                    case '&':
                      out.s = symb_type::symb_amper;
                      break;
                    case '|':
                      out.s = symb_type::symb_straight;
                      break;
                    case '+':
                      out.s = symb_type::symb_plus;
                      break;
                    case '*':
                      out.s = symb_type::symb_multi;
                      break;
                  }
                  break;
              }
            }
          }
          break;
        case states::div_comment:
          switch (nxtChar) {
            case '*':
              commentLevel++;
            case '/':
              fullToken.clear();
              state = states::comment;
              break;
            default: // actually a divide
              out.tokenType = token_type::type_symb;
              out.s = symb_type::symb_div;
              state = states::done;
              break;
          }
          break;
        case states::comment:
          switch (nxtChar) {
            case '\n':
              if (commentLevel == 0) {
                state = states::unstarted;
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
                    state = states::unstarted;
                  }
                }
              }
              break;
          }
          break;
        case states::colon_assign:
          out.tokenType = token_type::type_symb;
          if (nxtChar == '=') {
            fullToken.push_back(nxtChar);
            out.s = symb_type::symb_assign;
            singleton = true;
          } else {
            out.s = symb_type::symb_colon;
          }
          state = states::done;
          break;
        case states::smaller_eq:
          out.tokenType = token_type::type_symb;
          if (nxtChar == '=') {
            fullToken.push_back(nxtChar);
            out.s = symb_type::symb_smaller_eq;
            singleton = true;
          } else {
            out.s = symb_type::symb_smaller;
          }
          state = states::done;
          break;
        case states::greater_eq:
          out.tokenType = token_type::type_symb;
          if (nxtChar == '=') {
            fullToken.push_back(nxtChar);
            out.s = symb_type::symb_greater_eq;
            singleton = true;
          } else {
            out.s = symb_type::symb_greater;
          }
          state = states::done;
          break;
        case states::character: // TODO
          if (nxtChar == '=') {
            fullToken.push_back(nxtChar);
            out.tokenType = token_type::type_symb;
            out.s = symb_type::symb_assign;
            state = states::done;
            singleton = true;
          } else {
            out.tokenType = token_type::type_symb;
            out.s = symb_type::symb_colon;
            state = states::done;
          }
          break;
        case states::stringy: // TODO
          if (nxtChar == '=') {
            fullToken.push_back(nxtChar);
            out.tokenType = token_type::type_symb;
            out.s = symb_type::symb_assign;
            state = states::done;
            singleton = true;
          } else {
            out.tokenType = token_type::type_symb;
            out.s = symb_type::symb_colon;
            state = states::done;
          }
          break;
          
        case states::number: // TODO
          if (nxtChar == '=') {
            fullToken.push_back(nxtChar);
            out.tokenType = token_type::type_symb;
            out.s = symb_type::symb_assign;
            state = states::done;
            singleton = true;
          } else {
            out.tokenType = token_type::type_symb;
            out.s = symb_type::symb_colon;
            state = states::done;
          }
          break;
        case states::decimal: // TODO
          if (nxtChar == '=') {
            fullToken.push_back(nxtChar);
            out.tokenType = token_type::type_symb;
            out.s = symb_type::symb_assign;
            state = states::done;
            singleton = true;
          } else {
            out.tokenType = token_type::type_symb;
            out.s = symb_type::symb_colon;
            state = states::done;
          }
          break;
        case states::id_key: // TODO
          if (nxtChar == '=') {
            fullToken.push_back(nxtChar);
            out.tokenType = token_type::type_symb;
            out.s = symb_type::symb_assign;
            state = states::done;
            singleton = true;
          } else {
            out.tokenType = token_type::type_symb;
            out.s = symb_type::symb_colon;
            state = states::done;
          }
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
      
    } while (state != states::done && !fileEnd);
    
    if (!whitespace && !singleton) {
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
