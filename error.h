#ifndef ERROR_H
#define ERROR_H

#include <vector>
#define error_list std::vector<error_obj>

enum err_type {
  warning,
  error,
  crit_error
};

// defines error obj
struct error_obj {
  err_type errT;
  char msg[255];
  int lineNum;
  int charNum;
};

#endif
