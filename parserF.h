#ifndef PARSERF_H
#define PARSERF_H

#include "codegen.h"

// 'exists safe' (tree ends with an 'exists checker', exists 
//   returned by this function can be trusted)
// 'exists checker' (sets exists to TRUE and checks to see if
//   obj exists, if does not exist, exist is set to FALSE)

//====================== Parser functions ======================//
void p_program();

void p_program_header();

void p_program_body();

// exists safe
void p_declaration(bool &);

// exists safe
void p_procedure_declaration(bool &);

// exists checker
void p_procedure_header(bool &);

// exists safe?
void p_parameter_list(bool &);

// exists safe
void p_parameter(bool &);

void p_procedure_body();

// exists safe
void p_variable_declaration(bool &);

// exists checker
void p_type_mark(bool &);

void p_lower_bound();

void p_upper_bound();

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
std::string p_identifier(bool &);

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
void p_number(bool &);

// cannot error | exists checker
void p_string(bool &);

// cannot error | exists checker
void p_char(bool &);

#endif

