#pragma once 


#include "CExpr.h"
#include "MatrixFunctions.h"
#include "CMatrixCommands.h"
#include "CDense.h"
#include "CSparse.h"
#include "CMatrix.h"
#include "CExceptions.h"
#include "define.h"

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>


//======================================================================
enum commands {
    PRINT = 0,
    SCAN,
    GEM,
    TRANSPOSE,
    READEXPR,
    EVALUATE,
    INVERSE,
    RANK,
    WRONG_COM,
    DETERMINANT,
    HELP,
    CLEAR,
    DELETE,
    EXIT
};
//----------------------------------------------------------------------
class CApplication {
  public:
  /**
   * run method\n
   *- catches all exceptions (errord) that can occur while running the program
   */
    void        Run         ();
  private:
    /** 
     * main method that reads and executes commands from user
     * @param[in] in: terminal input
     */
    void        Execute     (std::istream & in);
    /**
     * this function analyzes command
     * @param[in] in: input stream
     * @param[out] returns enum command
     */
    static int  GetCommand  (std::istream & in);
    /**
     * this method reads and executes single command
     * @param[in] is: input stream
     * @param[in] command: enum command
     * @param[in] str: rest of the command
     */
    void        SwitchCommand(std::istream & is, int command, 
                                   const std::string & str);
    /**
     * this method scans matrix with given name from user input or from file\n
     * allows user to rewrite already existing one
     */ 
    void        Scan        (std::istream & in);
    /**
     * this method reads matrix from file
     */
    void        ReadFromFile(const std::string & varName, const std::string & fileName);
    ///this method prints out matrix or evaluated expression with given name
    void        Print       (std::istream & in);

    void        WriteToFile 
    (const std::string & varName, const std::string & fileName) ; 

    /// deletes expression with given name from memory
    void        Delete      (std::istream & in);
    /**
        reads expression from input, parses it to tokens and turn it to RP notation\n
        if expression can be evaluated, evaluates it and keeps in memory as evaluated matrix\n
        if not, keeps it as non-evaluated expression with variables\n
        recursive expressions are not allowed
    */
    void        ReadExpr    (std::istream & in);
    /**
     * evaluates given expression and prints its result out\n
     * does NOT change original expression
    */
    void        Evaluate    (std::istream & in);
    /**
        method that prints out matrix (or evaluated expression) of given name in row echelon form
        does NOT change it\n
    */  
    void        Gem         (std::istream & in);
    /**
     *  transposes matrix (or evaluated expression) of given name
     *  does NOT change it
     */
    void        Transpose   (std::istream & in);
    /**
     *  prints out rank of matrix (or evaluated expression) of given name\n
     */
    void        Rank        (std::istream & in);
    /**
     *  prints out inverses matrix (or evaluated expression) of given name\n
     *  unless it's singular
     */
    void        Inverse     (std::istream & in);
    /**
     *  finds determinant of matrix (or evaluated expression) of given name
     */
    void        Determinant (std::istream & in);
    /// help command
    void        Help        ();
    /// functions that read variable name from input stream
    static std::string ReadVar     (std::istream & in);
    
    CMemory matrices;
};