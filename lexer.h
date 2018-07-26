#ifndef LEXER_H
#define LEXER_H

#include "error.h"
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>

// enum token type TODO
enum token_type {
  integer,
  floating,
  character,
  string,
  symbol, // operator
  identifier,
  keyword
};

// enum symbol TODO
enum symbol_type {
  add,
  sub,
  multi,
  divider
};

// enum keyword TODO
enum keyword_type {
  res_if,
  res_else
};

// job object for a sample job TODO
struct tok {
  token_type tokenType;
  std::string name;
  union {
    int i;
    float f;
    char c;
    symbol_type s;
    keyword_type r;
  };
  int linePos; // line of first char
  int charPos; // loc of first char
};

class lexer {

private:
  std::unordered_map<const char*, keyword_type> keywordL;
  error_list* errL;
  
  std::fstream fs;
  
  void reportError(err_type, char*);

public:
  int curLine;
  int curChar;
  
  bool backtracking;
  int storedLine;
  int storedChar;

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
  bool init(char*, error_list*);
  
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
