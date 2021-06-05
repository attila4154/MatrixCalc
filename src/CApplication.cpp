#pragma once
#include "../lib/CApplication.h"


//=======================================================================
void CApplication::Execute (std::istream & in) {
    /* main method that reads and executes commands from user */
    std::string str; 
    do {
        std::cout << "> ";
        getline (in, str);
        if (in.eof()) throw exit_exc ();
    } while (str.empty() );

    std::istringstream is (str);
    int command = GetCommand(is);
    
    SwitchCommand (is, command, str);
}
//----------------------------------------------------------------------
int CApplication::GetCommand (std::istream & in) {
    std::string command;
    getline (in, command, ' ');
    if (command == "PRINT"     || command == "print")     return commands::PRINT;
    if (command == "GEM"       || command == "gem")       return commands::GEM;
    if (command == "SCAN"      || command == "scan")      return commands::SCAN;
    if (command == "EXIT"      || command == "exit")      return commands::EXIT;
    if (command == "TRANSPOSE" || command == "transpose") return commands::TRANSPOSE;
    if (command == "evaluate"  || command == "EVALUATE")  return commands::EVALUATE;
    if (command == "rank"      || command == "RANK")      return commands::RANK;
    if (command == "inverse"   || command == "INVERSE")   return commands::INVERSE;
    if (command == "help"      || command == "HELP")      return commands::HELP;
    if (command =="determinant"|| command == "DETERMINANT")   return commands::DETERMINANT;
    else {
        //if it's not any of the basic commands, try to read as expression:
        char temp;
        if (!(in >> temp) || temp != '=') {   
            throw wrong_command();
        } 
        in.putback (temp);
        in.putback (' ');
        in.putback ((command.data())[0]);
        return commands::READEXPR;
    }
}
//----------------------------------------------------------------------
void CApplication::SwitchCommand (std::istream & is, int command, const std::string & str) {
    switch (command) {
        case commands::PRINT : 
        {
            Print (is);
            break;
        }
        case commands::SCAN : 
        {
            Scan (is);
            break;
        }
        case commands::READEXPR : {
            ReadExpr (is);   
            break;
        }
        case commands::EVALUATE : {
            Evaluate (is);
            break;
        }
        case commands::GEM : 
        {
            Gem (is);
            break;
        }
        case commands::TRANSPOSE : {
            Transpose (is);
            break;
        }
        case commands::RANK : {
            Rank (is);
            break;
        }
        case INVERSE : {
            Inverse (is);
            break;
        }
        case DETERMINANT : {
            Determinant (is);
            break;
        }
        case HELP : {
            Help ();
            break;
        }   
        case commands::EXIT : 
            throw exit_exc();
        default:
            throw wrong_command(str);
    }
}
//----------------------------------------------------------------------
std::string CApplication::ReadVar (std::istream & in) {
    std::string var;
    if (!getline (in, var, ' '))
        throw WrongFormat ("could not read variable name\n");
    return var;
}
//----------------------------------------------------------------------
void CApplication::Print (std::istream & in) {
    /* function that prints out matrix or NOT evaluated expression in RP notation */
    std::string varName = ReadVar(in);
    if (matrices.find (varName) == matrices.end() ) throw variable_not_set (varName);
    else
        std::cout << *(matrices.find(varName)->second) << std::endl;
}
//----------------------------------------------------------------------
void CApplication::Scan (std::istream & in){
    /* function that scans matrix of given size from user input or from file
       allows to rewrite already existing matrix/expression               */
    
    std::string varName = ReadVar (in);
    std::string str;
    getline (in, str);
    if (str[0] == '-' && str[1] == 'f') {
        ReadFromFile (str, varName);
        return;
    }

    std::istringstream is (str);
    int m, n;
    ReadSize (is, m, n);
    std::shared_ptr<CMatrix> matrix;
    std::cout << "scanning:" << std::endl;
    ReadMatrix (std::cin, matrix, m, n);
    //  every matrix is stored as expression with 1 token:
    std::shared_ptr <CExpr> expr = std::make_shared<CExpr> (matrix);
    if (matrices.find (varName) != matrices.end() ) {
        std::cout << "variable " << varName << " was rewritten" << std::endl;
        matrices.erase (varName);
    }
    matrices.emplace (varName, expr);
}
//----------------------------------------------------------------------
void CApplication::ReadMatrix (std::istream & in, MPtr & matrix, int m, int n) {
    /* 
        function that reads matrix of given size and stores it as Dense or Sparse matrix 
        depending on how many 'empty characters' (0) it has                           
    */
    std::string buff;
    do {
        getline (in, buff);
    } while (buff.empty() );
    std::istringstream is (buff);

    if (ShouldBeDense (buff) ) {
        matrix = std::make_shared <CDense>  (m,n);
    } else 
        matrix = std::make_shared <CSparse> (m,n);

    if (!(is >> matrix)) throw WrongFormat();
    if (is.rdbuf()->in_avail() != 0) throw WrongFormat();
}
//----------------------------------------------------------------------
void CApplication::ReadFromFile(const std::string & in, std::string varName) {
    /* 
        reads matrix with no given size from file as Dense matrix
    */ 
    std::string fileName;
    std::istringstream is (in);
    is >> fileName; //skipping '-f' characters
    if (!(is >> fileName) ) throw WrongFormat ("could not read file name\n");
    if (!is.eof()) throw WrongFormat ("too many arguments\n");
    std::fstream f (fileName);
    if (!f.is_open()) {
        std::cout << "file " << fileName << " does not exist or can not be opened\n";
        return;
    }
    // MPtr matrix = std::make_shared<CDense> ();
    float value;
    int m = 0, n = 0;
    std::cout << "read matrix is " << std::endl;
    
    while (f >> value ) {
        std::cout << value;
        if (!m) n++;
        if (f.peek() == '\n') {
            std::cout << std::endl;
            m++;
        }
    }
    std::cout << "dimensions are " << m << 'x' << n << std::endl;
     
    f.close();
}
//----------------------------------------------------------------------
void CApplication::ReadExpr(std::istream & in) {
    /*
        *reads expression from input, parses it to tokens and turn it to RP notation
        *if expression can be evaluated, evaluates it and keeps in memory as evaluated matrix
        *if not, keeps it as non-evaluated expression 
    */
    std::shared_ptr<CExpr> expression = std::make_shared<CExpr> ();
    std::string varName = ReadVar(in);
    expression->ReadExpr (in, matrices);
    if (expression->Contains (varName)) 
        throw WrongFormat ("can not evaluate recursive expressions\n");
    if (expression->Size () != 1 && !expression->ContainsVariable())
    try {
        auto e = expression->Evaluate (matrices);
        std::cout << "expr was evaluated as\n" << *e << std::endl;
        expression = std::make_shared <CExpr> (e);
    } catch (variable_not_set & var) {
        ;
    }
    if (matrices.find (varName) != matrices.end() ) {
        std::cout << "variable " << varName << " was rewritten" << std::endl;
        matrices.erase (varName);
    }
    matrices.emplace (varName, expression);
}
//----------------------------------------------------------------------
void CApplication::Evaluate (std::istream & in) {
    /*
        evaluates given expression and prints its result out
        does NOT change original expression
    */
    std::string varName = ReadVar(in);
    if (matrices.find(varName) == matrices.end())
        throw variable_not_set (varName);
    std::cout << *(matrices.find(varName)->second->Evaluate(matrices)) 
                                                         << std::endl;
}
//----------------------------------------------------------------------
void CApplication::Gem  (std::istream & in){
    /*
        method that prints out gemmed matrix (or evaluated expression)
        does not change matrix 
    */  
    std::string varName = ReadVar (in);
    if (matrices.find (varName) != matrices.end() ) {
        std::cout << "matrix \'" << varName << "\' in row-echelon from is:\n";
        std::cout << *matrices.find(varName)->second->Evaluate(matrices)
                                            ->GEM().first << std::endl;
    }
    else throw variable_not_set (varName);
}
//----------------------------------------------------------------------
void CApplication::Transpose (std::istream & in) {
    /*
        function that prints out transposed matrix (or evaluated expression)
        does not change matrix
    */
    std::string varName = ReadVar(in);
    if (matrices.count(varName) == 0)
        throw variable_not_set (varName);
    std::cout << *matrices.find(varName)->second->Evaluate(matrices)->Transpose() 
                                                                   << std::endl;
}
//----------------------------------------------------------------------
void CApplication::Rank (std::istream & in) {
    /*
        function that prints rank of matrix (or evaluated expression) 
    */
    std::string varName = ReadVar(in);
    if (matrices.find(varName) == matrices.end())
        throw variable_not_set (varName);
    auto matrix = (matrices.find(varName)->second->Evaluate(matrices));
    std::cout << "Rank of matrix \'" << varName << "\' is " << matrix->Rank () << std::endl; 
}
//----------------------------------------------------------------------
void CApplication::Inverse (std::istream & in) {
    /*
        prints out inverse of given square matrix or expression
        unless it's singular
    */
    std::string varName = ReadVar(in);
    if (matrices.find(varName) == matrices.end())
        throw variable_not_set (varName);
    auto matrix = matrices.find(varName)->second->Evaluate(matrices)->Inverse();
    if (matrix) std::cout << *matrix << std::endl;
    else std::cout << "matrix is singular\n";
}
//----------------------------------------------------------------------
void CApplication::Determinant (std::istream & in) {
    std::string varName = ReadVar(in);
    if (matrices.find(varName) == matrices.end())
        throw variable_not_set (varName);
    std::cout << "determinant of \'" << varName << "\' is ";
    std::cout << matrices.find (varName)->second->Evaluate(matrices)->Determinant() << std::endl;
}
//----------------------------------------------------------------------
void CApplication::Help () {
    // std::ifstream f ("help.txt");
    // char c;
    // while (!f.eof()) {
    //     f.read (&c, sizeof(char));
    //     std::cout.write (&c, sizeof(char));
    // }
    system ("less help.txt");
}
//----------------------------------------------------------------------
void   ReadSize (std::istream & in, int & m, int & n) {
    /* function that reads size of matrix for scan function */
    // size has format '( m , n )'
    char c;
    try {
        in >> c; if (c != '(') throw ("");
        in >> m;  
        in >> c; if (c != ',') throw ("");
        in >> n;
        in >> c; if (c != ')') throw ("");
    } catch (...) {
        throw WrongFormat ("could not read matrix size\n");
    }
}
//----------------------------------------------------------------------
void CApplication::Run () {
    /*
        run function
        - catches all exceptions that can occur while running the program
    */
    while (true) {
        try {
            Execute (std::cin);
        } catch (exit_exc & e) {
            std::cout << "exiting" << std::endl;
            break;
        }
        catch (wrong_command & e) {
            std::cout << e.what();
        }
        catch (WrongFormat & wf) {
            std::cout << wf.what();
        } 
        catch (WrongDimensions & wf) {
            wf.Print(std::cout);
        } 
        catch (variable_not_set & var) {
            std::cout << "variable " << var.what () << " not set\n";
        }
    }
}
//======================================================================