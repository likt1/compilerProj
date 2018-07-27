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

//====================== Globals ======================//
// symbol tables
symbol_table globalST;
std::stack<symbol_table> localSTs;
// error list
error_list errList;
// error flags (parse error list and if there is a large error then disable
bool genCode = true;
bool abortFlag = false;
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

//====================== Parser functions ======================//
void p_program(bool &);
void p_program_header(bool &);
void p_program_body(bool &);
void p_declaration(bool &, bool);
void p_parameter_list(bool &);
void p_parameter(bool &);
void p_procedure_body(bool &);
void p_variable_declaration(bool &);
void p_type_mark(bool &);
void p_lower_bound(bool &);
void p_upper_bound(bool &);
void p_statement(bool &, bool);
void p_procedure_call(bool &);
void p_assignment_statement(bool &);
void p_destination(bool &);
void p_if_statement(bool &);
void p_loop_statement(bool &);
void p_return_statement(bool &);
void p_identifier(bool &);
void p_expression(bool &);
void p_expression_pr(bool &);
void p_arithOp(bool &);
void p_arithOp_pr(bool &);
void p_relation(bool &);
void p_relation_pr(bool &);
void p_term(bool &);
void p_term_pr(bool &);
void p_factor(bool &);
void p_name(bool &);
void p_argument_list(bool &);
void p_number(bool &);
void p_string(bool &);
void p_char(bool &);

void p_program(bool &success) {  
  tok token; bool suc = true;
  
  p_program_header(suc);
  
  p_program_body(suc);
  
  if (getTok(token)) { // .
    if (!(token.tokenType == token_type::type_symb && token.s == symb_type::symb_period)) {
      std::string errMsg = "Missing . from <program>";
      reportError(token, err_type::error, errMsg);
    }
  } else {
    abortFlag = true;
  }
}

void p_program_header(bool &success) {
  if (!abortFlag) {
    tok token; bool suc = true;
    
    if (getTok(token)) { // program
      if (!(token.tokenType == token_type::type_keyword && token.k == key_type::key_program)) {
        std::string errMsg = "Missing 'program' from <program_header>";
        reportError(token, err_type::error, errMsg);
      }
    } else {
      abortFlag = true;
    }
    
    p_identifier(suc);
    
    if (!abortFlag && getTok(token)) { // is
      if (!(token.tokenType == token_type::type_keyword && token.k == key_type::key_is)) {
        std::string errMsg = "Missing 'is' from <program_header>";
        reportError(token, err_type::error, errMsg);
      }
    } else {
      abortFlag = true;
    }
  }
}

void p_program_body(bool &success) {
  if (!abortFlag) {
    tok token; bool suc = true;
    
    bool igError = false;
    while (suc) { // declaration repeat
      p_declaration(suc, igError);
      
      if (getTok(token)) { // ;
        if (!(token.tokenType == token_type::type_symb && token.s == symb_type::symb_semicolon)) {
          std::string errMsg = "Missing ';' from <program_body><declaration>";
          reportError(token, err_type::error, errMsg);
        }
      } else {
        abortFlag = true;
      }
      igError = true;
    }
    
    if (!abortFlag && getTok(token)) { // begin
      if (!(token.tokenType == token_type::type_keyword && token.k == key_type::key_begin)) {
        std::string errMsg = "Missing 'begin' from <program_body>";
        reportError(token, err_type::error, errMsg);
      }
    } else {
      abortFlag = true;
    }
    
    igError = false;
    while (suc) { // statement repeat
      p_statement(suc, igError);
      
      if (getTok(token)) { // ;
        if (!(token.tokenType == token_type::type_symb && token.s == symb_type::symb_semicolon)) {
          std::string errMsg = "Missing ';' from <program_body><declaration>";
          reportError(token, err_type::error, errMsg);
        }
      } else {
        abortFlag = true;
      }
      igError = true;
    }
    
    if (!abortFlag && getTok(token)) { // end
      if (!(token.tokenType == token_type::type_keyword && token.k == key_type::key_end)) {
        std::string errMsg = "Missing 'end' from <program_body>";
        reportError(token, err_type::error, errMsg);
      }
    } else {
      abortFlag = true;
    }
    
    if (!abortFlag && getTok(token)) { // program
      if (!(token.tokenType == token_type::type_keyword && token.k == key_type::key_program)) {
        std::string errMsg = "Missing 'program' from <program_body>";
        reportError(token, err_type::error, errMsg);
      }
    } else {
      abortFlag = true;
    }
  }
}

void p_declaration(bool &success, bool suppressE) {
  
}

void p_parameter_list(bool &success) {
  
}

void p_parameter(bool &success) {
  
}

void p_procedure_body(bool &success) {
  
}

void p_variable_declaration(bool &success) {
  
}

void p_type_mark(bool &success) {
  
}

void p_lower_bound(bool &success) {
  
}

void p_upper_bound(bool &success) {
  
}

void p_statement(bool &success, bool suppressE) {
  
}

void p_procedure_call(bool &success) {
  
}

void p_assignment_statement(bool &success) {
  
}

void p_destination(bool &success) {
  
}

void p_if_statement(bool &success) {
  
}

void p_loop_statement(bool &success) {
  
}

void p_return_statement(bool &success) {
  
}

void p_identifier(bool &success) {
  
}

void p_expression(bool &success) {
  
}

void p_expression_pr(bool &success) {
  
}

void p_arithOp(bool &success) {
  
}

void p_arithOp_pr(bool &success) {
  
}

void p_relation(bool &success) {
  
}

void p_relation_pr(bool &success) {
  
}

void p_term(bool &success) {
  
}

void p_term_pr(bool &success) {
  
}

void p_factor(bool &success) {
  
}

void p_name(bool &success) {
  
}

void p_argument_list(bool &success) {
  
}

void p_number(bool &success) {
  
}

void p_string(bool &success) {
  
}

void p_char(bool &success) {
  
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
