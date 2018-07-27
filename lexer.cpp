#include "lexer.h"

// enum state names
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
//====================== Utility functions ======================//
std::string cleanUnderscores(std::string str) {
  std::string out;
  for (size_t i = 0; i < str.size(); i++) {
    if (str[i] != '_') {
      out.push_back(str[i]);
    }
  }
  return out;
}

int strToInt(std::string str) {
  return std::stoi(cleanUnderscores(str));
}

float strToFl(std::string str) {
  return std::stof(cleanUnderscores(str));
}

//====================== To String functions ======================//
const char* lexer::typeToString(token_type tokenType) {
  switch (tokenType) {
    case type_int: return "int";
    case type_float: return "float";
    case type_char: return "char";
    case type_string: return "string";
    case type_symb: return "symbol";
    case type_id: return "identifier";
    case type_keyword: return "keyword";
    default: return "unknown";
  }
}

const char* lexer::symbToString(symb_type symbType) {
  switch (symbType) {
    case symb_period: return "period";
    case symb_op_paren: return "op paren";
    case symb_semicolon: return "semicolon";
    case symb_cl_paren: return "cl paren";
    case symb_comma: return "comma";
    case symb_op_bracket: return "op brak";
    case symb_colon: return "colon";
    case symb_cl_bracket: return "cl brak";
    case symb_minus: return "minus";
    case symb_assign: return "assign";
    case symb_amper: return "ampersand";
    case symb_straight: return "straight";
    case symb_plus: return "plus";
    case symb_smaller: return "smol than";
    case symb_greater: return "gr8t than";
    case symb_smaller_eq: return "smol eq than";
    case symb_greater_eq: return "gr8t eq than";
    case symb_equals: return "equals";
    case symb_not_equals: return "not eq";
    case symb_multi: return "multiply";
    case symb_div: return "divide";
    case symb_quote: return "quote";
    case symb_db_quote: return "db quote";
    default: return "unknown";
  }
}

const char* lexer::keywToString(key_type keyType) {
  switch (keyType) {
    case key_program: return "program";
    case key_is: return "is";
    case key_begin: return "begin";
    case key_end: return "end";
    case key_global: return "global";
    case key_procedure: return "procedure";
    case key_in: return "in";
    case key_out: return "out";
    case key_inout: return "inout";
    case key_integer: return "integer";
    case key_float: return "float";
    case key_string: return "string";
    case key_bool: return "bool";
    case key_char: return "char";
    case key_if: return "if";
    case key_then: return "then";
    case key_else: return "else";
    case key_for: return "for";
    case key_return: return "return";
    case key_not: return "not";
    case key_true: return "true";
    case key_false: return "false";
    default: return "unknown";
  }
}
  

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
void lexer::reportError(err_type eT, std::string msg) {
  error_obj newError;
  newError.errT = eT;
  newError.msg = msg;
  newError.lineNum = curLine;
  newError.charNum = curChar;

  errL->push_back(newError);
  std::cout << "  (L: " << newError.lineNum;
  std::cout << "|C: " << newError.charNum;
  std::cout  << " [" << newError.errT << "] ";
  std::cout << newError.msg << "\n";
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
    bool whitespace, consume;
    int commentLevel = 0;
    std::string fullToken;
    do {
      char nxtChar;
      whitespace = false; consume = false;
      fs.get(nxtChar);
      if (fs.eof()) {
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
        case '\r':
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
        std::string errString = "Illegal character ";
        errString.append("[");
        errString.append(std::to_string((int)nxtChar));
        errString.append("] detected");
        reportError(err_type::error, errString);
        nxtChar = ' ';
        whitespace = true;
      }
      
      // state machine to gen tokens
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
                case '>': // handled by greater_eq
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
                    consume = true;
                  } else {
                    illegalChar = true;
                    reportError(err_type::error, "Illegal ! detected");
                    fullToken.clear();
                    state = states::unstarted;
                  }
                  break;
                case '=':
                  peeky = fs.peek();
                  if (peeky == '=') {
                    fs.get(nxtChar);
                    curChar++;
                    fullToken.push_back(nxtChar);
                    out.tokenType = token_type::type_symb;
                    out.s = symb_type::symb_equals;
                    state = states::done;
                    consume = true;
                  } else {
                    illegalChar = true;
                    reportError(err_type::error, "Illegal = detected");
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
                  consume = true;
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
              if (peeky == '*' && commentLevel != 0) {
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
            consume = true;
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
            consume = true;
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
            consume = true;
          } else {
            out.s = symb_type::symb_greater;
          }
          state = states::done;
          break;
        case states::character:
          fullToken.push_back(nxtChar);
          if (nxtChar == '\'') { // char has ended
            peeky = fullToken[1];
            if (fullToken.size() != 3) {
              illegalChar = true;
              reportError(err_type::error, "Malformed char detected");
            } else if (!(std::isalnum(peeky) || peeky == ' ' || 
                peeky == '_' || peeky == ';' || peeky == ':' || peeky == '.' ||
                peeky == '"')) {
              illegalChar = true;
              reportError(err_type::error, "Illegal char detected");
            }
            out.tokenType = token_type::type_char;
            out.c = peeky;
            consume = true;
            state = states::done;
          }
          break;
        case states::stringy:
          fullToken.push_back(nxtChar);
          if (nxtChar == '\"') { // string has ended
            if (fullToken.size() == 2) {
              illegalChar = true;
              reportError(err_type::error, "Empty string detected");
            }
            out.tokenType = token_type::type_string;
            consume = true;
            state = states::done;
          } else if (!(std::isalnum(nxtChar) || nxtChar == ' ' || nxtChar == '_' ||
              nxtChar == ',' || nxtChar == ';' || nxtChar == ':' || nxtChar == '.' ||
              nxtChar == '\'')) {
            illegalChar = true;
            reportError(err_type::error, "Illegal char in string detected");
          }
          break;
        case states::number:
          if (std::isdigit(nxtChar) || nxtChar == '_') {
            fullToken.push_back(nxtChar);
          } else if (nxtChar == '.') {
            fullToken.push_back(nxtChar);
            state = states::decimal;
          } else {
            out.tokenType = token_type::type_int;
            out.i = strToInt(fullToken);
            state = states::done;
          }
          break;
        case states::decimal:
          if (std::isdigit(nxtChar) || nxtChar == '_') {
            fullToken.push_back(nxtChar);
          } else {
            out.tokenType = token_type::type_float;
            out.f = strToFl(fullToken);
            state = states::done;
          }
          break;
        case states::id_key:
          if (isalnum(nxtChar) || nxtChar == '_') {
            fullToken.push_back(nxtChar);
          } else {
            out.tokenType = token_type::type_id;
            for (auto it = keywordL.begin(); it != keywordL.end(); ++it ) {
              if (fullToken.compare(it->first) == 0) {
                out.tokenType = token_type::type_keyword;
                out.k = it->second;
                break;
              }
            }
            state = states::done;
          }
          break;
      }
    } while (state != states::done && !fileEnd);
    
    if (!whitespace && !consume) {
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
