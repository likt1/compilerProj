// MAIN.CPP for compiler

#include "lexer.h"
#include "parserF.h"

#include <stack>

#define DEBUG_FLAG false
#define SCANNER_ONLY false

//====================== Globals ======================//
// symbol tables
symbol_table globalScope;
// error list
error_list errList;
// error flags (parse error list and if there is a large error then disable
bool genCode = true;
bool abortFlag = false;
// lexer
lexer scanner;

//====================== Populate functions ====================//
void populateGlobalScope() {
  // populate globalScope with builtin functions
  procedure* proc; param* paramO; symbol_elm elem; std::string procN, paramN = "val"; 
  
  procN = "getbool"; proc = new procedure(); proc->tblType = tbl_type::tbl_proc;
  paramO = new param(); paramO->tblType = tbl_type::tbl_param; paramO->order = 1;
  paramO->paramType = param_type::param_out; paramO->objType = obj_type::obj_bool;
  elem = std::make_pair(paramN, paramO);
  proc->local.scope.insert(elem);
  elem = std::make_pair(procN, proc);
  globalScope.insert(elem);
  
  procN = "getinteger"; proc = new procedure(); proc->tblType = tbl_type::tbl_proc;
  paramO = new param(); paramO->tblType = tbl_type::tbl_param; paramO->order = 1;
  paramO->paramType = param_type::param_out; paramO->objType = obj_type::obj_integer;
  elem = std::make_pair(paramN, paramO);
  proc->local.scope.insert(elem);
  elem = std::make_pair(procN, proc);
  globalScope.insert(elem);
  
  procN = "getfloat"; proc = new procedure(); proc->tblType = tbl_type::tbl_proc;
  paramO = new param(); paramO->tblType = tbl_type::tbl_param; paramO->order = 1;
  paramO->paramType = param_type::param_out; paramO->objType = obj_type::obj_float;
  elem = std::make_pair(paramN, paramO);
  proc->local.scope.insert(elem);
  elem = std::make_pair(procN, proc);
  globalScope.insert(elem);
  
  procN = "getstring"; proc = new procedure(); proc->tblType = tbl_type::tbl_proc;
  paramO = new param(); paramO->tblType = tbl_type::tbl_param; paramO->order = 1;
  paramO->paramType = param_type::param_out; paramO->objType = obj_type::obj_string;
  elem = std::make_pair(paramN, paramO);
  proc->local.scope.insert(elem);
  elem = std::make_pair(procN, proc);
  globalScope.insert(elem);
  
  procN = "getchar"; proc = new procedure(); proc->tblType = tbl_type::tbl_proc;
  paramO = new param(); paramO->tblType = tbl_type::tbl_param; paramO->order = 1;
  paramO->paramType = param_type::param_out; paramO->objType = obj_type::obj_char;
  elem = std::make_pair(paramN, paramO);
  proc->local.scope.insert(elem);
  elem = std::make_pair(procN, proc);
  globalScope.insert(elem);
  
  procN = "putbool"; proc = new procedure(); proc->tblType = tbl_type::tbl_proc;
  paramO = new param(); paramO->tblType = tbl_type::tbl_param; paramO->order = 1;
  paramO->paramType = param_type::param_in; paramO->objType = obj_type::obj_bool;
  elem = std::make_pair(paramN, paramO);
  proc->local.scope.insert(elem);
  elem = std::make_pair(procN, proc);
  globalScope.insert(elem);
  
  procN = "putinteger"; proc = new procedure(); proc->tblType = tbl_type::tbl_proc;
  paramO = new param(); paramO->tblType = tbl_type::tbl_param; paramO->order = 1;
  paramO->paramType = param_type::param_in; paramO->objType = obj_type::obj_integer;
  elem = std::make_pair(paramN, paramO);
  proc->local.scope.insert(elem);
  elem = std::make_pair(procN, proc);
  globalScope.insert(elem);
  
  procN = "putfloat"; proc = new procedure(); proc->tblType = tbl_type::tbl_proc;
  paramO = new param(); paramO->tblType = tbl_type::tbl_param; paramO->order = 1;
  paramO->paramType = param_type::param_in; paramO->objType = obj_type::obj_float;
  elem = std::make_pair(paramN, paramO);
  proc->local.scope.insert(elem);
  elem = std::make_pair(procN, proc);
  globalScope.insert(elem);
  
  procN = "putstring"; proc = new procedure(); proc->tblType = tbl_type::tbl_proc;
  paramO = new param(); paramO->tblType = tbl_type::tbl_param; paramO->order = 1;
  paramO->paramType = param_type::param_in; paramO->objType = obj_type::obj_string;
  elem = std::make_pair(paramN, paramO);
  proc->local.scope.insert(elem);
  elem = std::make_pair(procN, proc);
  globalScope.insert(elem);
  
  procN = "putchar"; proc = new procedure(); proc->tblType = tbl_type::tbl_proc;
  paramO = new param(); paramO->tblType = tbl_type::tbl_param; paramO->order = 1;
  paramO->paramType = param_type::param_in; paramO->objType = obj_type::obj_char;
  elem = std::make_pair(paramN, paramO);
  proc->local.scope.insert(elem);
  elem = std::make_pair(procN, proc);
  globalScope.insert(elem);
}

//====================== Utility functions ======================//
void reportError(tok &token, err_type eT, std::string msg) {
  error_obj newError;
  newError.errT = eT;
  newError.msg = msg;
  newError.lineNum = token.linePos;
  newError.charNum = token.charPos;
  
  if (eT != err_type::warning) {
    genCode = false;
  }

  errList.push_back(newError);
  if (DEBUG_FLAG) {
    std::cout << "  (L: " << newError.lineNum;
    std::cout << "|C: " << newError.charNum;
    std::cout << ") [" << scanner.errTypeToString(newError.errT) << "] ";
    std::cout << newError.msg << "\n";
  }
}

std::string cleanDblQuotes(std::string str) {
  std::string out;
  for (size_t i = 0; i < str.size(); i++) {
    if (str[i] != '"') {
      out.push_back(str[i]);
    }
  }
  return out;
}

void print_dbg_message(const char* msg) {
  if (DEBUG_FLAG) {
    std::cout << msg;
  }
}

void printTok(tok &token) {
  std::cout << "(L: " << token.linePos << "|C: " << token.charPos << ") ";
  std::cout << scanner.typeToString(token.tokenType) << "\t[";
  std::cout << token.name << "]";
  switch (token.tokenType) {
    case type_int:
      std::cout << "-> " << token.i;
      break;
    case type_float:
      std::cout << "-> " << token.f;
      break;
    case type_char:
      std::cout << "-> " << token.c;
      break;
    case type_symb:
      std::cout << "-> " << scanner.symbToString(token.s);
      break;
    case type_illegal:
      std::cout << "-> " << scanner.illgToString(token.il);
      break;
  }
  std::cout << "\n";
}

void peekTok(tok &token) {
  scanner.next_tok(token);
  if (token.tokenType == type_string) {
    token.name = cleanDblQuotes(token.name);
  }
  scanner.undo();
}

bool getTok(tok &token) {
  if (!scanner.next_tok(token)) {
    genCode = false;
  } else {
    if (token.tokenType == type_string) {
      token.name = cleanDblQuotes(token.name);
    }
  }
  if (scanner.fileEnd) { // end of file abort
    print_dbg_message("End of file reached\n");
    abortFlag = true;
    return false;
  }
  if (DEBUG_FLAG) {
    printTok(token);
  }
  return true;
}

bool a_getTok(tok &token) {
  return !abortFlag && getTok(token);
}

bool check(tok &token, token_type type, int sec_type) {
  if (token.tokenType != type) {
    if (DEBUG_FLAG) {
      std::cout << scanner.typeToString(token.tokenType);
      std::cout << " vs [" << scanner.typeToString(type);
      switch (type) {
        case type_symb:
          std::cout << "|" << scanner.symbToString((symb_type)sec_type);
          break;
        case type_keyword:
          std::cout << "|" << scanner.keywToString((key_type)sec_type);
          break;
        case type_illegal:
          std::cout << "|" << scanner.illgToString((ill_type)sec_type);
          break;
      }
      std::cout << "] FALSE\n";
    }
    return false;
  }
  
  int tokenSecondary;
  switch (type) {
    case type_symb:
      tokenSecondary = token.s;
      if (DEBUG_FLAG) {
        std::cout << scanner.symbToString(token.s);
        std::cout << " vs " << scanner.symbToString((symb_type)sec_type);
      }
      break;
    case type_keyword:
      tokenSecondary = token.k;
      if (DEBUG_FLAG) {
        std::cout << scanner.keywToString(token.k);
        std::cout << " vs " << scanner.keywToString((key_type)sec_type);
      }
      break;
    case type_illegal:
      tokenSecondary = token.il;
      if (DEBUG_FLAG) {
        std::cout << scanner.illgToString(token.il);
        std::cout << " vs " << scanner.illgToString((ill_type)sec_type);
      }
      break;
  }
  if (tokenSecondary != sec_type) {
    print_dbg_message(" FALSE\n");
    return false;
  }
  print_dbg_message(" TRUE\n");
  return true;
}

void check_try() {
  
}

void eat_misspelling(tok &token) {
  if (token.tokenType != token_type::type_id) { // if not misspelling
    scanner.undo(); // do not eat
  } // otherwise do nothing (eat)
}

// All errors on a single line (with same message) should be condensed into 1
void print_errors() {
  if (errList.size() == 0) {
    std::cout << "No errors or warnings detected\n";
  } else {
    int linePos = -1; bool statement = false, consume = false;
    for (error_list::iterator i = errList.begin() ; i != errList.end(); ++i) {
    
      if ((*i).msg.compare("Incorrect start token in <statement>") == 0) {
        if (linePos < 0) {
          linePos = (*i).lineNum;
          std::cout << "  (L: " << linePos;
          std::cout << "|C: " << (*i).charNum;
          std::cout << ") [" << scanner.errTypeToString((*i).errT) << "] ";
          std::cout << "Malformed line in <statement>\n";
        } else {
          if (linePos != (*i).lineNum) {
            linePos = -1;
            --i;
          }
        }
      } else if ((*i).msg.compare("Incorrect start token in <declaration>") == 0) {
        if (linePos < 0) {
          linePos = (*i).lineNum;
          std::cout << "  (L: " << linePos;
          std::cout << "|C: " << (*i).charNum;
          std::cout << ") [" << scanner.errTypeToString((*i).errT) << "] ";
          std::cout << "Malformed line in <declaration>\n";
        } else {
          if (linePos != (*i).lineNum) {
            linePos = -1;
            --i;
          }
        }
      } else {
        if (linePos > -1) {
          linePos = -1;
        }
        std::cout << "  (L: " << (*i).lineNum;
        std::cout << "|C: " << (*i).charNum;
        std::cout << ") [" << scanner.errTypeToString((*i).errT) << "] ";
        std::cout << (*i).msg << "\n";
      }
    }
  }
}

symbol* findObj(std::string symbN, symbol_table &scope, tok &token) {
  if (scope.count(symbN) != 0) {
    return scope.find(symbN)->second;
  } else if (globalScope.count(symbN) != 0) {
    return globalScope.find(symbN)->second;
  } else {
    std::string errMsg = "Object does not exist or is not in scope";
    reportError(token, err_type::error, errMsg);
  }
  return NULL;
}

void checkArrayLength(obj_type &lType, obj_type &rType, factor &left, factor &right, symbol_table &scope, tok &token) {
  // check if both are ids, arrays, and not indexed, that their array sizes match
  if ((left.objType == obj_type::obj_id && right.objType == obj_type::obj_id) &&
      (left.obj.idx == false && right.obj.idx == false)) {
    object* lObj,* rObj;
    lObj = (object*) findObj(left.obj.name, scope, token);
    rObj = (object*) findObj(right.obj.name, scope, token);
    if (lObj == NULL || rObj == NULL || !isArrayLengthSame(lObj, rObj)) {
      std::string errMsg = "Array lengths do not match";
      reportError(token, err_type::error, errMsg);
    }
  }
  
  if (left.objType == obj_type::obj_id) {
    lType = left.obj.varType;
  } else {
    lType = left.objType;
  }
  
  if (right.objType == obj_type::obj_id) {
    rType = right.obj.varType;
  } else {
    rType = right.objType;
  }
}

/* parser paradigm
 *   If a token was incorrect in a required parsing, the token is placed back where
 *     it was, EXCEPT if it might be a spelling error (identifier at keyword location
 *     where it is guarenteed illegal).
 *   For DECLARATIONS and STATEMENTS, the parser will attempt to eat all tokens (as
 *     potential declarations and statements) until it finds the corresponding end word.
 *     If it does not, it will eat ALL TOKENS and that messes up a lot of parsing.
 */
 
/* type check and code gen paradigm
 *   Only add objects if they parse 100% correctly.  This means that statements or 
 *     declares that are otherwise correct but are missing only the semicolon will 
 *     NOT be added to the respective block symbol table or statement list and will 
 *     cause other places to throw uninit or out of scope identifier type check errors.
 *   When dealing with an upper bound that's smaller than the lower bound, switch 
 *     the two values.  Parsing and type checking is still legal.
 */

// TODO check that all error messages return valid token to the error throwing code
// Function definitions exist in header parserF.h
//====================== Parser functions ======================//
void p_program() {  
  tok token;
  
  std::string progN = p_program_header(); 
  
  p_program_body(progN);
  
  if (getTok(token)) { // .
    if (!check(token, token_type::type_symb, symb_type::symb_period)) {
      std::string errMsg = "Unknown token after <program>";
      reportError(token, err_type::error, errMsg);
    }
  } else {
    token.linePos = 0;
    scanner.next_tok(token);
    while (token.linePos == 0) {
      scanner.undo();
      scanner.undo();
      scanner.next_tok(token);
    }
    std::string errMsg = "Missing . from <program>";
    reportError(token, err_type::warning, errMsg);
  }
}

std::string p_program_header() {
  tok token;
  
  if (getTok(token) && // program
      !check(token, token_type::type_keyword, key_type::key_program)) {
    std::string errMsg = "Missing 'program' from <program_header>";
    reportError(token, err_type::error, errMsg);
    eat_misspelling(token);
  }
  
  bool reqExists = false;
  std::string progN = p_identifier(reqExists); 
  if (!reqExists) {
    std::string errMsg = "Missing 'identifier' from <program_header>";
    reportError(token, err_type::error, errMsg);
  }
  procedure* program = new procedure(); program->tblType = tbl_type::tbl_proc;
  symbol_elm elem (progN, program);
  globalScope.insert(elem);
  
  if (a_getTok(token) && // is
      !check(token, token_type::type_keyword, key_type::key_is)) {
    std::string errMsg = "Missing 'is' from <program_header>";
    reportError(token, err_type::error, errMsg);
    eat_misspelling(token);
  }
  
  return progN;
}

void p_program_body(std::string progN) {
  if (!abortFlag) {
    tok token; symbol_table::iterator program = globalScope.find(progN);
    procedure* programObj = (procedure*) program->second;
    
    bool exists = false;
    do { // declaration repeat
      symbol* declared; bool globalFlag;
      std::string decN = p_declaration(exists, declared, programObj->local.scope, globalFlag);
      
      bool addFlag = true;
      if (exists && a_getTok(token) && // ;
          !check(token, token_type::type_symb, symb_type::symb_semicolon)) {
        std::string errMsg = "Missing ';' from <program_body><declaration>";
        reportError(token, err_type::error, errMsg);
        scanner.undo();
        addFlag = false;
      }
      
      if (addFlag) {
        if (decN.size() > 0) { // if empty, declaration fired off an error already
          symbol_elm elem (decN, declared); // declared is not NULL so make element
          if (!globalFlag) {
            programObj->local.scope.insert(elem);
          } else {
            globalScope.insert(elem);
          }
        }
      }
    } while (exists && !abortFlag);
    
    if (a_getTok(token) && // begin
        !check(token, token_type::type_keyword, key_type::key_begin)) {
      std::string errMsg = "Missing 'begin' from <program_body>";
      reportError(token, err_type::error, errMsg);
      //eat_misspelling(token); // statements after begin can start with an identifier
      scanner.undo();
    }
    
    // TODO codegen
    do { // statement repeat
      p_statement(exists, programObj->local.scope); 
      
      if (exists && a_getTok(token) && // ;
          !check(token, token_type::type_symb, symb_type::symb_semicolon)) {
        std::string errMsg = "Missing ';' from <program_body><statement>";
        reportError(token, err_type::error, errMsg);
        scanner.undo();
      }
    } while (exists && !abortFlag);
    
    if (a_getTok(token) && // end
        !check(token, token_type::type_keyword, key_type::key_end)) {
      std::string errMsg = "Missing 'end' from <program_body>";
      reportError(token, err_type::error, errMsg);
      eat_misspelling(token);
    }
    
    if (a_getTok(token) && // program
        !check(token, token_type::type_keyword, key_type::key_program)) {
      std::string errMsg = "Missing 'program' from <program_body>";
      reportError(token, err_type::error, errMsg);
      eat_misspelling(token);
    }
  }
}

// will return the name of the identifier of the object with the object in 'declared'
// if there are errors, name will be "" and declared is NULL
std::string p_declaration(bool &exists, symbol* &declared, symbol_table &scope, bool &globalFlag) {
  if (!abortFlag) {
    tok token;
    
    globalFlag = false;
    if (getTok(token)) { // opt: global
      if (check(token, token_type::type_keyword, key_type::key_global)) {
        globalFlag = true;
      } else { // backtrack
        scanner.undo();
      }
    }
    
    // Procedure or variable?
    std::string decN;
    decN = p_procedure_declaration(exists, declared, scope);
    
    if (!exists) { // if it wasn't a procedure then it's a variable
      decN = p_variable_declaration(exists, declared, scope);
    } // if exists is still false then a declaration doesn't exist
    
    if (globalFlag && !exists) { // global with no declaration
      std::string errMsg = "Orphan 'global' in <declaration>";
      reportError(token, err_type::error, errMsg);
    }
    
    if (!exists) { // DECLARATIONS ALWAYS END WITH 'BEGIN'
      // so if there was no declaration, the next token has to be 'begin'
      // consume all invalid tokens until next declaration (or end of declarations) 
      if (a_getTok(token)) { 
        if (!check(token, token_type::type_keyword, key_type::key_begin)) {
          std::string errMsg = "Incorrect start token in <declaration>";
          reportError(token, err_type::error, errMsg);
          decN = p_declaration(exists, declared, scope, globalFlag); // recursive consume all
        } else { // if we find a declaration, undo the consume
          scanner.undo();
        }
      } else { // everything has been consumed, file ended
        return "";
      }
    }
    return decN;
  }
  return "";
}

std::string p_procedure_declaration(bool &exists, symbol* &declared, symbol_table &scope) {
  if (!abortFlag) {
   
    // create a new procedure if it exists and assign it to declared
    std::string procN = p_procedure_header(exists, declared, scope); 
    
    if (exists) { // if the procedure header exists then parse body
      if (procN.size() > 0) { // and the procedure name is valid
        // populate new procedure
        p_procedure_body((procedure* &) declared);
        
        return procN;
      } else {
        // still parse through the body, but do not create a procedure
        procedure* tmp = new procedure();
        p_procedure_body(tmp);
        delete tmp;
      }
    }
  }
  return "";
}

std::string p_procedure_header(bool &exists, symbol* &declared, symbol_table &scope) {
  if (!abortFlag) {
    tok token;
    
    exists = true;
    if (getTok(token) && // procedure exists?
        !check(token, token_type::type_keyword, key_type::key_procedure)) {
      exists = false;
      scanner.undo();
      //std::string errMsg = "Missing 'procedure' from <procedure_header>";
      //reportError(token, err_type::error, errMsg);
    }
    
    if (exists) {
      bool reqExists = false, suc = true;
      std::string procN = p_identifier(reqExists);
      if (!reqExists) {
        std::string errMsg = "Missing 'identifier' from <procedure_header>";
        reportError(token, err_type::error, errMsg);
        suc = false;
      }
      procedure* proc = new procedure(); proc->tblType = tbl_type::tbl_proc;
      
      if (scope.count(procN) != 0) {
        std::string errMsg = "Identifier name already exists in current scope from <procedure_declaration>";
        reportError(token, err_type::error, errMsg);
        suc = false;
      } else if (globalScope.count(procN) != 0) {
        std::string errMsg = "Identifier name already exists in global scope from <procedure_declaration>";
        reportError(token, err_type::error, errMsg);
        suc = false;
      } else {
        // assign procedure name (with procedure pointer) to itself
        symbol_elm elem (procN, (symbol*) proc);
        proc->local.scope.insert(elem);
      }
      
      if (a_getTok(token) && // (
          !check(token, token_type::type_symb, symb_type::symb_op_paren)) {
        std::string errMsg = "Missing '(' from <procedure_header>";
        reportError(token, err_type::error, errMsg);
        scanner.undo();
        suc = false;
      }
      
      bool hasParams = false;
      p_parameter_list(hasParams, suc, 1, *proc); // params are automatically added
      
      if (a_getTok(token) && // )
          !check(token, token_type::type_symb, symb_type::symb_cl_paren)) {
        std::string errMsg = "Missing ')' from <procedure_header>";
        reportError(token, err_type::error, errMsg);
        scanner.undo();
        suc = false;
      }
      
      if (suc) { // only if everything passed
        declared = (symbol*) proc; // assign obj to output
        return procN; // return name
      } else {
        delete proc;
      }
    }
  } else { // abort, no more file, therefore cannot exist
    exists = false;
  }
  return "";
}

void p_parameter_list(bool &exists, bool &suc, int order, procedure &proc) {
  if (!abortFlag) {
    tok token;
    
    bool prevParams = exists;
    p_parameter(exists, order, proc); // adds param to procedure's scope
    
    if (exists) {
      tok lookahead; peekTok(lookahead); // ,
      if (check(lookahead, token_type::type_symb, symb_type::symb_comma)) {
        getTok(token);
        p_parameter_list(exists, suc, order + 1, proc);
      } else {
        p_parameter(exists, order + 1, proc);
        if (exists) {
          std::string errMsg = "Missing ',' from <parameter_list>";
          reportError(token, err_type::error, errMsg);
          suc = false;
        } else {
          exists = true;
        }
      }
    }
    
    if (prevParams && !exists) {
      std::string errMsg = "Orphan ',' from <parameter_list>";
      reportError(token, err_type::error, errMsg);
      exists = true;
      suc = false;
    }
  } else {
    suc = false;
  }
}

void p_parameter(bool &exists, int order, procedure &proc) {
  if (!abortFlag) {
    tok token; bool suc = true;
    
    object* paramVals;
    std::string paramN = p_variable_declaration(exists, (symbol* &) paramVals, proc.local.scope);
    
    param* parameter = new param();
    if (paramN.size() > 0) { // valid variable declaration
      // move paramVals from obj
      parameter->tblType = tbl_type::tbl_param;
      parameter->objType = paramVals->objType;
      parameter->lb = paramVals->lb;
      parameter->ub = paramVals->ub;
    } else { // invalid, so remember to delete param obj
      suc = false;
    }
    
    if (exists && a_getTok(token)) { // in/out/inout
      if (check(token, token_type::type_keyword, key_type::key_in)) {
        parameter->paramType = param_type::param_in; // define in
      } else if (check(token, token_type::type_keyword, key_type::key_out)) {
        parameter->paramType = param_type::param_out; // define out
      } else if (check(token, token_type::type_keyword, key_type::key_inout)) {
        parameter->paramType = param_type::param_inout; // define inout
      } else {
        std::string errMsg = "Missing 'in/out/inout' from <parameter>";
        reportError(token, err_type::error, errMsg);
        scanner.undo();
        suc = false;
      }
    }
    
    if (suc) {
      // assign to procedure
      parameter->order = order;
      symbol_elm elem (paramN, (symbol*) parameter); // declared is not NULL so make element
      proc.local.scope.insert(elem);
    } else {
      delete parameter;
    }
  }
}

void p_procedure_body(procedure* &proc) {
  if (!abortFlag) {
    tok token;
    
    bool exists = false;
    do { // declaration repeat
      symbol* declared; bool globalFlag = false;
      std::string decN = p_declaration(exists, declared, proc->local.scope, globalFlag); 
      
      bool addFlag = true;
      if (exists && a_getTok(token) && // ;
          !check(token, token_type::type_symb, symb_type::symb_semicolon)) {
        std::string errMsg = "Missing ';' from <procedure_body><declaration>";
        reportError(token, err_type::error, errMsg);
        scanner.undo();
        addFlag = false;
      }
      
      if (addFlag) {
        if (decN.size() > 0) { // if empty, declaration fired off an error already
          symbol_elm elem (decN, declared); // declared is not NULL so make element
          if (!globalFlag) {
            proc->local.scope.insert(elem);
          } else {
            // according to project semantics, only those in the OUTERMOST SCOPE
            //   are considered for globalScope
            //globalScope.insert(elem); 
            proc->local.scope.insert(elem);
          }
        }
      }
    } while (exists && !abortFlag);
    
    if (a_getTok(token) && // begin
        !check(token, token_type::type_keyword, key_type::key_begin)) {
      std::string errMsg = "Missing 'begin' from <procedure_body>";
      reportError(token, err_type::error, errMsg);
      //eat_misspelling(token);
      scanner.undo();
    }
    
    // TODO code gen START HERE
    // check that statements are all correct
    // MAIN STATEMENTS THAT NEED CHECKING:
    //   assignment       |right can be assigned to left (which needs to exist)
    //   procedure call   |correct args for procedure (if it even is one)
    // ifs and loops just contain more assignments or procedure calls (if it's \
    //   not another if or loop.  These need to know current scope only (to send
    //   to their assignments and procedure calls)
    
    // When all typechecking is complete, update program version (should be the
    //   same) and start code gen.
    // Code gen needs to generate:
    //   declarations:
    //     variables have their memory spaces
    //     procedures have their memory and code spaces
    //   main program:
    //     calls variables and procedures in program execution space
    //       these calls remember current location so that when the procedure
    //       returns, it gets back to where it is
    
    do { // statement repeat
      p_statement(exists, proc->local.scope); // statement should be valid when it gets here
      
      if (exists && a_getTok(token) && // ;
          !check(token, token_type::type_symb, symb_type::symb_semicolon)) {
        std::string errMsg = "Missing ';' from <procedure_body><statement>";
        reportError(token, err_type::error, errMsg);
        scanner.undo();
      }
    } while (exists && !abortFlag);
     
    if (a_getTok(token) && // end
        !check(token, token_type::type_keyword, key_type::key_end)) {
      std::string errMsg = "Missing 'end' from <procedure_body>";
      reportError(token, err_type::error, errMsg);
      eat_misspelling(token);
    }
    
    if (a_getTok(token) && // procedure
        !check(token, token_type::type_keyword, key_type::key_procedure)) {
      std::string errMsg = "Missing 'procedure' from <procedure_body>";
      reportError(token, err_type::error, errMsg);
      eat_misspelling(token);
    }
  }
}

std::string p_variable_declaration(bool &exists, symbol* &declared, symbol_table &scope) {
  if (!abortFlag) {
    tok token;
    
    obj_type type = p_type_mark(exists);
    
    if (exists) {
      bool reqExists = false, suc = true;
      std::string varN = p_identifier(reqExists);
      if (!reqExists) {
        std::string errMsg = "Missing 'identifier' from <variable_declaration>";
        reportError(token, err_type::error, errMsg);
        suc = false;
      }
      
      // create new obj
      object* var = new object();
      var->tblType = tbl_type::tbl_obj;
      var->objType = type;
      var->ub = 0; var->lb = 0;
      if (scope.count(varN) != 0) {
        std::string errMsg = "Identifier name already exists in current scope from <variable_declaration>";
        reportError(token, err_type::error, errMsg);
        suc = false;
      } else if (globalScope.count(varN) != 0) {
        std::string errMsg = "Identifier name already exists in global scope from <variable_declaration>";
        reportError(token, err_type::error, errMsg);
        suc = false;
      }
      
      bool array = false;
      if (a_getTok(token)) { // optional [
        if (check(token, token_type::type_symb, symb_type::symb_op_bracket)) {
          array = true;
        } else {
          scanner.undo();
        }
      }
      
      if (array) {
        p_lower_bound(suc, var->lb);
        
        if (a_getTok(token) && // :
            !check(token, token_type::type_symb, symb_type::symb_colon)) {
          std::string errMsg = "Missing ':' from <variable_declaration>";
          reportError(token, err_type::error, errMsg);
          scanner.undo();
          suc = false;
        }
        
        p_upper_bound(suc, var->ub);
        
        if (a_getTok(token) && // ]
            !check(token, token_type::type_symb, symb_type::symb_cl_bracket)) {
          std::string errMsg = "Missing ']' from <variable_declaration>";
          reportError(token, err_type::error, errMsg);
          scanner.undo();
          suc = false;
        }
        
        if (var->ub < var->lb) {
          int tmp = var->ub;
          var->ub = var->lb;
          var->lb = tmp;
        }
      }
      
      if (suc) { // only if everything passed
        declared = (symbol*) var; // assign obj to output
        return varN; // return name
      } else {
        delete var;
      }
    }
  }
  return "";
}

obj_type p_type_mark(bool &exists) {
  if (!abortFlag) {
    tok token;
    
    exists = true;
    if (a_getTok(token)) { // refer to type_mark in specification
      if (check(token, token_type::type_keyword, key_type::key_integer)) {
        return obj_type::obj_integer; // integer
      } else if(check(token, token_type::type_keyword, key_type::key_float)) {
        return obj_type::obj_float; // float
      } else if(check(token, token_type::type_keyword, key_type::key_string)) {
        return obj_type::obj_string; // string
      } else if(check(token, token_type::type_keyword, key_type::key_bool)) {
        return obj_type::obj_bool; // bool
      } else if(check(token, token_type::type_keyword, key_type::key_char)) {
        return obj_type::obj_char; // char
      } else {
        scanner.undo();
        exists = false;
        return obj_type::obj_none;
      }
    }
  } else {
    exists = false;
  }
}

// suc to control if the above declare should succeed
void p_lower_bound(bool &suc, int &val) {
  if (!abortFlag) {
    tok token;
    
    bool neg = false;
    if (a_getTok(token)) { // optional -
      if (check(token, token_type::type_symb, symb_type::symb_minus)) {
        neg = true;
      } else {
        scanner.undo();
      }
    }
    
    bool reqExists = false;
    factor getObj = p_number(reqExists);
    if (!reqExists) {
      std::string errMsg = "Missing 'number' from <lower_bound>";
      reportError(token, err_type::error, errMsg);
      suc = false;
    } else {
      // TYPECHECK only accept ints
      if (getObj.objType == obj_type::obj_integer) {
        if (!neg) {
          val = getObj.i;
        } else {
          val = -getObj.i;
        }
      } else {
        std::string errMsg = "Illegal float in <lower_bound>";
        reportError(token, err_type::error, errMsg);
        suc = false;
      }
    }
  } else {
    suc = false;
  }
}

// suc to control if the above declare should succeed
void p_upper_bound(bool &suc, int &val) {
  if (!abortFlag) {
    tok token;
    
    bool neg = false;
    if (a_getTok(token)) { // optional -
      if (check(token, token_type::type_symb, symb_type::symb_minus)) {
        neg = true;
      } else {
        scanner.undo();
      }
    }
    
    bool reqExists = false;
    factor getObj = p_number(reqExists);
    if (!reqExists) {
      std::string errMsg = "Missing 'number' from <upper_bound>";
      reportError(token, err_type::error, errMsg);
      suc = false;
    } else {
      // TYPECHECK only accept ints
      if (getObj.objType == obj_type::obj_integer) {
        if (!neg) {
          val = getObj.i;
        } else {
          val = -getObj.i;
        }
      } else {
        std::string errMsg = "Illegal float in <upper_bound>";
        reportError(token, err_type::error, errMsg);
        suc = false;
      }
    }
  } else {
    suc = false;
  }
}

void p_statement(bool &exists, symbol_table &scope) {
  if (!abortFlag) {
    tok token;
    
    p_assignment_statement(exists, scope); // needs check
    if (!exists) {
      p_if_statement(exists, scope);
    }
    if (!exists) {
      p_loop_statement(exists, scope);
    }
    if (!exists) {
      p_return_statement(exists);
    }
    if (!exists) {
      p_procedure_call(exists, scope); // needs check
    } // if exists is still false, a statement does not exist
    
    if (!exists) { // STATEMENTS ALWAYS END IN EITHER 'ELSE' OR 'END'
      // so if there was no statement, the next token has to be 'else' or 'end'
      getTok(token); // consume all invalid tokens until next statement (or end of statements) 
      if (!check(token, token_type::type_keyword, key_type::key_end)
          && !check(token, token_type::type_keyword, key_type::key_else)) {
        // unknown token in declaration
        std::string errMsg = "Incorrect start token in <statement>";
        reportError(token, err_type::error, errMsg);
        p_statement(exists, scope);
      } else {
        scanner.undo(); // if we find a statement, undo the consume
      }
    }
  }
}

void p_procedure_call(bool &exists, symbol_table &scope) {
  if (!abortFlag) {
    tok token;
    
    std::string procN = p_identifier(exists);
    
    // since procedure call is last on the statement list, if an identifier exists
    //   it has to be a procedure call
    if (exists) {
      print_dbg_message("is procedure\n");
      symbol* tmp; procedure* proc;
      tmp = findObj(procN, scope, token);
      
      // make sure it is a procedure
      if (tmp != NULL && tmp->tblType == tbl_type::tbl_proc) {
        proc = (procedure*) tmp;
      } else {
        std::string errMsg = "Symbol called is not a procedure <procedure_call>";
        reportError(token, err_type::error, errMsg);
      }
      
      if (a_getTok(token) && // (
          !check(token, token_type::type_symb, symb_type::symb_op_paren)) {
        std::string errMsg = "Missing '(' from <procedure_call>";
        reportError(token, err_type::error, errMsg);
        scanner.undo();
      }
      
      bool hasArgs = false;
      p_argument_list(hasArgs, scope, proc, 1); // will check to see that the params meet the proc 
      // and add statements if they all work out
      
      if (a_getTok(token) && // )
          !check(token, token_type::type_symb, symb_type::symb_cl_paren)) {
        std::string errMsg = "Missing ')' from <procedure_call>";
        reportError(token, err_type::error, errMsg);
        scanner.undo();
      }
    }
  }
}

// ASSIGNMENT TYPE CONVERSION
//   int <-> float
//   int <-> bool
void p_assignment_statement(bool &exists, symbol_table &scope) {
  if (!abortFlag) {
    tok token;
    
    nameObj dest = p_destination(exists, scope);
    
    if (exists) {
      tok lookahead; peekTok(lookahead); // check = or :=
      if (check(lookahead, token_type::type_symb, symb_type::symb_assign)) {
        getTok(token);
        print_dbg_message("is assignment\n");
      } else if (check(lookahead, token_type::type_illegal, ill_type::ill_equals) ||
                 check(lookahead, token_type::type_symb, symb_type::symb_equals)) {
        std::string errMsg = "Malformed ':=' from <assignment_statement>";
        getTok(token);
        reportError(token, err_type::error, errMsg);
        print_dbg_message("is assignment\n");
      } else {
        exists = false;
        scanner.undo();
      }
    }
    
    if (exists) {
      factor expF = p_expression(exists, scope);
      if (!exists) {
        std::string errMsg = "Missing <expression> from <assignment_statement>";
        reportError(token, err_type::error, errMsg);
      } else {
        // TODO typecheck left (destination) vs right
        obj_type lType, rType; factor left;
        left.objType = obj_type::obj_id; left.obj = dest;
        checkArrayLength(lType, rType, left, expF, scope, token);
        
        /* TODO codegen and typecheck
        bool suc = true, floatFlag = false;
        if (lType == obj_type::obj_integer && rType == obj_type::obj_integer) {
          if (opType != op_type::op_multi) {
          } else { // div
          }
          
        } else if (lType == obj_type::obj_integer && rType == obj_type::obj_float) {
          floatFlag = true;
          if (opType != op_type::op_multi) {
          } else { // div
          }
          
        } else if (lType == obj_type::obj_float && rType == obj_type::obj_integer) {
          floatFlag = true;
          if (opType != op_type::op_multi) {
          } else { // div
          }
          
        } else if (lType == obj_type::obj_float && rType == obj_type::obj_float) {
          floatFlag = true;
          if (opType != op_type::op_multi) {
          } else { // div
          }
          
        } else {
          std::string errMsg = "Incorrect types for multiply <term>";
          reportError(token, err_type::error, errMsg);
          suc = false;
        }*/
        
        
      }
    }
  }
}

nameObj p_destination(bool &exists, symbol_table &scope) {
  nameObj out; out.name = "";
  if (!abortFlag) {
    tok token;
    
    out.name = p_identifier(exists);
    
    if (exists) {
      out.idx = false;
      if (a_getTok(token)) { // optional [
        if (check(token, token_type::type_symb, symb_type::symb_op_bracket)) {
          out.idx = true;
        } else {
          scanner.undo();
        }
      }
      
      if (out.idx) { // if this is correct, must be a destination
        // check to see if it's in scope
        symbol* tmp; object* obj;
        tmp = findObj(out.name, scope, token);
        
        // make sure it is an object
        if (tmp != NULL && (tmp->tblType == tbl_type::tbl_obj || tmp->tblType == tbl_type::tbl_param)) {
          obj = (object*) tmp;
          out.varType = obj->objType; // save obj type into factor nameObj (so we don't have to look it up in the future)
        } else {
          std::string errMsg = "Symbol called is not an object <destination>";
          reportError(token, err_type::error, errMsg);
          out.name = "";
        }
      
        factor expF = p_expression(exists, scope);
        if (!exists) {
          std::string errMsg = "Missing index <expression> from <destination>";
          reportError(token, err_type::error, errMsg);
          out.name = "";
        }
        
        // check to see that expression is an int within lb and ub
        if (exists && obj != NULL) {
          if (obj->lb == obj->ub) { // obj is not an array so throw error
            std::string errMsg = "Cannot index into non-array object <destination>";
            reportError(token, err_type::error, errMsg);
            out.name = "";
          } else {
            if (expF.objType == obj_type::obj_integer) { // retF is integer
              // do int bounds check
              if (expF.i >= obj->lb && expF.i <= obj->ub) {
                out.boundsIntPos = expF.i;
              } else {
                std::string errMsg = "Out of bounds error <destination>";
                reportError(token, err_type::error, errMsg);
                out.name = "";
              }
            } else {
              if (expF.objType == obj_type::obj_id && 
                  expF.obj.varType == obj_type::obj_integer) {
                // if an int object, check to see that it's either not an array, 
                //   or an index into an array TODO
                // THIS SEEMS LIKE A RUNTIME THING
                //   IF THE EXPRESSION IS A VARIABLE SET BY getInteger(), 
                //   WE CANNOT DO THIS DURING COMPILE TIME
                object* expO = (object*) findObj(expF.obj.name, scope, token);
                if (expO != NULL) {
                  if (expO->lb != expO->ub && !expF.obj.idx) {
                    std::string errMsg = "Index cannot be an array <destination>";
                    reportError(token, err_type::error, errMsg);
                    out.name = "";
                  } else {
                    out.boundsIntPos = expF.i;
                    out.boundsPos = expF.obj.name;
                  }
                }
              } else {
                std::string errMsg = "Index must resolve into an integer <destination>";
                reportError(token, err_type::error, errMsg);
                out.name = "";
              }
            }
          }
        }
        
        if (a_getTok(token) && // ]
            !check(token, token_type::type_symb, symb_type::symb_cl_bracket)) {
          std::string errMsg = "Missing ']' from <destination>";
          reportError(token, err_type::error, errMsg);
          scanner.undo();
          out.name = "";
        }
        exists = true;
      }
    }
  } else {
    exists = false;
  }
  return out;
}

void p_if_statement(bool &exists, symbol_table &scope) {
  if (!abortFlag) {
    tok token;
    
    exists = true;
    if (a_getTok(token) && // if
        !check(token, token_type::type_keyword, key_type::key_if)) {
      exists = false;
      scanner.undo();
      //std::string errMsg = "Missing '(' from <procedure_call>";
      //reportError(token, err_type::error, errMsg);
    }
    
    if (exists) {
      if (a_getTok(token) && // (
          !check(token, token_type::type_symb, symb_type::symb_op_paren)) {
        std::string errMsg = "Missing '(' from <if_statement>";
        reportError(token, err_type::error, errMsg);
        scanner.undo();
      }
      
      bool reqExists = false;
      p_expression(reqExists, scope);
      if (!reqExists) {
        std::string errMsg = "Missing <expression> from <if_statement>";
        reportError(token, err_type::error, errMsg);
      }
      
      if (a_getTok(token) && // )
          !check(token, token_type::type_symb, symb_type::symb_cl_paren)) {
        std::string errMsg = "Missing ')' from <if_statement>";
        reportError(token, err_type::error, errMsg);
        scanner.undo();
      }
      
      if (a_getTok(token) && // then
          !check(token, token_type::type_keyword, key_type::key_then)) {
        std::string errMsg = "Missing 'then' from <if_statement>";
        reportError(token, err_type::error, errMsg);
        //eat_misspelling(token); // statements after then can start with an identifier
        scanner.undo();
      }
      
      bool sExists = false;
      p_statement(sExists, scope); // first statement
      if (!sExists) {
        std::string errMsg = "Missing statements from <if_statement><then>";
        reportError(token, err_type::error, errMsg);
      }
        
      if (sExists && a_getTok(token) && // ;
          !check(token, token_type::type_symb, symb_type::symb_semicolon)) {
        std::string errMsg = "Missing ';' from <if_statement><then><statement>";
        reportError(token, err_type::error, errMsg);
        scanner.undo();
      }
      
      while (sExists) { // statement repeat
        p_statement(sExists, scope); 
        
        if (sExists && a_getTok(token) && // ;
            !check(token, token_type::type_symb, symb_type::symb_semicolon)) {
          std::string errMsg = "Missing ';' from <if_statement><then><statement>";
          reportError(token, err_type::error, errMsg);
          scanner.undo();
        }
      }
      
      bool elseFlag = false;
      if (a_getTok(token)) { // opt: else
        if (check(token, token_type::type_keyword, key_type::key_else)) {
          elseFlag = true;
        } else {
          scanner.undo();
        }
      }
      
      if (elseFlag) {
        p_statement(sExists, scope); // first statement
        if (!sExists) {
          std::string errMsg = "Missing statements from <if_statement><else>";
          reportError(token, err_type::error, errMsg);
        }
          
        if (sExists && a_getTok(token) && // ;
            !check(token, token_type::type_symb, symb_type::symb_semicolon)) {
          std::string errMsg = "Missing ';' from <if_statement><else><statement>";
          reportError(token, err_type::error, errMsg);
          scanner.undo();
        }
        
        while (sExists) { // statement repeat
          p_statement(sExists, scope); 
          
          if (sExists && a_getTok(token) && // ;
              !check(token, token_type::type_symb, symb_type::symb_semicolon)) {
            std::string errMsg = "Missing ';' from <if_statement><else><statement>";
            reportError(token, err_type::error, errMsg);
            scanner.undo();
          }
        }
      }
      
      if (a_getTok(token) && // end
          !check(token, token_type::type_keyword, key_type::key_end)) {
        std::string errMsg = "Missing 'end' from <if_statement>";
        reportError(token, err_type::error, errMsg);
        eat_misspelling(token);
      }
      
      if (a_getTok(token) && // if
          !check(token, token_type::type_keyword, key_type::key_if)) {
        std::string errMsg = "Missing 'if' from <if_statement><end>";
        reportError(token, err_type::error, errMsg);
        eat_misspelling(token); // statements after end if can start with an identifier
        //scanner.undo();
      }
    }
  }
}

void p_loop_statement(bool &exists, symbol_table &scope) {
  if (!abortFlag) {
    tok token;
    
    exists = true;
    if (a_getTok(token) && // for
        !check(token, token_type::type_keyword, key_type::key_for)) {
      exists = false;
      scanner.undo();
      //std::string errMsg = "Missing '(' from <procedure_call>";
      //reportError(token, err_type::error, errMsg);
    }
    
    if (exists) {
      if (a_getTok(token) && // (
          !check(token, token_type::type_symb, symb_type::symb_op_paren)) {
        std::string errMsg = "Missing '(' from <loop_statement>";
        reportError(token, err_type::error, errMsg);
        scanner.undo();
      }
      
      bool reqExists = false;
      p_assignment_statement(reqExists, scope);
      if (!reqExists) {
        std::string errMsg = "Missing assignment statement from <loop_statement>";
        reportError(token, err_type::error, errMsg);
      }
      
      if (a_getTok(token) && // ;
          !check(token, token_type::type_symb, symb_type::symb_semicolon)) {
        std::string errMsg = "Missing ';' from <loop_statement>";
        reportError(token, err_type::error, errMsg);
        scanner.undo();
      }
      
      p_expression(reqExists, scope);
      if (!reqExists) {
        std::string errMsg = "Missing <expression> from <loop_statement>";
        reportError(token, err_type::error, errMsg);
      }
      
      if (a_getTok(token) && // )
          !check(token, token_type::type_symb, symb_type::symb_cl_paren)) {
        std::string errMsg = "Missing ')' from <loop_statement>";
        reportError(token, err_type::error, errMsg);
        scanner.undo();
      }
      
      bool sExists = false;
      p_statement(sExists, scope); // first statement
      if (!sExists) {
        std::string errMsg = "Missing statements from <loop_statement>";
        reportError(token, err_type::error, errMsg);
      }
        
      if (sExists && a_getTok(token) && // ;
          !check(token, token_type::type_symb, symb_type::symb_semicolon)) {
        std::string errMsg = "Missing ';' from <loop_statement><statement>";
        reportError(token, err_type::error, errMsg);
        scanner.undo();
      }
      
      while (sExists) { // statement repeat
        p_statement(sExists, scope); 
        
        if (sExists && a_getTok(token) && // ;
            !check(token, token_type::type_symb, symb_type::symb_semicolon)) {
          std::string errMsg = "Missing ';' from <loop_statement><statement>";
          reportError(token, err_type::error, errMsg);
          scanner.undo();
        }
      }
      
      if (a_getTok(token) && // end
          !check(token, token_type::type_keyword, key_type::key_end)) {
        std::string errMsg = "Missing 'end' from <loop_statement>";
        reportError(token, err_type::error, errMsg);
        eat_misspelling(token);
      }
      
      if (a_getTok(token) && // for
          !check(token, token_type::type_keyword, key_type::key_for)) {
        std::string errMsg = "Missing 'for' from <loop_statement><end>";
        reportError(token, err_type::error, errMsg);
        //eat_misspelling(token); // statements after end for can start with an identifier
        scanner.undo();
      }
    }
  }
}

void p_return_statement(bool &exists) {
  if (!abortFlag) {
    tok token;
    
    exists = true;
    if (a_getTok(token) && // return
        !check(token, token_type::type_keyword, key_type::key_return)) {
      exists = false;
      scanner.undo();
    }
  } else {
    exists = false;
  }
}

std::string p_identifier(bool &exists) {
  if (!abortFlag) {
    tok token;
    
    exists = false;
    if (a_getTok(token)) { // identifier
      if (token.tokenType == token_type::type_id) {
        exists = true;
      } else {
        scanner.undo();
      }
    } 
    if (exists) {
      print_dbg_message("is identifier\n");
      return token.name;
    }
  } else {
    exists = false;
  }
  return "";
}

// EXPRESSION TYPE CONVERSION
//   int -> float (to achive float vs float equiv in arithmetic operations only)
//   int -> bool (to achive bool vs bool equiv in relational operations only)
factor p_expression(bool &exists, symbol_table &scope) {
  factor out;  out.objType = obj_type::obj_none;
  if (!abortFlag) {
    tok token;
    
    bool notFlag = false;
    if (a_getTok(token)) { // opt: not
      if (check(token, token_type::type_keyword, key_type::key_not)) {
        notFlag = true;
      } else {
        scanner.undo();
      }
    }
    
          // typecheck not can be with int, bool only TODO
    
    out = p_arithOp(exists, scope);
    
    if (exists) {
      bool check = false;
      factor right = p_expression_pr(check, scope, out);
      if (check) {
        out = right;
      }
    }
  } else {
    exists = false;
  }
  return out;
}

factor p_expression_pr(bool &exists, symbol_table &scope, factor left) {
  factor right; right.objType = obj_type::obj_none;
  if (!abortFlag) {
    tok token;
    
    op_type opType = op_type::op_none;
    if (a_getTok(token)) { // & or | and if neither exist backtrack
      if (check(token, token_type::type_symb, symb_type::symb_amper)) {
        opType = op_type::op_amper; // handle ampersand
      } else if (check(token, token_type::type_symb, symb_type::symb_straight)) {
        opType = op_type::op_straight; // handle straight
      } else { // if it is a symbol [not )], than assume expression (with undef symb) and eat
        scanner.undo();
        exists = false;
        /*if (!token.tokenType == token_type::type_symb ||
            check(token, token_type::type_symb, symb_type::symb_op_paren)) {
          scanner.undo();
        } else {
          std::string errMsg = "Illegal symbol in <expression>";
          reportError(token, err_type::error, errMsg);
          p_arithOp(exists);
          p_expression_pr(exists);
        }
        */
      }
      
          // typecheck | & can be with int, bool only
      if (opType != op_type::op_none) {
        right = p_arithOp(exists, scope);
        
        if (exists) {
          // typecheck TODO
          
          bool check = false;
          factor tmpRight = p_expression_pr(check, scope, right);
          if (check) {
            right = tmpRight;
          }
        } else {
          std::string errMsg = "Missing arithOp after <expression>";
          reportError(token, err_type::error, errMsg);
        }
      }
    }
  } else {
    exists = false;
  }
  return right;
}

factor p_arithOp(bool &exists, symbol_table &scope) {
  factor out; out.objType = obj_type::obj_none;
  if (!abortFlag) {
    out = p_relation(exists, scope);
    
    if (exists) {
      bool check = false;
      factor right = p_arithOp_pr(check, scope, out);
      if (check) {
        out = right;
      }
    }
  } else {
    exists = false;
  }
  return out;
}

factor p_arithOp_pr(bool &exists, symbol_table &scope, factor left) {
  factor right; right.objType = obj_type::obj_none;
  if (!abortFlag) {
    tok token;
    
    op_type opType = op_type::op_none;
    if (a_getTok(token)) { // + or - and if neither exist backtrack
      if (check(token, token_type::type_symb, symb_type::symb_plus)) {
        opType = op_type::op_plus; // handle plus
      } else if (check(token, token_type::type_symb, symb_type::symb_minus)) {
        opType = op_type::op_minus; // handle minus
      } else {
        scanner.undo();
        exists = false;
      }
      
      // typecheck + - can be with int, float only
      if (opType != op_type::op_none) {
        right = p_relation(exists, scope);
        
        if (exists) {
          // typecheck TODO
          
          bool check = false;
          factor tmpRight = p_arithOp_pr(check, scope, right);
          if (check) {
            right = tmpRight;
          }
        } else {
          std::string errMsg = "Missing relation after <arithOp>";
          reportError(token, err_type::error, errMsg);
        }
      }
    }
  } else {
    exists = false;
  }
  return right;
}

factor p_relation(bool &exists, symbol_table &scope) {
  factor out; out.objType = obj_type::obj_none;
  if (!abortFlag) {
    out = p_term(exists, scope);
    
    if (exists) {
      bool check = false;
      factor right = p_relation_pr(check, scope, out);
      if (check) {
        out = right;
      }
    }
  } else {
    exists = false;
  }
  return out;
}

factor p_relation_pr(bool &exists, symbol_table &scope, factor left) {
  factor right; right.objType = obj_type::obj_none;
  if (!abortFlag) {
    tok token;
    
    op_type opType = op_type::op_none;
    if (a_getTok(token)) { // <|>=|<=|>|==|!= and if none exist backtrack
          // typecheck all relationals return bool and
          //   can be with int, float, bool, char only
      if (check(token, token_type::type_symb, symb_type::symb_smaller)) {
        opType = op_type::op_smaller; // handle smaller than
      } else if (check(token, token_type::type_symb, symb_type::symb_greater_eq)) {
        opType = op_type::op_greater_eq; // handle greater than or equals
      } else if (check(token, token_type::type_symb, symb_type::symb_smaller_eq)) {
        opType = op_type::op_smaller_eq; // handle smaller than or equals
      } else if (check(token, token_type::type_symb, symb_type::symb_greater)) {
        opType = op_type::op_greater; // handle grater than
      } else if (check(token, token_type::type_symb, symb_type::symb_equals)) {
        opType = op_type::op_equals; // handle equals
      } else if (check(token, token_type::type_symb, symb_type::symb_not_equals)) {
        opType = op_type::op_not_equals; // handle not equals
      } else if (check(token, token_type::type_symb, symb_type::symb_assign) ||
                 token.tokenType == token_type::type_illegal) {
        // handle illegal
        std::string errMsg = "Illegal symb in <expression>";
        reportError(token, err_type::error, errMsg);
      } else {
        scanner.undo();
        exists = false;
      }
      
      if (opType != op_type::op_none) {
        right = p_term(exists, scope);
        
        if (exists) {
          // typecheck TODO
          
          bool check = false;
          factor tmpRight = p_relation_pr(check, scope, right);
          if (check) {
            right = tmpRight;
          }
        } else {
          std::string errMsg = "Missing term after <relation>";
          reportError(token, err_type::error, errMsg);
        }
      }
    }
  } else {
    exists = false;
  }
  return right;
}

factor p_term(bool &exists, symbol_table &scope) {
  factor out; out.objType = obj_type::obj_none;
  if (!abortFlag) {
    out = p_factor(exists, scope);
    
    if (exists) {
      bool check = false;
      factor right = p_term_pr(check, scope, out);
      if (check) {
        out = right;
      }
    }
  } else {
    exists = false;
  }
  return out;
}

factor p_term_pr(bool &exists, symbol_table &scope, factor left) {
  factor right; right.objType = obj_type::obj_none;
  if (!abortFlag) {
    tok token;
    
    op_type opType = op_type::op_none;
    if (a_getTok(token)) { // * or / and if neither exist backtrack
      if (check(token, token_type::type_symb, symb_type::symb_multi)) {
        opType = op_type::op_multi; // handle multiply
      } else if (check(token, token_type::type_symb, symb_type::symb_div)) {
        opType = op_type::op_div; // handle divide
      } else {
        scanner.undo();
        exists = false;
      }
      
      // typecheck * and / can be with int, float only
      if (opType != op_type::op_none) {
        right = p_factor(exists, scope); 
        
        if (exists) {
          obj_type lType, rType;
          checkArrayLength(lType, rType, left, right, scope, token);
          
          // TODO codegen
          bool suc = true, floatFlag = false;
          if (lType == obj_type::obj_integer && rType == obj_type::obj_integer) {
            if (opType != op_type::op_multi) {
            } else { // div
            }
            
          } else if (lType == obj_type::obj_integer && rType == obj_type::obj_float) {
            floatFlag = true;
            if (opType != op_type::op_multi) {
            } else { // div
            }
            
          } else if (lType == obj_type::obj_float && rType == obj_type::obj_integer) {
            floatFlag = true;
            if (opType != op_type::op_multi) {
            } else { // div
            }
            
          } else if (lType == obj_type::obj_float && rType == obj_type::obj_float) {
            floatFlag = true;
            if (opType != op_type::op_multi) {
            } else { // div
            }
            
          } else {
            std::string errMsg = "Incorrect types for multiply <term>";
            reportError(token, err_type::error, errMsg);
            suc = false;
          }
          
          // if only one is an id, return id as the main object
          // if both are ids return the array
          // if both are ids and neither or both are arrays, return the right id
          // if neither are ids, return the right one
          // TODO try to compress this into a fuction so that it can be reused?
          if (suc) { // if previously unsuccessful, don't do anything
            if (left.objType == obj_type::obj_id && right.objType != obj_type::obj_id) {
              right = left;
            } else if (left.objType == obj_type::obj_id && right.objType == obj_type::obj_id) {
              object* lObj,* rObj;
              lObj = (object*) findObj(left.obj.name, scope, token);
              rObj = (object*) findObj(right.obj.name, scope, token);
              if (lObj != NULL && rObj != NULL && 
                  isArrayObj(lObj) && left.obj.idx == false && 
                  !(isArrayObj(rObj) && right.obj.idx == false)) {
                right = left;
              }
            }
            
            if (floatFlag) {
              if (right.objType == obj_type::obj_id) {
                right.obj.varType = obj_type::obj_float;
              } else {
                right.objType = obj_type::obj_float;
              }
            }
          }
          
          bool check = false;
          factor tmpRight = p_term_pr(check, scope, right);
          if (check) {
            right = tmpRight;
          }
        } else {
          std::string errMsg = "Missing factor after <term>";
          reportError(token, err_type::error, errMsg);
        }
      }
    }
  } else {
    exists = false;
  }
  return right;
}

factor p_factor(bool &exists, symbol_table &scope) {
  factor out; out.objType = obj_type::obj_none;
  if (!abortFlag) {
    tok token;
    
    exists = false;
    if (a_getTok(token)) { // ( if not backtrack
      if (check(token, token_type::type_symb, symb_type::symb_op_paren)) {
        exists = true; // handle expression
      } else {
        scanner.undo();
      }
    }
    
    if (exists) {
      bool reqExists = false;
      out = p_expression(reqExists, scope); // name already handles id scopes
      if (!reqExists) {
        std::string errMsg = "Missing <expression> from <factor><expression>";
        reportError(token, err_type::error, errMsg);
      }
      
      if (a_getTok(token) && // )
          !check(token, token_type::type_symb, symb_type::symb_cl_paren)) {
        std::string errMsg = "Missing ')' from <factor><expression>";
        reportError(token, err_type::error, errMsg);
        scanner.undo();
        out.objType = obj_type::obj_none;
      }
      
      // handling expression, return expression except if there is no ) then void it
    } else {
      bool neg = false;
      if (a_getTok(token)) { // - if not backtrack
        if (check(token, token_type::type_symb, symb_type::symb_minus)) {
          neg = true; // handle negative
        } else {
          scanner.undo();
        }
      }
      
      out = p_name(exists, scope); // name handles id scopes
      if (!exists) {
        out = p_number(exists);
      }
      if (!exists) {
        out = p_string(exists);
        if (exists && neg) {
          std::string errMsg = "Invalid - for string from <factor>";
          reportError(token, err_type::error, errMsg);
          out.objType = obj_type::obj_none;
        }
      }
      if (!exists) {
        out = p_char(exists);
        if (exists && neg) {
          std::string errMsg = "Invalid - for char from <factor>";
          reportError(token, err_type::error, errMsg);
          out.objType = obj_type::obj_none;
        }
      }
      if (!exists && a_getTok(token)) { // true or false and if neither exist backtrack
        if (check(token, token_type::type_keyword, key_type::key_true)) {
          exists = true;
          if (neg) {
            std::string errMsg = "Invalid - for true from <factor>";
            reportError(token, err_type::error, errMsg);
          } else {
            out.objType = obj_type::obj_bool; // handle true
            out.b = true;
            print_dbg_message("is bool\n");
          }
        } else if (check(token, token_type::type_keyword, key_type::key_false)) {
          exists = true;
          if (neg) {
            std::string errMsg = "Invalid - for false from <factor>";
            reportError(token, err_type::error, errMsg);
          } else {
            out.objType = obj_type::obj_bool; // handle false
            out.b = false;
            print_dbg_message("is bool\n");
          }
        } else {
          // handle alone neg, not true or false and none of the above, but still have neg
          if (neg) {
            std::string errMsg = "Orphan '-' in <factor>";
            reportError(token, err_type::error, errMsg);
          }
          scanner.undo();
        }
      }
    }
  } else {
    exists = false;
  }
  return out;
}

factor p_name(bool &exists, symbol_table &scope) {
  factor outF; outF.objType = obj_type::obj_none;
  if (!abortFlag) {
    tok token; nameObj out;
    
    out.name = p_identifier(exists);
    
    if (exists) {
      print_dbg_message("is name\n");
      // check to see if it's in scope
      symbol* tmp; object* obj;
      tmp = findObj(out.name, scope, token);
      
      // make sure it is an object
      if (tmp != NULL && (tmp->tblType == tbl_type::tbl_obj || tmp->tblType == tbl_type::tbl_param)) {
        obj = (object*) tmp;
        out.varType = obj->objType; // save obj type into factor nameObj (so we don't have to look it up in the future)
      } else {
        std::string errMsg = "Symbol called is not an object <name>";
        reportError(token, err_type::error, errMsg);
        out.name = "";
      }
    
      out.idx = false;
      if (a_getTok(token)) { // optional [
        if (check(token, token_type::type_symb, symb_type::symb_op_bracket)) {
          out.idx = true;
        } else {
          scanner.undo();
        }
      }
      
      if (out.idx) {
        factor expF = p_expression(exists, scope);
        if (!exists) {
          std::string errMsg = "Missing index <expression> from <name>";
          reportError(token, err_type::error, errMsg);
          out.name = "";
        }
        
        // check to see that expression is an int within lb and ub
        if (exists && obj != NULL) {
          print_dbg_message("in line 1750");
          if (obj->lb == obj->ub) { // obj is not an array so throw error
            std::string errMsg = "Cannot index into non-array object <name>";
            reportError(token, err_type::error, errMsg);
            out.name = "";
          } else {
            if (expF.objType == obj_type::obj_integer) { // retF is integer
              // do int bounds check
              if (expF.i >= obj->lb && expF.i <= obj->ub) {
                out.boundsIntPos = expF.i;
              } else {
                std::string errMsg = "Out of bounds error <name>";
                reportError(token, err_type::error, errMsg);
                out.name = "";
              }
            } else {
              if (expF.objType == obj_type::obj_id && 
                  expF.obj.varType == obj_type::obj_integer) {
                // if an int object, check to see that it's either not an array, 
                //   or an index into an array TODO
                // THIS SEEMS LIKE A RUNTIME THING
                //   IF THE EXPRESSION IS A VARIABLE SET BY getInteger(), 
                //   WE CANNOT DO THIS DURING COMPILE TIME
                object* expO = (object*) findObj(expF.obj.name, scope, token);
                if (expO != NULL) {
                  if (expO->lb != expO->ub && !expF.obj.idx) {
                    std::string errMsg = "Index cannot be an array <name>";
                    reportError(token, err_type::error, errMsg);
                    out.name = "";
                  } else {
                    out.boundsIntPos = expF.i;
                    out.boundsPos = expF.obj.name;
                  }
                }
              } else {
                std::string errMsg = "Index must resolve into an integer <name>";
                reportError(token, err_type::error, errMsg);
                out.name = "";
              }
            }
          }
        }
        
        if (a_getTok(token) && // ]
            !check(token, token_type::type_symb, symb_type::symb_cl_bracket)) {
          std::string errMsg = "Missing ']' from <name>";
          reportError(token, err_type::error, errMsg);
          scanner.undo();
          out.name = "";
        }
        exists = true;
      }
      
      if (out.name.size() > 0) {
        outF.objType = obj_type::obj_id;
        outF.obj = out;
      }
    }
  } else {
    exists = false;
  }
  return outF;
}

// IF proc IS NULL, DON'T CHECK IF ARGUMENTS ARE CORRECT
void p_argument_list(bool &exists, symbol_table &scope, procedure* proc, int i) {
  if (!abortFlag) {
    tok token;
    
    // TODO typecheck
    bool prevArgs = exists;
    factor expF = p_expression(exists, scope);
    
    if (exists) {
      // check that expF can fit into procedure proc param at slot i
    
      tok lookahead; peekTok(lookahead); // ,
      if (check(lookahead, token_type::type_symb, symb_type::symb_comma)) {
        getTok(token);
        p_argument_list(exists, scope, proc, i + 1);
      } else {
        p_expression(exists, scope);
        if (exists) {
          std::string errMsg = "Missing ',' from <argument_list>";
          reportError(token, err_type::error, errMsg);
        } else {
          exists = true;
        }
      }
    }
    
    if (prevArgs && !exists) {
      std::string errMsg = "Orphan ',' from <argument_list>";
      reportError(token, err_type::error, errMsg);
      exists = true;
    }
  }
}

factor p_number(bool &exists) {
  factor out; out.objType = obj_type::obj_none;
  if (!abortFlag) {
    tok token;
    
    exists = true;
    if (getTok(token)) {
      if (token.tokenType == token_type::type_int) {
        out.objType = obj_type::obj_integer; // handle int
        out.i = token.i;
        print_dbg_message("is int\n");
      } else if (token.tokenType == token_type::type_float) {
        out.objType = obj_type::obj_float; // handle float
        out.f = token.f;
        print_dbg_message("is float\n");
      } else { // number does not exist
        exists = false;
        scanner.undo();
      }
    }
  } else {
    exists = false;
  }
  return out;
}

factor p_string(bool &exists) {
  factor out; out.objType = obj_type::obj_none;
  if (!abortFlag) {
    tok token;
    
    exists = true;
    if (a_getTok(token) && // string
        !(token.tokenType == token_type::type_string)) {
      exists = false;
      scanner.undo();
    }
    if (exists) {
      out.objType = obj_type::obj_string;
      out.s = token.name;
      print_dbg_message("is string\n");
    }
  } else {
    exists = false;
  }
  return out;
}

factor p_char(bool &exists) {
  factor out; out.objType = obj_type::obj_none;
  if (!abortFlag) {
    tok token;
    
    exists = true;
    if (a_getTok(token) && // char
        !(token.tokenType == token_type::type_char)) {
      exists = false;
      scanner.undo();
    }
    if (exists) {
      out.objType = obj_type::obj_char;
      out.c = token.c;
      print_dbg_message("is char\n");
    }
  } else {
    exists = false;
  }
  return out;
}

void printSymbolTable(std::string name, symbol_table &tbl) {
  for (symbol_table::iterator it = tbl.begin(); it != tbl.end(); ++it) {
    if (name.compare(it->first) != 0) {
      std::cout << it->first << ": " << tblTypeToString(it->second->tblType);
      
      if (it->second->tblType == tbl_type::tbl_proc) {
        procedure* proc = (procedure*) it->second;
        std::cout << " " << proc->local.scope.size();
        std::cout << " Begin proc ------------\n";
        printSymbolTable(it->first, proc->local.scope);
      } else {
        if (it->second->tblType == tbl_type::tbl_param) {
          param* x = (param*) it->second;
          std::cout << " " << paramTypeToString(x->paramType);
          std::cout << " " << x->order;
        }
        object* x = (object*) it->second;
        std::cout << "\n  " << " " << objTypeToString(x->objType) << " [" << x->lb << ":" 
                  << x->ub << "]" << "\n";
      }
    } else {
      std::cout << it->first << ": self\n";
    }
  }
  std::cout << "------------\n";
}

//====================== Main ======================//
int main(int argc, const char *argv[]) {
  if (argc == 2) {
    std::cout << "Compiler start " << argv[1] << "\n";
    scanner.init(argv[1], &errList);
    
    populateGlobalScope();
    
    if (!SCANNER_ONLY) {
      p_program();
      print_errors();
      
      // for recursiveFib.src
      /* recursivefib
        global integer x;
        integer i;
        integer max;
      */
      
      // for vectorOps.src
      /* vectorops
        integer indx;
        integer size;
        char x;
        float vect1[-5:5];
        float vect2[-5:5];
        float vect3[-2:8];
        float vect4[0:10];
        float vect5[10:20];
      */
      
      printSymbolTable("global", globalScope);
      
    } else {
      tok token;
    
      while (getTok(token)) {
        printTok(token);
      }
    }
    
    scanner.deinit();
  } else {
    std::cout << "Too many or too little args!";
  }

  return 0;
}
