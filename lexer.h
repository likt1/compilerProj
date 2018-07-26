#ifndef LEXER_H
#define LEXER_H

#include "error.h"
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>

#define symbol_table std::unordered_map<const char*, tok>
// define a symbol table list to work with block independence?

// enum token type TODO
enum token_type {
  unknown,
  integer,
  floating,
  string,
  symbol,
  identifier
};

// enum symbol TODO
enum symbol_type {
  add,
  sub,
  multi,
  divider
};

// job object for a sample job TODO
struct tok {
  token_type tokenType;
  std::string name;
  union {
    int i;
    float f;
    symbol_type s;
  };
  int linePos;
  int charPos;
};

class lexer {

private:
  symbol_table* globalT;
  error_list* errL;
  
  std::fstream fs;
  
  void reportError(err_type, char*);

public:
  int curLine;
  int curChar;

  std::vector<tok> tokMem;
  int tokCursor;
  
  // ^ are things that can be private but are public for debugging purposes
  
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
  bool init(char*, symbol_table*, error_list*);
  
  // Closes the file
  bool deinit();
  
  // Returns the next token, if tokCursor is smaller than tokCount (we have
  //   backed up), return next token from tokMem
  // Will always increment tokCursor
  tok next_tok(symbol_table*); // current scope required)
  
  // Decrements tokCursor (calling next_tok right after will return the same 
  //   token as the cursor only moves behind one space)
  void undo();
  
};

#endif
