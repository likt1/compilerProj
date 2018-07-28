#ifndef CODEGEN_H
#define CODEGEN_H

enum obj_type {
  obj_integer,
  obj_float,
  obj_string,
  obj_bool,
  obj_char,
  obj_temp,
  obj_id
};

enum st_type {
  st_assign,
  st_if,
  st_loop,
  st_return,
  st_proc
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
  obj_type objType;
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
#define statements_vector std::vector<statement>
#define expression_vector std::vector<expression>

class block {
  symbol_table scope;
  statements_vector statements;
};

class procedure : symbol {
public:
  block local;
};

class object : symbol {
public:
  std::string name;
  int lb;
  int ub;
};

class assign_statement : statement {
public:
  object dest;
  expression_vector destLoc;
  expression_vector assignExpression;
};

class if_statement : statement {
public:
  expression_vector test;
  block local;
  block elseLocal;
};

class loop_statement : statement {
public:
  //assign_statement assignment;
  expression_vector contExpression;
  block local;
};

// ??
class return_statement : statement {
public:
  
};

#endif

