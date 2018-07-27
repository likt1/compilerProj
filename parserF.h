#ifndef PARSERF_H
#define PARSERF_H

// 'exists safe' (tree ends with an 'exists checker', exists 
//   returned by this function can be trusted)
// 'exists checker' (sets exists to TRUE and checks to see if
//   obj exists, if does not exist, exist is set to FALSE)

//====================== Parser functions ======================//
void p_program(bool &);

void p_program_header(bool &);

void p_program_body(bool &);

// exists safe
void p_declaration(bool &, bool &);

// exists safe
void p_procedure_declaration(bool &, bool &);

// exists checker
void p_procedure_header(bool &, bool &);

// exists safe?
void p_parameter_list(bool &, bool &);

// exists safe
void p_parameter(bool &, bool &);

void p_procedure_body(bool &);

// exists safe
void p_variable_declaration(bool &, bool &);

// exists checker
void p_type_mark(bool &, bool &);

void p_lower_bound(bool &);

void p_upper_bound(bool &);

// exists safe
void p_statement(bool &, bool &);

// exists safe
void p_procedure_call(bool &, bool &);

// exists safe
void p_assignment_statement(bool &, bool &);

// exists safe
void p_destination(bool &, bool &);

// exists checker
void p_if_statement(bool &, bool &);

// exists checker
void p_loop_statement(bool &, bool &);

// exists checker
void p_return_statement(bool &, bool &);

// cannot error | exists checker
void p_identifier(bool &, bool &);

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

void p_argument_list(bool &, bool &);

// cannot error
void p_number(bool &, bool &);

// cannot error
void p_string(bool &);

// cannot error
void p_char(bool &);

#endif

