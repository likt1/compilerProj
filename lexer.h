#ifndef LEXER_H
#define LEXER_H

#include "error.h"
#include "ctype.h"
#include <fstream>
#include <iostream>
#include <unordered_map>

// enum token type
enum token_type {
  type_int,
  type_float,
  type_char,
  type_string,
  type_symb, // operator
  type_id,
  type_keyword,
  type_illegal
};

// enum symbol
enum symb_type {
  symb_period,      // .
  symb_op_paren,    // (
  symb_semicolon,   // ;
  symb_cl_paren,    // )
  symb_comma,       // ,
  symb_op_bracket,  // [
  symb_colon,       // :
  symb_cl_bracket,  // ]
  symb_minus,       // -
  symb_assign,      // :=
  symb_amper,       // &
  symb_straight,    // |
  symb_plus,        // +
  symb_smaller,     // <
  symb_greater,     // >
  symb_smaller_eq,  // <=
  symb_greater_eq,  // >=
  symb_equals,      // ==
  symb_not_equals,  // !=
  symb_multi,       // *
  symb_div,         // /
  symb_quote,       // '
  symb_db_quote     // "
};

// enum keyword 
enum key_type {
  key_program,
  key_is,
  key_begin,
  key_end,
  key_global,
  key_procedure,
  key_in,
  key_out,
  key_inout,
  key_integer,
  key_float,
  key_string,
  key_bool,
  key_char,
  key_if,
  key_then,
  key_else,
  key_for,
  key_return,
  key_not,
  key_true,
  key_false
};

// enum illegals
enum ill_type {
  ill_equals,
  ill_exc_pnt
};

// job object for a sample job TODO
struct tok {
  token_type tokenType;
  std::string name;
  union {
    int i;
    float f;
    char c;
    symb_type s;
    key_type k;
    ill_type il;
  };
  int linePos; // line of first char
  int charPos; // loc of first char
};

class lexer {

private:
  std::unordered_map<const char*, key_type> keywordL;
  error_list* errL;
  
  std::fstream fs;
  
  void reportError(err_type, std::string);
  
public:
  int curLine;
  int curChar;
  
  bool backtracking;
  bool fileEnd;
  int storedLine;
  int storedChar;

  std::vector<tok> tokMem;
  int tokCursor;
  
  // ^ are things that can be private but are public for debugging purposes
  
  // to see what we have
  const char* typeToString(token_type);
  const char* symbToString(symb_type);
  const char* keywToString(key_type);
  const char* illgToString(ill_type);
  
  lexer();
  ~lexer();
  
  // Requires
  //   Program file to open and parse
  //   Global symbol table to save and find to
  //   Global error list to handle errors
  // Does
  //   Inits pointer to global symbol table
  //   Inits pointer to global error list
  //   Sets tokCursor, curLine, curChar to 0
  // Returns
  //   Success/fail.
  bool init(const char*, error_list*);
  
  // Closes the file
  bool deinit();
  
  // Saves the next token in the input obj, if tokCursor is smaller than 
  //   tokCount (we have backed up), return next token from tokMem
  // Will always increment tokCursor
  // Returns true if no error and false otherwise
  bool next_tok(tok &);
  
  // Decrements tokCursor (calling next_tok right after will return the same 
  //   token as the cursor only moves behind one space)
  void undo();
  
};

#endif
