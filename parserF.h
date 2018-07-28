#ifndef PARSERF_H
#define PARSERF_H

#include "codegen.h"

// 'exists safe' (tree ends with an 'exists checker', exists 
//   returned by this function can be trusted)
// 'exists checker' (sets exists to TRUE and checks to see if
//   obj exists, if does not exist, exist is set to FALSE)

//====================== Parser functions ======================//
void p_program();

std::string p_program_header();

void p_program_body(std::string);

// No type check needed, only creation of symbols (vars and procedures)
// exists safe
std::string p_declaration(bool &, 
    symbol* &, symbol_table &, bool &);

// exists safe
std::string p_procedure_declaration(bool &, 
    symbol* &, symbol_table &);

// exists checker
void p_procedure_header(bool &);

// exists safe?
void p_parameter_list(bool &);

// exists safe
void p_parameter(bool &);

void p_procedure_body();

// exists safe
std::string p_variable_declaration(bool &, 
    symbol* &, symbol_table &); // type check complete

// exists checker
obj_type p_type_mark(bool &); // type check complete

void p_lower_bound(bool &, int &); // type check complete

void p_upper_bound(bool &, int &); // type check complete

// exists safe
void p_statement(bool &);

// exists safe
void p_procedure_call(bool &);

// exists safe
void p_assignment_statement(bool &);

// exists safe
void p_destination(bool &);

// exists checker
void p_if_statement(bool &);

// exists checker
void p_loop_statement(bool &);

// exists checker
void p_return_statement(bool &);

// cannot error | exists checker
std::string p_identifier(bool &); // type check complete

// exists safe
void p_expression(bool &);

// exists safe
void p_expression_pr(bool &);

// exists safe
void p_arithOp(bool &);

// exists safe
void p_arithOp_pr(bool &);

// exists safe
void p_relation(bool &);

// exists safe
void p_relation_pr(bool &);

// exists safe
void p_term(bool &);

// exists safe
void p_term_pr(bool &);

// exists checker
void p_factor(bool &);

// exists checker (basically destination)
void p_name(bool &);

// exists safe?
void p_argument_list(bool &);

// cannot error | exists checker
factor p_number(bool &); // type check complete

// cannot error | exists checker
void p_string(bool &);

// cannot error | exists checker
void p_char(bool &);

#endif

