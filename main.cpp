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
bool reqExists = false;
// lexer
lexer scanner;

//====================== Utility functions ======================//
void reportError(tok &token, err_type eT, std::string msg) {
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
    case type_illegal:
      std::cout << "-> " << scanner.illgToString(token.il);
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
    abortFlag = true;
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

void eat_misspelling(tok &token) {
  if (token.tokenType != token_type::type_id) { // if not misspelling
    scanner.undo(); // do not eat
  } // otherwise do nothing (eat)
}

// parser paradigm
//   If a token was incorrect in a required parsing, the token is placed back where
//     it was, EXCEPT if it was a start of a parse tree
//   Look ahead checks for spelling errors are ONLY DONE with back to back token
//     handles.

// DEFINITIONS IN HEADER parserF.h
//====================== Parser functions ======================//
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
    std::string errMsg = "Missing . from <program>";
    reportError(token, err_type::error, errMsg);
  }
}

void p_program_header(bool &success) {
  tok token; bool suc = true;
  
  if (getTok(token) && // program
      !check_crit(token, token_type::type_keyword, key_type::key_program)) {
    std::string errMsg = "Missing 'program' from <program_header>";
    reportError(token, err_type::error, errMsg);
    eat_misspelling(token);
  }
  
  p_identifier(suc, reqExists); // unused suc
  if (!reqExists) {
    std::string errMsg = "Missing 'identifier' from <program_header>";
    reportError(token, err_type::error, errMsg);
  }
  
  if (a_getTok(token) && // is
      !check_crit(token, token_type::type_keyword, key_type::key_is)) {
    std::string errMsg = "Missing 'is' from <program_header>";
    reportError(token, err_type::error, errMsg);
    eat_misspelling(token);
  }
}

void p_program_body(bool &success) {
  if (!abortFlag) {
    tok token; bool suc = true;
    
    bool exists = false;
    do { // declaration repeat
      p_declaration(suc, exists); // unused suc
      
      if (exists && a_getTok(token) && // ;
          !check_crit(token, token_type::type_symb, symb_type::symb_semicolon)) {
        std::string errMsg = "Missing ';' from <program_body><declaration>";
        reportError(token, err_type::error, errMsg);
        scanner.undo();
      }
    } while (exists && !abortFlag);
    
    if (a_getTok(token) && // begin
        !check_crit(token, token_type::type_keyword, key_type::key_begin)) {
      std::string errMsg = "Missing 'begin' from <program_body>";
      reportError(token, err_type::error, errMsg);
      //eat_misspelling(token); // statements after begin can start with an identifier
      scanner.undo();
    }
    
    do { // statement repeat
      p_statement(suc, exists); // unused suc
      
      if (exists && a_getTok(token) && // ;
          !check_crit(token, token_type::type_symb, symb_type::symb_semicolon)) {
        std::string errMsg = "Missing ';' from <program_body><statement>";
        reportError(token, err_type::error, errMsg);
        scanner.undo();
      }
    } while (exists && !abortFlag);
    
    if (a_getTok(token) && // end
        !check_crit(token, token_type::type_keyword, key_type::key_end)) {
      std::string errMsg = "Missing 'end' from <program_body>";
      reportError(token, err_type::error, errMsg);
      eat_misspelling(token);
    }
    
    if (a_getTok(token) && // program
        !check_crit(token, token_type::type_keyword, key_type::key_program)) {
      std::string errMsg = "Missing 'program' from <program_body>";
      reportError(token, err_type::error, errMsg);
      eat_misspelling(token);
    }
  }
}

void p_declaration(bool &success, bool &exists) {
  if (!abortFlag) {
    tok token; bool suc = true;
    
    bool global_obj = false;
    if (getTok(token)) { // opt: global TODO update to crit_check
      if (token.tokenType == token_type::type_keyword && token.k == key_type::key_global) {
        global_obj = true;
      } else { // backtrack
        scanner.undo();
      }
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
    
    exists = true;
    if (getTok(token) && // procedure exists?
        !check_crit(token, token_type::type_keyword, key_type::key_procedure)) {
      exists = false;
      scanner.undo();
      //std::string errMsg = "Missing 'procedure' from <procedure_header>";
      //reportError(token, err_type::error, errMsg);
    }
    
    if (exists) {
      p_identifier(suc, reqExists);
      if (!reqExists) {
        std::string errMsg = "Missing 'identifier' from <procedure_header>";
        reportError(token, err_type::error, errMsg);
      }
      
      if (a_getTok(token) && // (
          !check_crit(token, token_type::type_symb, symb_type::symb_op_paren)) {
        std::string errMsg = "Missing '(' from <procedure_header>";
        reportError(token, err_type::error, errMsg);
        scanner.undo();
      }
      
      bool hasParams = false;
      p_parameter_list(suc, hasParams); // unused suc
      if (hasParams) {
        // add param list to procedure
      } 
      
      if (a_getTok(token) && // )
          !check_crit(token, token_type::type_symb, symb_type::symb_cl_paren)) {
        std::string errMsg = "Missing ')' from <procedure_header>";
        reportError(token, err_type::error, errMsg);
        scanner.undo();
      }
    }
  }
}

void p_parameter_list(bool &success, bool &exists) {
  if (!abortFlag) {
    tok token; bool suc = true;
    
    bool prevParams = exists;
    p_parameter(suc, exists);
    
    if (exists) {
      tok lookahead; peekTok(lookahead); // ,
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
    
    if (exists && a_getTok(token)) { // in/out/inout
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
    }
  }
}

void p_procedure_body(bool &success) {
  if (!abortFlag) {
    tok token; bool suc = true;
    
    bool exists = false;
    do { // declaration repeat
      p_declaration(suc, exists); // unused suc
      
      if (exists && a_getTok(token) && // ;
          !check_crit(token, token_type::type_symb, symb_type::symb_semicolon)) {
        std::string errMsg = "Missing ';' from <procedure_body><declaration>";
        reportError(token, err_type::error, errMsg);
        scanner.undo();
      }
    } while (exists && !abortFlag);
    
    if (a_getTok(token) && // begin
        !check_crit(token, token_type::type_keyword, key_type::key_begin)) {
      std::string errMsg = "Missing 'begin' from <procedure_body>";
      reportError(token, err_type::error, errMsg);
      //eat_misspelling(token);
      scanner.undo();
    }
    
    do { // statement repeat
      p_statement(suc, exists); // unused suc
      
      if (exists && a_getTok(token) && // ;
          !check_crit(token, token_type::type_symb, symb_type::symb_semicolon)) {
        std::string errMsg = "Missing ';' from <procedure_body><statement>";
        reportError(token, err_type::error, errMsg);
        scanner.undo();
      }
    } while (exists && !abortFlag);
     
    if (a_getTok(token) && // end
        !check_crit(token, token_type::type_keyword, key_type::key_end)) {
      std::string errMsg = "Missing 'end' from <procedure_body>";
      reportError(token, err_type::error, errMsg);
      eat_misspelling(token);
    }
    
    if (a_getTok(token) && // procedure
        !check_crit(token, token_type::type_keyword, key_type::key_procedure)) {
      std::string errMsg = "Missing 'procedure' from <procedure_body>";
      reportError(token, err_type::error, errMsg);
      eat_misspelling(token);
    }
  }
}

void p_variable_declaration(bool &success, bool &exists) {
  if (!abortFlag) {
    tok token; bool suc = true;
    
    p_type_mark(suc, exists);
    
    if (exists) {
      p_identifier(suc, reqExists);
      if (!reqExists) {
        std::string errMsg = "Missing 'identifier' from <variable_declaration>";
        reportError(token, err_type::error, errMsg);
      }
      
      bool array = false;
      if (a_getTok(token)) { // optional [
        if (check_crit(token, token_type::type_symb, symb_type::symb_op_bracket)) {
          array = true;
        } else {
          scanner.undo();
        }
      }
      
      if (array) {
        p_lower_bound(suc);
        
        if (a_getTok(token) && // :
            !check_crit(token, token_type::type_symb, symb_type::symb_colon)) {
          std::string errMsg = "Missing ':' from <variable_declaration>";
          reportError(token, err_type::error, errMsg);
          scanner.undo();
        }
        
        p_upper_bound(suc);
        
        if (a_getTok(token) && // ]
            !check_crit(token, token_type::type_symb, symb_type::symb_cl_bracket)) {
          std::string errMsg = "Missing ']' from <variable_declaration>";
          reportError(token, err_type::error, errMsg);
          scanner.undo();
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
    }
    
    p_number(suc, reqExists);
    if (!reqExists) {
      std::string errMsg = "Missing 'number' from <lower_bound>";
      reportError(token, err_type::error, errMsg);
    }
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
    }
    
    p_number(suc, reqExists);
    if (!reqExists) {
      std::string errMsg = "Missing 'number' from <upper_bound>";
      reportError(token, err_type::error, errMsg);
    }
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
    } // if exists is still false, a statement does not exist
  }
}

void p_procedure_call(bool &success, bool &exists) {
  if (!abortFlag) {
    tok token; bool suc = true;
    
    p_identifier(suc, exists);
    
    if (exists) {
      if (a_getTok(token) && // (
          !check_crit(token, token_type::type_symb, symb_type::symb_op_paren)) {
        std::string errMsg = "Missing '(' from <procedure_call>";
        reportError(token, err_type::error, errMsg);
        scanner.undo();
      }
      
      bool hasArgs = false;
      p_argument_list(suc, hasArgs); // unused suc
      if (hasArgs) {
        // add arg list to call
      }
      
      if (a_getTok(token) && // )
          !check_crit(token, token_type::type_symb, symb_type::symb_cl_paren)) {
        std::string errMsg = "Missing ')' from <procedure_call>";
        reportError(token, err_type::error, errMsg);
        scanner.undo();
      }
    }
  }
}

void p_assignment_statement(bool &success, bool &exists) {
  if (!abortFlag) {
    tok token; bool suc = true;
    
    p_destination(suc, exists);
    
    if (exists) {
      tok lookahead; peekTok(lookahead); // check = or :=
      if (check_crit(lookahead, token_type::type_symb, symb_type::symb_assign)) {
        getTok(token);
      } else if (check_crit(lookahead, token_type::type_illegal, ill_type::ill_equals)) {
        std::string errMsg = "Malformed '!=' from <assignment_statement>";
        reportError(token, err_type::error, errMsg);
      } else {
        exists = false;
      }
    }
    
    if (exists) {
      p_expression(suc);
    }
  }
}

void p_destination(bool &success, bool &exists) {
  if (!abortFlag) {
    tok token; bool suc = true;
    
    p_identifier(suc, exists);
    
    if (exists) {
      bool array = false;
      if (a_getTok(token)) { // optional [
        if (check_crit(token, token_type::type_symb, symb_type::symb_op_bracket)) {
          array = true;
        } else {
          scanner.undo();
        }
      }
      
      if (array) {
        p_expression(suc);
        
        if (a_getTok(token) && // ]
            !check_crit(token, token_type::type_symb, symb_type::symb_cl_bracket)) {
          std::string errMsg = "Missing ']' from <destination>";
          reportError(token, err_type::error, errMsg);
          scanner.undo();
        }
      }
    }
  }
}

void p_if_statement(bool &success, bool &exists) {
  if (!abortFlag) {
    tok token; bool suc = true;
    
    exists = true;
    if (a_getTok(token) && // if
        !check_crit(token, token_type::type_keyword, key_type::key_if)) {
      exists = false;
      scanner.undo();
      //std::string errMsg = "Missing '(' from <procedure_call>";
      //reportError(token, err_type::error, errMsg);
    }
    
    if (exists) {
      if (a_getTok(token) && // (
          !check_crit(token, token_type::type_symb, symb_type::symb_op_paren)) {
        std::string errMsg = "Missing '(' from <if_statement>";
        reportError(token, err_type::error, errMsg);
        scanner.undo();
      }
      p_expression(suc);
      
      if (a_getTok(token) && // )
          !check_crit(token, token_type::type_symb, symb_type::symb_cl_paren)) {
        std::string errMsg = "Missing ')' from <if_statement>";
        reportError(token, err_type::error, errMsg);
        scanner.undo();
      }
      
      if (a_getTok(token) && // then
          !check_crit(token, token_type::type_keyword, key_type::key_then)) {
        std::string errMsg = "Missing 'then' from <if_statement>";
        reportError(token, err_type::error, errMsg);
        //eat_misspelling(token); // statements after then can start with an identifier
        scanner.undo();
      }
      
      p_statement(suc, exists); // first statement
      if (!exists) {
        std::string errMsg = "Missing statements from <if_statement><then>";
        reportError(token, err_type::error, errMsg);
      }
        
      if (exists && a_getTok(token) && // ;
          !check_crit(token, token_type::type_symb, symb_type::symb_semicolon)) {
        std::string errMsg = "Missing ';' from <if_statement><then><statement>";
        reportError(token, err_type::error, errMsg);
        scanner.undo();
      }
      
      while (exists) { // statement repeat
        p_statement(suc, exists); // unused suc
        
        if (exists && a_getTok(token) && // ;
            !check_crit(token, token_type::type_symb, symb_type::symb_semicolon)) {
          std::string errMsg = "Missing ';' from <if_statement><then><statement>";
          reportError(token, err_type::error, errMsg);
          scanner.undo();
        }
      }
      
      bool elseFlag = false;
      if (a_getTok(token)) { // opt: else
        if (check_crit(token, token_type::type_keyword, key_type::key_else)) {
          elseFlag = true;
        } else {
          scanner.undo();
        }
      }
      
      if (elseFlag) {
        p_statement(suc, exists); // first statement
        if (!exists) {
          std::string errMsg = "Missing statements from <if_statement><else>";
          reportError(token, err_type::error, errMsg);
        }
          
        if (exists && a_getTok(token) && // ;
            !check_crit(token, token_type::type_symb, symb_type::symb_semicolon)) {
          std::string errMsg = "Missing ';' from <if_statement><else><statement>";
          reportError(token, err_type::error, errMsg);
          scanner.undo();
        }
        
        while (exists) { // statement repeat
          p_statement(suc, exists); // unused suc
          
          if (exists && a_getTok(token) && // ;
              !check_crit(token, token_type::type_symb, symb_type::symb_semicolon)) {
            std::string errMsg = "Missing ';' from <if_statement><else><statement>";
            reportError(token, err_type::error, errMsg);
            scanner.undo();
          }
        }
      }
      
      if (a_getTok(token) && // end
          !check_crit(token, token_type::type_keyword, key_type::key_end)) {
        std::string errMsg = "Missing 'end' from <if_statement>";
        reportError(token, err_type::error, errMsg);
        eat_misspelling(token);
      }
      
      if (a_getTok(token) && // if
          !check_crit(token, token_type::type_keyword, key_type::key_if)) {
        std::string errMsg = "Missing 'if' from <if_statement><end>";
        reportError(token, err_type::error, errMsg);
        eat_misspelling(token); // statements after end if can start with an identifier
        //scanner.undo();
      }
    }
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

void p_argument_list(bool &success, bool &exists) {
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
    //p_program(suc);
    
    // DEBUG
    tok token;
    
    while (getTok(token)) {
      printTok(token);
    }
    //
    
    scanner.deinit();
  } else {
    std::cout << "Too many or too little args!";
  }

  return 0;
}
