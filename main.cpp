// MAIN.CPP for compiler

#include "lexer.h"
#include "parserF.h"

#include <stack>

#define DEBUG_FLAG false
#define SCANNER_ONLY false

//====================== Globals ======================//
// symbol tables
symbol_table globalTable;
// error list
error_list errList;
// error flags (parse error list and if there is a large error then disable
bool genCode = true;
bool abortFlag = false;
// lexer
lexer scanner;

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
  globalTable.insert(elem);
  
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
    tok token; symbol_table::iterator program = globalTable.find(progN);
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
            globalTable.insert(elem);
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
      p_statement(exists); 
      
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
      } else if (globalTable.count(procN) != 0) {
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
      p_parameter_list(hasParams, suc, *proc); // params are automatically added
      
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

void p_parameter_list(bool &exists, bool &suc, procedure &proc) {
  if (!abortFlag) {
    tok token;
    
    bool prevParams = exists;
    p_parameter(exists, proc);
    
    if (exists) {
      tok lookahead; peekTok(lookahead); // ,
      if (check(lookahead, token_type::type_symb, symb_type::symb_comma)) {
        getTok(token);
        p_parameter_list(exists, suc, proc);
      } else {
        p_parameter(exists, proc);
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

void p_parameter(bool &exists, procedure &proc) {
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
            //   are considered for globalTable
            //globalTable.insert(elem); 
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
      p_statement(exists); 
      
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
      } else if (globalTable.count(varN) != 0) {
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

void p_statement(bool &exists) {
  if (!abortFlag) {
    tok token;
    
    p_assignment_statement(exists);
    if (!exists) {
      p_if_statement(exists);
    }
    if (!exists) {
      p_loop_statement(exists);
    }
    if (!exists) {
      p_return_statement(exists);
    }
    if (!exists) {
      p_procedure_call(exists);
    } // if exists is still false, a statement does not exist
    
    if (!exists) { // STATEMENTS ALWAYS END IN EITHER 'ELSE' OR 'END'
      // so if there was no statement, the next token has to be 'else' or 'end'
      getTok(token); // consume all invalid tokens until next statement (or end of statements) 
      if (!check(token, token_type::type_keyword, key_type::key_end)
          && !check(token, token_type::type_keyword, key_type::key_else)) {
        // unknown token in declaration
        std::string errMsg = "Incorrect start token in <statement>";
        reportError(token, err_type::error, errMsg);
        p_statement(exists);
      } else {
        scanner.undo(); // if we find a statement, undo the consume
      }
    }
  }
}

void p_procedure_call(bool &exists) {
  if (!abortFlag) {
    tok token;
    
    p_identifier(exists);
    
    if (exists) {
      if (a_getTok(token) && // (
          !check(token, token_type::type_symb, symb_type::symb_op_paren)) {
        std::string errMsg = "Missing '(' from <procedure_call>";
        reportError(token, err_type::error, errMsg);
        scanner.undo();
      }
      
      bool hasArgs = false;
      p_argument_list(hasArgs); 
      if (hasArgs) {
        // add arg list to call
      }
      
      if (a_getTok(token) && // )
          !check(token, token_type::type_symb, symb_type::symb_cl_paren)) {
        std::string errMsg = "Missing ')' from <procedure_call>";
        reportError(token, err_type::error, errMsg);
        scanner.undo();
      }
    }
  }
}

void p_assignment_statement(bool &exists) {
  if (!abortFlag) {
    tok token;
    
    p_destination(exists);
    
    if (exists) {
      tok lookahead; peekTok(lookahead); // check = or :=
      if (check(lookahead, token_type::type_symb, symb_type::symb_assign)) {
        getTok(token);
      } else if (check(lookahead, token_type::type_illegal, ill_type::ill_equals) ||
                 check(lookahead, token_type::type_symb, symb_type::symb_equals)) {
        std::string errMsg = "Malformed ':=' from <assignment_statement>";
        getTok(token);
        reportError(token, err_type::error, errMsg);
      } else {
        exists = false;
        scanner.undo();
      }
    }
    
    if (exists) {
      p_expression(exists);
      if (!exists) {
        std::string errMsg = "Missing <expression> from <assignment_statement>";
        reportError(token, err_type::error, errMsg);
      }
    }
  }
}

void p_destination(bool &exists) {
  if (!abortFlag) {
    tok token;
    
    p_identifier(exists);
    
    if (exists) {
      bool array = false;
      if (a_getTok(token)) { // optional [
        if (check(token, token_type::type_symb, symb_type::symb_op_bracket)) {
          array = true;
        } else {
          scanner.undo();
        }
      }
      
      if (array) {
        p_expression(exists);
        if (!exists) {
          std::string errMsg = "Missing <expression> from <destination>";
          reportError(token, err_type::error, errMsg);
        }
        
        if (a_getTok(token) && // ]
            !check(token, token_type::type_symb, symb_type::symb_cl_bracket)) {
          std::string errMsg = "Missing ']' from <destination>";
          reportError(token, err_type::error, errMsg);
          scanner.undo();
        }
      }
    }
  }
}

void p_if_statement(bool &exists) {
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
      p_expression(reqExists);
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
      p_statement(sExists); // first statement
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
        p_statement(sExists); 
        
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
        p_statement(sExists); // first statement
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
          p_statement(sExists); 
          
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

void p_loop_statement(bool &exists) {
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
      p_assignment_statement(reqExists);
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
      
      p_expression(reqExists);
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
      p_statement(sExists); // first statement
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
        p_statement(sExists); 
        
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

void p_expression(bool &exists) {
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
    
    p_arithOp(exists);
    p_expression_pr(exists);
  }
}

void p_expression_pr(bool &exists) {
  if (!abortFlag) {
    tok token;
    
    if (a_getTok(token)) { // & or | and if neither exist backtrack
      if (check(token, token_type::type_symb, symb_type::symb_amper)) {
        // handle ampersand
        p_arithOp(exists);
        p_expression_pr(exists);
      } else if (check(token, token_type::type_symb, symb_type::symb_straight)) {
        // handle straight
        p_arithOp(exists);
        p_expression_pr(exists);
      } else { // if it is a symbol [not )], than assume expression (with undef symb) and eat
        scanner.undo();
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
    }
  }
}

void p_arithOp(bool &exists) {
  if (!abortFlag) {
   
    
    p_relation(exists);
    p_arithOp_pr(exists);
  }
}

void p_arithOp_pr(bool &exists) {
  if (!abortFlag) {
    tok token;
    
    if (a_getTok(token)) { // + or - and if neither exist backtrack
      if (check(token, token_type::type_symb, symb_type::symb_plus)) {
        // handle plus
        p_relation(exists);
        p_arithOp_pr(exists);
      } else if (check(token, token_type::type_symb, symb_type::symb_minus)) {
        // handle minus
        p_relation(exists);
        p_arithOp_pr(exists);
      } else {
        scanner.undo();
      }
    }
  }
}

void p_relation(bool &exists) {
  if (!abortFlag) {
   
    
    p_term(exists);
    p_relation_pr(exists);
  }
}

void p_relation_pr(bool &exists) {
  if (!abortFlag) {
    tok token;
    
    if (a_getTok(token)) { // <|>=|<=|>|==|!= and if none exist backtrack
      if (check(token, token_type::type_symb, symb_type::symb_smaller)) {
        // handle smaller than
        p_term(exists);
        p_relation_pr(exists);
      } else if (check(token, token_type::type_symb, symb_type::symb_greater_eq)) {
        // handle greater than or equals
        p_term(exists);
        p_relation_pr(exists);
      } else if (check(token, token_type::type_symb, symb_type::symb_smaller_eq)) {
        // handle smaller than or equals
        p_term(exists);
        p_relation_pr(exists);
      } else if (check(token, token_type::type_symb, symb_type::symb_greater)) {
        // handle grater than
        p_term(exists);
        p_relation_pr(exists);
      } else if (check(token, token_type::type_symb, symb_type::symb_equals)) {
        // handle equals
        p_term(exists);
        p_relation_pr(exists);
      } else if (check(token, token_type::type_symb, symb_type::symb_not_equals)) {
        // handle not equals
        p_term(exists);
        p_relation_pr(exists);
      } else if (check(token, token_type::type_symb, symb_type::symb_assign) ||
                 token.tokenType == token_type::type_illegal) {
        // handle illegal
        std::string errMsg = "Illegal symb in <expression>";
        reportError(token, err_type::error, errMsg);
        p_term(exists);
        p_relation_pr(exists);
      } else {
        scanner.undo();
      }
    }
  }
}

void p_term(bool &exists) {
  if (!abortFlag) {
   
    
    p_factor(exists);
    p_term_pr(exists);
  }
}

void p_term_pr(bool &exists) {
  if (!abortFlag) {
    tok token;
    
    if (a_getTok(token)) { // * or / and if neither exist backtrack
      if (check(token, token_type::type_symb, symb_type::symb_multi)) {
        // handle multiply
        p_factor(exists);
        p_term_pr(exists);
      } else if (check(token, token_type::type_symb, symb_type::symb_div)) {
        // handle divide
        p_factor(exists);
        p_term_pr(exists);
      } else {
        scanner.undo();
      }
    }
  }
}

void p_factor(bool &exists) {
  if (!abortFlag) {
    tok token;
    
    exists = false;
    if (a_getTok(token)) { // ( if not backtrack
      if (check(token, token_type::type_symb, symb_type::symb_op_paren)) {
        // handle expression
        exists = true;
      } else {
        scanner.undo();
      }
    }
    
    if (exists) {
      bool reqExists = false;
      p_expression(reqExists);
      if (!reqExists) {
        std::string errMsg = "Missing <expression> from <factor><expression>";
        reportError(token, err_type::error, errMsg);
      }
      
      if (a_getTok(token) && // )
          !check(token, token_type::type_symb, symb_type::symb_cl_paren)) {
        std::string errMsg = "Missing ')' from <factor><expression>";
        reportError(token, err_type::error, errMsg);
        scanner.undo();
      }
    } else {
      bool neg = false;
      if (a_getTok(token)) { // - if not backtrack
        if (check(token, token_type::type_symb, symb_type::symb_op_paren)) {
          // handle negative
          neg = true;
        } else {
          scanner.undo();
        }
      }
      
      p_name(exists);
      if (!exists) {
        p_number(exists);
      }
      if (!exists) {
        p_string(exists);
        if (exists && neg) {
          std::string errMsg = "Invalid - for string from <factor>";
          reportError(token, err_type::error, errMsg);
        }
      }
      if (!exists) {
        p_char(exists);
        if (exists && neg) {
          std::string errMsg = "Invalid - for char from <factor>";
          reportError(token, err_type::error, errMsg);
        }
      }
      if (!exists) {
        if (a_getTok(token)) { // true or false and if neither exist backtrack
          if (check(token, token_type::type_keyword, key_type::key_true)) {
            exists = true;
            if (neg) {
              std::string errMsg = "Invalid - for true from <factor>";
              reportError(token, err_type::error, errMsg);
            } else {
              // handle true
            }
          } else if (check(token, token_type::type_keyword, key_type::key_false)) {
            exists = true;
            if (neg) {
              std::string errMsg = "Invalid - for false from <factor>";
              reportError(token, err_type::error, errMsg);
            } else {
              // handle false
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
    }
  }
}

void p_name(bool &exists) {
 
  p_destination(exists);
}

void p_argument_list(bool &exists) {
  if (!abortFlag) {
    tok token;
    
    bool prevArgs = exists;
    p_expression(exists);
    
    if (exists) {
      tok lookahead; peekTok(lookahead); // ,
      if (check(lookahead, token_type::type_symb, symb_type::symb_comma)) {
        getTok(token);
        p_argument_list(exists);
      } else {
        p_expression(exists);
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
  factor out;
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

void p_string(bool &exists) {
  if (!abortFlag) {
    tok token;
    
    exists = true;
    if (a_getTok(token) && // string
        !(token.tokenType == token_type::type_string)) {
      exists = false;
      scanner.undo();
    }
    if (exists) {
      print_dbg_message("is string\n");
    }
  }
}

void p_char(bool &exists) {
  if (!abortFlag) {
    tok token;
    
    exists = true;
    if (a_getTok(token) && // char
        !(token.tokenType == token_type::type_char)) {
      exists = false;
      scanner.undo();
    }
    if (exists) {
      print_dbg_message("is char\n");
    }
  }
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
        }
        object* x = (object*) it->second;
        std::cout << "\n  " << " " << objTypeToString(x->objType) << " [" << x->lb << ":" 
                  << x->ub << "]" << "\n";
      }
    } else {
      std::cout << it->first << ": \n";
    }
  }
  std::cout << "------------\n";
}

//====================== Main ======================//
int main(int argc, const char *argv[]) {
  if (argc == 2) {
    std::cout << "Compiler start " << argv[1] << "\n";
    scanner.init(argv[1], &errList);
   
    
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
      
      printSymbolTable("global", globalTable);
      
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
