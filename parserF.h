#ifndef PARSERF_H
#define PARSERF_H

#include "codegen.h"

// 'exists safe' (tree ends with an 'exists checker', exists 
//   returned by this function can be trusted)
// 'exists checker' (sets exists to TRUE and checks to see if
//   obj exists, if does not exist, exist is set to FALSE)

//====================== Parser functions ======================//
void p_program();

std::string p_program_header(); // type check complete

void p_program_body(std::string);

// No type check needed, only creation of symbols (vars and procedures)
// exists safe
std::string p_declaration(bool &, 
    symbol* &, symbol_table &, bool &);

// exists safe
std::string p_procedure_declaration(bool &, 
    symbol* &, symbol_table &);

// exists checker
std::string p_procedure_header(bool &,
    symbol* &, symbol_table &); // type check complete

// exists safe?
void p_parameter_list(bool &, bool &,
    int, procedure &); // type check complete ?

// exists safe
void p_parameter(bool &,
    int, procedure &); // type check complete ?

void p_procedure_body(procedure* &);

// exists safe
std::string p_variable_declaration(bool &, 
    symbol* &, symbol_table &); // type check complete

// exists checker
obj_type p_type_mark(bool &); // type check complete

void p_lower_bound(bool &, int &); // type check complete

void p_upper_bound(bool &, int &); // type check complete

// exists safe
void p_statement(bool &,
    symbol_table &);

// exists safe
void p_procedure_call(bool &,
    symbol_table &);

// exists safe
void p_assignment_statement(bool &,
    symbol_table &);

// exists safe
nameObj p_destination(bool &,
    symbol_table &); // type check complete (should)

// exists checker
void p_if_statement(bool &,
    symbol_table &);

// exists checker
void p_loop_statement(bool &,
    symbol_table &);

// exists checker
void p_return_statement(bool &);

// cannot error | exists checker
std::string p_identifier(bool &); // type check complete

// exists safe
factor p_expression(bool &,
    symbol_table &);

// exists safe
void p_expression_pr(bool &,
    symbol_table &);

// exists safe
void p_arithOp(bool &,
    symbol_table &);

// exists safe
void p_arithOp_pr(bool &,
    symbol_table &);

// exists safe
void p_relation(bool &,
    symbol_table &);

// exists safe
void p_relation_pr(bool &,
    symbol_table &);

// exists safe
factor p_term(bool &,
    symbol_table &);

// exists safe
factor p_term_pr(bool &,
    symbol_table &, factor);

// exists checker
factor p_factor(bool &,
    symbol_table &); // type check complete (should)

// exists checker (basically destination)
factor p_name(bool &,
    symbol_table &); // type check complete (should)

// exists safe?
void p_argument_list(bool &,
    procedure*);

// cannot error | exists checker
factor p_number(bool &); // type check complete

// cannot error | exists checker
factor p_string(bool &); // type check complete

// cannot error | exists checker
factor p_char(bool &); // type check complete

#endif

