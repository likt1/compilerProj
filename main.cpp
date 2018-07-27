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
void p_program();
void p_program_header();
void p_program_body();
void p_declaration();
void p_parameter_list();
void p_parameter();
void p_procedure_body();
void p_variable_declaration();
void p_type_mark();
void p_lower_bound();
void p_upper_bound();
void p_statement();
void p_procedure_call();
void p_assignment_statement();
void p_destination();
void p_if_statement();
void p_loop_statement();
void p_return_statement();
void p_identifier();
void p_expression();
void p_expression_pr();
void p_arithOp();
void p_arithOp_pr();
void p_relation();
void p_relation_pr();
void p_term();
void p_term_pr();
void p_factor();
void p_name();
void p_argument_list();
void p_number();
void p_string();
void p_char();

void p_program() {  
  tok token;
  p_program_header();
  p_program_body();
  if (getTok(token)) {
    if (!(token.tokenType == token_type::type_symb && token.s == symb_type::symb_period)) {
      std::string errMsg = "Period missing from program parsing";
      reportError(token, err_type::error, errMsg);
    }
  } else {
    abortFlag = true;
  }
}

void p_program_header() {
  
}

void p_program_body() {
  
}

void p_declaration() {
  
}

void p_parameter_list() {
  
}

void p_parameter() {
  
}

void p_procedure_body() {
  
}

void p_variable_declaration() {
  
}

void p_type_mark() {
  
}

void p_lower_bound() {
  
}

void p_upper_bound() {
  
}

void p_statement() {
  
}

void p_procedure_call() {
  
}

void p_assignment_statement() {
  
}

void p_destination() {
  
}

void p_if_statement() {
  
}

void p_loop_statement() {
  
}

void p_return_statement() {
  
}

void p_identifier() {
  
}

void p_expression() {
  
}

void p_expression_pr() {
  
}

void p_arithOp() {
  
}

void p_arithOp_pr() {
  
}

void p_relation() {
  
}

void p_relation_pr() {
  
}

void p_term() {
  
}

void p_term_pr() {
  
}

void p_factor() {
  
}

void p_name() {
  
}

void p_argument_list() {
  
}

void p_number() {
  
}

void p_string() {
  
}

void p_char() {
  
}

//====================== Main ======================//
int main(int argc, const char *argv[]) {
  if (argc == 2) {
    std::cout << "Compiler start " << argv[1] << "\n";
    scanner.init(argv[1], &errList);
    
    
    // begin parsing
    p_program();
    
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
