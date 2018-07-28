#ifndef CODEGEN_H
#define CODEGEN_H

enum tbl_type {
  tbl_proc,
  tbl_obj,
  tbl_param
};

const char* tblTypeToString(tbl_type type) {
  switch (type) {
    case tbl_proc: return "procedure";
    case tbl_obj: return "object";
    case tbl_param: return "parameter";
    default: return "unknown";
  }
}

enum param_type {
  param_in,
  param_out,
  param_inout
};

const char* paramTypeToString(param_type type) {
  switch (type) {
    case param_in: return "in";
    case param_out: return "out";
    case param_inout: return "inout";
    default: return "unknown";
  }
}

enum obj_type {
  obj_integer,
  obj_float,
  obj_string,
  obj_bool,
  obj_char,
  obj_temp,
  obj_none
};

const char* objTypeToString(obj_type type) {
  switch (type) {
    case obj_integer: return "integer";
    case obj_float: return "float";
    case obj_string: return "string";
    case obj_bool: return "bool";
    case obj_char: return "char";
    case obj_temp: return "temp";
    case obj_none: return "none";
    default: return "unknown";
  }
}

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

// TODO clean up symbol_tables
#define symbol_table std::unordered_map<std::string, symbol*>
#define symbol_elm std::pair<std::string, symbol*>
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

