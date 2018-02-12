#ifndef ERROR_H
#define ERROR_H

#include <vector>
#define error_list std::vector<errorObj>

enum err_type {
  warning,
  error,
  crit_error
};

// defines error obj
struct errorObj {
  err_type errT;
  char* msg;
};

#endif
