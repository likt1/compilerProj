#ifndef CODEGEN_H
#define CODEGEN_H

enum tbl_type {
  tbl_proc,
  tbl_obj
};

enum param_type {
  param_in,
  param_out,
  param_inout
};

enum obj_type {
  obj_integer,
  obj_float,
  obj_string,
  obj_bool,
  obj_char,
  obj_temp
};

enum st_type {
  st_assign,
  st_if,
  st_loop,
  st_return,
  st_proc_call
};

enum op_type {
  op_minus,       // -
  op_amper,       // &
  op_straight,    // |
  op_plus,        // +
  op_smaller,     // <
  op_greater,     // >
  op_smaller_eq,  // <=
  op_greater_eq,  // >=
  op_equals,      // ==
  op_not_equals,  // !=
  op_multi,       // *
  op_div,         // /
};

struct factor {
  obj_type objType;
  std::string name;
  bool neg;
  union {
    int i;
    float f;
    char c;
    bool b;
    int tmp;
  };
};

class symbol {
public:
  tbl_type tblType;
};

class statement {
public:
  st_type stateType;
};

class expression {
public:
  factor left;
  factor right;
  op_type op;
};

#define symbol_table std::unordered_map<std::string, symbol>
#define symbol_elm std::pair<std::string, symbol>
#define statements_vector std::vector<statement>
#define expression_vector std::vector<expression>

class block {
public:
  symbol_table scope;
  statements_vector statements;
};

class procedure : public symbol {
public:
  block local;
};

class object : public symbol {
public:
  obj_type objType;
  int lb;
  int ub;
};

class param : public object {
public:
  param_type paramType;
};

class assign_statement : public statement {
public:
  object dest;
  expression_vector destLoc;
  expression_vector assignExpression;
};

class if_statement : public statement {
public:
  expression_vector test;
  statements_vector statements;
  statements_vector elseStatements;
};

class loop_statement : public statement {
public:
  //assign_statement assignment;
  statements_vector statements;
  expression_vector contExpression;
};

// ??
class return_statement : public statement {
public:
  
};

#endif

