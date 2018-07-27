// MAIN.CPP for compiler

#include "lexer.h"
#include "parserF.h"

#include <stack>

#define symbol_table std::unordered_map<const char*, token_type>
// define a symbol table list to work with scoping?
// we will always need a local scope and global scope, while there is a block
//   list, only check first and last ones

// job object for a sample job TODO
struct record {
  token_type tokenType;
};

//====================== Globals ======================//
// symbol tables
symbol_table globalST;
std::stack<symbol_table> localSTs;
// error list
error_list errList;
// error flags (parse error list and if there is a large error then disable
bool genCode = true;
bool abortFlag = false;
bool eat = false;
// lexer
lexer scanner;

//====================== Utility functions ======================//
void reportError(tok token, err_type eT, std::string msg) {
  error_obj newError;
  newError.errT = eT;
  newError.msg = msg;
  newError.lineNum = token.linePos;
  newError.charNum = token.charPos;
  
  if (eT != err_type::warning) {
    genCode = false;
  }

  errList.push_back(newError);
  std::cout << "    (L: " << newError.lineNum;
  std::cout << "|C: " << newError.charNum;
  std::cout  << ") [" << newError.errT << "] ";
  std::cout << newError.msg << "\n";
}

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

void peekTok(tok &token) {
  scanner.next_tok(token);
  if (token.tokenType == type_string) {
    token.name = cleanDblQuotes(token.name);
  }
  scanner.undo();
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
    std::cout << "End of file reached\n";
    return false;
  }
  return true;
}

bool a_getTok(tok &token) {
  return !abortFlag && getTok(token);
}

bool check_crit(tok &token, token_type type, int sec_type) {
  int tokenSecondary;
  switch (type) {
    case type_symb:
      tokenSecondary = token.s;
      break;
    case type_keyword:
      tokenSecondary = token.k;
      break;
  }
  if (tokenSecondary != sec_type) {
    return false;
  }
  return true;
}

void check_try() {
  
}

// parser paradigm
//   If a token was incorrect in a required parsing, the token is placed back where
//     it was, EXCEPT if it was a start of a parse tree
//   Look ahead checks for spelling errors are ONLY DONE with back to back token
//     handles.

void p_program(bool &success) {  
  tok token; bool suc = true;
  
  p_program_header(suc); // unused suc
  
  p_program_body(suc); // unused suc
  
  if (a_getTok(token)) { // .
    if (!check_crit(token, token_type::type_symb, symb_type::symb_period)) {
      std::string errMsg = "Unknown token after <program>";
      reportError(token, err_type::error, errMsg);
    }
  } else {
    abortFlag = true;
    std::string errMsg = "Missing . from <program>";
    reportError(token, err_type::error, errMsg);
  }
}

void p_program_header(bool &success) {
  tok token; bool suc = true, consume = true;
  
  if (getTok(token)) { // program
    if (!check_crit(token, token_type::type_keyword, key_type::key_program)) {
      std::string errMsg = "Missing 'program' from <program_header>";
      reportError(token, err_type::error, errMsg);
      if (token.tokenType != token_type::type_id) { // assume misspelling
        scanner.undo();
      }
    }
  } else {
    abortFlag = true;
  }
  
  p_identifier(suc, eat); // unused suc
  
  if (a_getTok(token)) { // is
    if (!check_crit(token, token_type::type_keyword, key_type::key_is)) {
      std::string errMsg = "Missing 'is' from <program_header>";
      reportError(token, err_type::error, errMsg);
      if (token.tokenType != token_type::type_id) { // assume misspelling
        scanner.undo();
      }
    } 
  } else {
    abortFlag = true;
  }
}

void p_program_body(bool &success) {
  if (!abortFlag) {
    tok token; bool suc = true;
    
    bool exists = false;
    do { // declaration repeat
      p_declaration(suc, exists); // unused suc
      
      if (exists && a_getTok(token)) { // ;
        if (!check_crit(token, token_type::type_symb, symb_type::symb_semicolon)) {
          std::string errMsg = "Missing ';' from <program_body><declaration>";
          reportError(token, err_type::error, errMsg);
          scanner.undo();
        }
      } else {
        abortFlag = true;
      }
    } while (exists);
    
    if (a_getTok(token)) { // begin
      if (!check_crit(token, token_type::type_keyword, key_type::key_begin)) {
        std::string errMsg = "Missing 'begin' from <program_body>";
        reportError(token, err_type::error, errMsg);
        if (token.tokenType != token_type::type_id) { // assume misspelling
          scanner.undo();
        }
      }
    } else {
      abortFlag = true;
    }
    
    exists = false;    
    do { // statement repeat
      p_statement(suc, exists); // unused suc
      
      if (exists && a_getTok(token)) { // ;
        if (!check_crit(token, token_type::type_symb, symb_type::symb_semicolon)) {
          std::string errMsg = "Missing ';' from <program_body><statement>";
          reportError(token, err_type::error, errMsg);
          scanner.undo();
        }
      } else {
        abortFlag = true;
      }
    } while (exists);
    
    if (a_getTok(token)) { // end
      if (!check_crit(token, token_type::type_keyword, key_type::key_end)) {
        std::string errMsg = "Missing 'end' from <program_body>";
        reportError(token, err_type::error, errMsg);
        if (token.tokenType != token_type::type_id) { // assume misspelling
          scanner.undo();
        }
      }
    } else {
      abortFlag = true;
    }
    
    if (a_getTok(token)) { // program
      if (!check_crit(token, token_type::type_keyword, key_type::key_program)) {
        std::string errMsg = "Missing 'program' from <program_body>";
        reportError(token, err_type::error, errMsg);
        if (token.tokenType != token_type::type_id) { // assume misspelling
          scanner.undo();
        }
      }
    } else {
      abortFlag = true;
    }
  }
}

void p_declaration(bool &success, bool &exists) {
  if (!abortFlag) {
    tok token; bool suc = true;
    
    bool global_obj = false;
    if (getTok(token)) { // opt: global
      if (token.tokenType == token_type::type_keyword && token.k == key_type::key_global) {
        global_obj = true;
      } else { // backtrack
        scanner.undo();
      }
    } else {
      abortFlag = true;
    }
    
    // Procedure or variable?
    p_procedure_declaration(suc, exists); // unused suc
    
    if (!exists) { // if it wasn't a procedure then it's a variable
      p_variable_declaration(suc, exists); // unused suc
    } // if exists is still false then a declaration doesn't exist
    
    if (global_obj && !exists) { // global with no declaration
      std::string errMsg = "Orphan 'global' in <declaration>";
      reportError(token, err_type::error, errMsg);
    }
  }
}

void p_procedure_declaration(bool &success, bool &exists) {
  if (!abortFlag) {
    bool suc = true;
    
    p_procedure_header(suc, exists); // unused suc
    
    if (exists) { // if the procedure header exists then parse body
      p_procedure_body(suc); // unused suc
    }
  }
}

void p_procedure_header(bool &success, bool &exists) {
  if (!abortFlag) {
    tok token; bool suc = true;
    
    if (getTok(token)) { // procedure
      if (!check_crit(token, token_type::type_keyword, key_type::key_procedure)) {
        exists = false;
        //std::string errMsg = "Missing 'procedure' from <procedure_header>";
        //reportError(token, err_type::error, errMsg);
        scanner.undo();
      }
    } else {
      abortFlag = true;
    }
    
    if (exists) {
      p_identifier(suc, eat);
      
      if (a_getTok(token)) { // (
        if (!check_crit(token, token_type::type_symb, symb_type::symb_op_paren)) {
          std::string errMsg = "Missing '(' from <procedure_header>";
          reportError(token, err_type::error, errMsg);
          scanner.undo();
        }
      } else {
        abortFlag = true;
      }
      
      bool hasParams = false;
      p_parameter_list(suc, hasParams); // unused suc
      if (hasParams) {
        // add param list to procedure
      } 
      
      if (a_getTok(token)) { // )
        if (!check_crit(token, token_type::type_symb, symb_type::symb_cl_paren)) {
          std::string errMsg = "Missing ')' from <procedure_header>";
          reportError(token, err_type::error, errMsg);
          scanner.undo();
        }
      } else {
        abortFlag = true;
      }
    }
  }
}

// does not return errors if it does not detect a parameter
void p_parameter_list(bool &success, bool &exists) {
  if (!abortFlag) {
    tok token; bool suc = true;
    
    bool prevParams = exists;
    p_parameter(suc, exists);
    
    if (exists) {
      tok lookahead;
      peekTok(lookahead); // ,
      if (check_crit(lookahead, token_type::type_symb, symb_type::symb_comma)) {
        getTok(token);
        p_parameter_list(suc, exists);
      } else {
        p_parameter(suc, exists);
        if (exists) {
          std::string errMsg = "Missing ',' from <parameter_list>";
          reportError(token, err_type::error, errMsg);
        } else {
          exists = true;
        }
      }
    }
    
    if (prevParams && !exists) {
      std::string errMsg = "Orphan ',' from <parameter_list>";
      reportError(token, err_type::error, errMsg);
      exists = true;
    }
  }
}

void p_parameter(bool &success, bool &exists) {
  if (!abortFlag) {
    tok token; bool suc = true;
    
    p_variable_declaration(suc, exists);
    
    if (exists) {
      if (a_getTok(token)) { // in/out/inout
        if (check_crit(token, token_type::type_keyword, key_type::key_in)) {
          // define in
        } else if (check_crit(token, token_type::type_keyword, key_type::key_out)) {
          // define out
        } else if (check_crit(token, token_type::type_keyword, key_type::key_inout)) {
          // define inout
        } else {
          std::string errMsg = "Missing 'in/out/inout' from <parameter>";
          reportError(token, err_type::error, errMsg);
          scanner.undo();
        }
      } else {
        abortFlag = true;
      }
    }
  }
}

void p_procedure_body(bool &success) {
  if (!abortFlag) {
    tok token; bool suc = true;
    
    bool exists = false;
    do { // declaration repeat
      p_declaration(suc, exists); // unused suc
      
      if (exists && a_getTok(token)) { // ;
        if (!check_crit(token, token_type::type_symb, symb_type::symb_semicolon)) {
          std::string errMsg = "Missing ';' from <procedure_body><declaration>";
          reportError(token, err_type::error, errMsg);
          scanner.undo();
        }
      } else {
        abortFlag = true;
      }
    } while (exists);
    
    if (a_getTok(token)) { // begin
      if (!check_crit(token, token_type::type_keyword, key_type::key_begin)) {
        std::string errMsg = "Missing 'begin' from <procedure_body>";
        reportError(token, err_type::error, errMsg);
        if (token.tokenType != token_type::type_id) { // assume misspelling
          scanner.undo();
        }
      }
    } else {
      abortFlag = true;
    }
    
    exists = false;    
    do { // statement repeat
      p_statement(suc, exists); // unused suc
      
      if (exists && a_getTok(token)) { // ;
        if (!check_crit(token, token_type::type_symb, symb_type::symb_semicolon)) {
          std::string errMsg = "Missing ';' from <procedure_body><statement>";
          reportError(token, err_type::error, errMsg);
          scanner.undo();
        }
      } else {
        abortFlag = true;
      }
    } while (exists);
    
    if (a_getTok(token)) { // end
      if (!check_crit(token, token_type::type_keyword, key_type::key_end)) {
        std::string errMsg = "Missing 'end' from <procedure_body>";
        reportError(token, err_type::error, errMsg);
        if (token.tokenType != token_type::type_id) { // assume misspelling
          scanner.undo();
        }
      }
    } else {
      abortFlag = true;
    }
    
    if (a_getTok(token)) { // program
      if (!check_crit(token, token_type::type_keyword, key_type::key_program)) {
        std::string errMsg = "Missing 'program' from <procedure_body>";
        reportError(token, err_type::error, errMsg);
        if (token.tokenType != token_type::type_id) { // assume misspelling
          scanner.undo();
        }
      }
    } else {
      abortFlag = true;
    }
  }
}

void p_variable_declaration(bool &success, bool &exists) {
  if (!abortFlag) {
    tok token; bool suc = true;
    
    p_type_mark(suc, exists);
    
    if (exists) {
      p_identifier(suc, eat);
      
      bool array = false;
      if (a_getTok(token)) { // optional [
        if (check_crit(token, token_type::type_symb, symb_type::symb_op_bracket)) {
          array = true;
        } else {
          scanner.undo();
        }
      } else {
        abortFlag = true;
      }
      
      if (array) {
        p_lower_bound(suc);
        
        if (a_getTok(token)) { // :
          if (!check_crit(token, token_type::type_symb, symb_type::symb_colon)) {
            std::string errMsg = "Missing ':' from <variable_declaration>";
            reportError(token, err_type::error, errMsg);
            scanner.undo();
          }
        } else {
          abortFlag = true;
        }
        
        p_upper_bound(suc);
        
        if (a_getTok(token)) { // ]
          if (!check_crit(token, token_type::type_symb, symb_type::symb_cl_bracket)) {
            std::string errMsg = "Missing ']' from <variable_declaration>";
            reportError(token, err_type::error, errMsg);
            scanner.undo();
          }
        } else {
          abortFlag = true;
        }
      }
    }
  }
}

void p_type_mark(bool &success, bool &exists) {
  if (!abortFlag) {
    tok token; bool suc = true;
    
    exists = true;
    if (a_getTok(token)) { // refer to type_mark in specification
      if (check_crit(token, token_type::type_keyword, key_type::key_integer)) {
        // integer
      } else if(check_crit(token, token_type::type_keyword, key_type::key_float)) {
        // float
      } else if(check_crit(token, token_type::type_keyword, key_type::key_string)) {
        // string
      } else if(check_crit(token, token_type::type_keyword, key_type::key_bool)) {
        // bool
      } else if(check_crit(token, token_type::type_keyword, key_type::key_char)) {
        // char
      } else {
        scanner.undo();
        exists = false;
      }
    } else {
      abortFlag = true;
    }
  }
}

void p_lower_bound(bool &success) {
  if (!abortFlag) {
    tok token; bool suc = true;
    
    bool neg = false;
    if (a_getTok(token)) { // optional -
      if (check_crit(token, token_type::type_symb, symb_type::symb_minus)) {
        neg = true;
      } else {
        scanner.undo();
      }
    } else {
      abortFlag = true;
    }
    
    p_number(suc, eat);
  }
}

void p_upper_bound(bool &success) {
  if (!abortFlag) {
    tok token; bool suc = true;
    
    bool neg = false;
    if (a_getTok(token)) { // optional -
      if (check_crit(token, token_type::type_symb, symb_type::symb_minus)) {
        neg = true;
      } else {
        scanner.undo();
      }
    } else {
      abortFlag = true;
    }
    
    p_number(suc, eat);
  }
}

void p_statement(bool &success, bool &exists) {
  if (!abortFlag) {
    bool suc = true;
    
    p_assignment_statement(suc, exists);
    if (!exists) {
      p_if_statement(suc, exists);
    }
    if (!exists) {
      p_loop_statement(suc, exists);
    }
    if (!exists) {
      p_return_statement(suc, exists);
    }
    if (!exists) {
      p_procedure_call(suc, exists);
    }
  }
}

void p_procedure_call(bool &success, bool &exists) {
  if (!abortFlag) {
    
  }
}

void p_assignment_statement(bool &success, bool &exists) {
  if (!abortFlag) {
    
  }
}

void p_destination(bool &success) {
  if (!abortFlag) {
    
  }
}

void p_if_statement(bool &success, bool &exists) {
  if (!abortFlag) {
    
  }
}

void p_loop_statement(bool &success, bool &exists) {
  if (!abortFlag) {
    
  }
}

void p_return_statement(bool &success, bool &exists) {
  if (!abortFlag) {
    
  }
}

void p_identifier(bool &success, bool &exists) {
  if (!abortFlag) {
    
  }
}

void p_expression(bool &success) {
  if (!abortFlag) {
    
  }
}

void p_expression_pr(bool &success) {
  if (!abortFlag) {
    
  }
}

void p_arithOp(bool &success) {
  if (!abortFlag) {
    
  }
}

void p_arithOp_pr(bool &success) {
  if (!abortFlag) {
    
  }
}

void p_relation(bool &success) {
  if (!abortFlag) {
    
  }
}

void p_relation_pr(bool &success) {
  if (!abortFlag) {
    
  }
}

void p_term(bool &success) {
  if (!abortFlag) {
    
  }
}

void p_term_pr(bool &success) {
  if (!abortFlag) {
    
  }
}

void p_factor(bool &success) {
  if (!abortFlag) {
    
  }
}

void p_name(bool &success) {
  if (!abortFlag) {
    
  }
}

void p_argument_list(bool &success) {
  if (!abortFlag) {
    
  }
}

void p_number(bool &success, bool &exists) {
  if (!abortFlag) {
    
  }
}

void p_string(bool &success) {
  if (!abortFlag) {
    
  }
}

void p_char(bool &success) {
  if (!abortFlag) {
    
  }
}

//====================== Main ======================//
int main(int argc, const char *argv[]) {
  if (argc == 2) {
    std::cout << "Compiler start " << argv[1] << "\n";
    scanner.init(argv[1], &errList);
    bool suc = true;
    
    
    // begin parsing
    p_program(suc);
    
    /* DEBUG
    tok token;
    
    while (getTok(token)) {
      printTok(token);
    }
    */
    
    scanner.deinit();
  } else {
    std::cout << "Too many or too little args!";
  }

  return 0;
}
