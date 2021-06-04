#pragma once
#include "../lib/CApplication.h"


//=======================================================================
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
    if (command == "determinant"   || command == "DETERMINANT")   return commands::DETERMINANT;
    if (command == "help"   || command == "HELP")   return commands::HELP;
    else {
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
std::string CApplication::ReadVar (std::istream & in) {
    std::string var;
    if (!getline (in, var, ' '))
        throw WrongFormat ("could not read variable name\n");
    if (matrices.find (var) == matrices.end() ) throw variable_not_set (var);
    return var;
}
//----------------------------------------------------------------------
void   CApplication::ReadSize    (std::istream & in, int & m, int & n) {
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
void CApplication::ReadMatrix (std::istream & in, MPtr & matrix, int m, int n) {
    std::string buff;
    try {
        getline (in, buff);
    } catch (...) {
        throw exit_exc();
    }
    std::istringstream is (buff);

    if (ShouldBeDense (buff) ) {
        matrix = std::make_shared <CDense>  (m,n);
    } else 
        matrix = std::make_shared <CSparse> (m,n);

    //for now matrix will be pointer to CDense, change it so that it was optional???
    if (!(is >> matrix)) throw exit_exc();
    if (is.rdbuf()->in_avail() != 0) throw exit_exc();
}
//----------------------------------------------------------------------
void CApplication::Execute (std::istream & in, 
                            CMemory & matrices) {
    std::cout << "> ";
    std::string str; getline (in, str);
    if (in.eof() ) throw exit_exc();
    std::istringstream is (str);
    int command = GetCommand(is);
    switch (command) {
        case commands::PRINT : 
        {
            Print (is);
            break;
        }
        case commands::GEM : 
        {
            Gem (is);
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
void CApplication::Print(std::istream & in){
    std::string varName = ReadVar(in);
    std::cout << *(matrices.find(varName)->second) << std::endl;
}
//----------------------------------------------------------------------
void CApplication::Gem  (std::istream & in){
    std::string varName = ReadVar (in);
    std::cout << "GEMing" << std::endl;
    std::cout << matrices.find(varName)->second->Evaluate(matrices)
                                        ->GEM().first << std::endl;
}
//----------------------------------------------------------------------
void CApplication::Scan (std::istream & in){
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
    std::shared_ptr <CExpr> expr = std::make_shared<CExpr> (matrix);
    if (matrices.find (varName) != matrices.end() ) {
        std::cout << "variable " << varName << " was rewritten" << std::endl;
        matrices.erase (varName);
    }
    matrices.emplace (varName, expr);
}
//----------------------------------------------------------------------
void CApplication::ReadFromFile(const std::string & in, std::string varName) {
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
void CApplication::ReadExpr(std::istream & in){
    std::shared_ptr<CExpr> expression = std::make_shared<CExpr> ();
    std::string varName = ReadVar(in);
    expression->ReadExpr(in, matrices);
    if (matrices.find (varName) != matrices.end() ) {
        std::cout << "variable " << varName << " was rewritten" << std::endl;
        matrices.erase (varName);
    }
    matrices.emplace (varName, expression);
}
//----------------------------------------------------------------------
void CApplication::Evaluate (std::istream & in) {
    std::string varName = ReadVar(in);
    if (matrices.find(varName) == matrices.end())
        throw variable_not_set (varName);
    
    auto p = (matrices.find(varName)->second->Evaluate(matrices));
    // matrices.erase (varName);
    // shared_ptr<CExpr> expr = make_shared <CExpr>(p);
    // matrices.emplace (varName, expr);   
    std::cout << *p << std::endl;
}
//----------------------------------------------------------------------
void CApplication::Transpose(std::istream & in) {
    std::string varName = ReadVar(in);
        if (matrices.count(varName) == 0)
            std::cout << "variable " << varName << " does not exist" 
                                                       << std::endl;
        else {
            // std::cout << varName << " was transposed\n";
            auto m = matrices.find(varName)->second->Evaluate(matrices);
            auto t = m->Transpose();
            std::cout << *t << std::endl;
        }
}
//----------------------------------------------------------------------
void CApplication::Rank (std::istream & in) {
    std::string varName = ReadVar(in);
    if (matrices.find(varName) == matrices.end())
        throw variable_not_set (varName);
    auto matrix = (matrices.find(varName)->second->Evaluate(matrices));
    std::cout << "Rank of matrix " << varName << " is " << matrix->Rank () << std::endl; 
}
//----------------------------------------------------------------------
void CApplication::Inverse (std::istream & in) {
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
    std::cout << "determinant of " << varName << " is ";
    std::cout << matrices.find (varName)->second->Evaluate(matrices)->Determinant() << std::endl;
}
//----------------------------------------------------------------------
void CApplication::Help () {
    std::ifstream f ("help.txt");
    char c;
    while (!f.eof()) {
        f.read (&c, sizeof(char));
        std::cout.write (&c, sizeof(char));
    }
}
//----------------------------------------------------------------------
void CApplication::Run () {
    while (true) {
        try {
            Execute (std::cin, matrices);
        } catch (exit_exc & e) {
            std::cout << "exiting" << std::endl;
            break;
        }
        catch (wrong_command & e) {
            e.Print (std::cout);
        }
        catch (WrongFormat & wf) {
            std::cout << wf.what();
        } 
        catch (WrongDimensions & wf) {
            std::cout << wf.what();
        } 
        catch (variable_not_set & var) {
            std::cout << "variable " << var.what () << " not set\n";
        }
    }
}
//======================================================================