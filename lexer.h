#ifndef LEXER_H
#define LEXER_H

#include "error.h"
#include <fstream>
#include <unordered_map>

#define global_map std::unordered_map<const char*, char*>

// enum token type TODO
enum tt {
  unknown,
  integer,
  string, 
  symbol,
  identifier
};

// enum symbol TODO
enum sym {
  add,
  sub,
  multi,
  divider
};

// job object for a sample job TODO
struct tok {
  tt tokenType;
  union {
    int i;
    sym s;
    char* name;
  };
};

class lexer {

private:
  global_map* globalM;
  error_list* errL;
  
  void reportError();

public:
  lexer();
  ~lexer();
  
  // needs program file to open and parse
  // needs global symbol table to save and find to
  // needs global error list to handle errors
  // DOES
  // Inits global map with identifiers
  bool init(char*, global_map*, error_list*); // Returns success/fail.
  
  tok scan();
  
};

#endif
