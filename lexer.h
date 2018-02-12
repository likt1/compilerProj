#ifndef LEXER_H
#define LEXER_H

class lexer {

private:
  

public:
  jobs();
  ~jobs();
  
  // needs program file to open and parse
  // needs global symbol table to save and find to
  bool init(char*); // Returns success/fail.
  
};

#endif
