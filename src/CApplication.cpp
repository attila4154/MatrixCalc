#pragma once
#include "../lib/CApplication.h"


//=======================================================================
int CApplication::GetCommand (std::istream & in) {
    std::string command;
    getline (in, command, ' ');
    if (command == "PRINT"|| command == "print") return commands::PRINT;
    if (command == "GEM"  || command == "gem")   return commands::GEM;
    if (command == "SCAN" || command == "scan")  return commands::SCAN;
    if (command == "EXIT" || command == "exit")  return commands::EXIT;
    if (command == "evaluate" || command == "EVALUATE")  return commands::EVALUATE;
    else {
        char temp;
        if (!(in >> temp) || temp != '=') throw wrong_command(); 
        in.putback (temp);
        in.putback (' ');
        in.putback ((command.data())[0]);
        return commands::READEXPR;
    }
}
//----------------------------------------------------------------------
std::string CApplication::ReadVar (std::istream & in) {
    std::string var;
    try {
        getline (in, var, ' ');
    } catch (...) {
        throw exit_exc ();
    }
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
        throw ("");
    }
}
//----------------------------------------------------------------------
void CApplication::ReadMatrix (std::istream & in, shared_ptr<CDense> matrix) {
    std::string buff;
    try {
        getline (in, buff);
    } catch (...) {
        throw exit_exc();
    }
    std::istringstream is (buff);
    if (!(is >> *matrix)) throw exit_exc();
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
        case commands::EXIT : 
            throw exit_exc();
        default:
            throw wrong_command();
    }
}
//----------------------------------------------------------------------
void CApplication::Print(std::istream & in){
    string varName = ReadVar(in);
        if (matrices.count(varName) == 0)
            std::cout << "variable " << varName << " does not exist" 
        << std::endl;
        else
            std::cout << "printing:\n" << *(matrices.find(varName)->second)
        << std::endl;
}
//----------------------------------------------------------------------
void CApplication::Gem  (std::istream & in){
    std::cout << "GEMing" << std::endl;
}
//----------------------------------------------------------------------
void CApplication::Scan (std::istream & in){
    std::cout << "scanning:" << std::endl;
    string varName = ReadVar (in);
    int m, n;
    ReadSize (in, m, n);
    shared_ptr<CDense> matrix = make_shared<CDense>(m,n);
    ReadMatrix (std::cin, matrix);
    shared_ptr <CExpr> expr = make_shared<CExpr> (matrix);
    matrices.emplace (varName, expr);
}
//----------------------------------------------------------------------
void CApplication::ReadExpr(std::istream & in){
    shared_ptr<CExpr> expression = make_shared<CExpr> ();
    string varName = ReadVar(in);
    expression->ReadExpr(in, matrices);
    matrices.emplace (varName, expression);
    // try {
    //     expression->Evaluate(matrices);
    // } catch (variable_not_set & var) {
    //     ;
    // }
}
//----------------------------------------------------------------------
void CApplication::Evaluate (std::istream & in) {
    string varName = ReadVar(in);
    if (matrices.find(varName) == matrices.end())
        throw (wrong_command ("variable not set"));
    cout << matrices.find(varName)->second->Evaluate(matrices) << endl;
}
//----------------------------------------------------------------------
void CApplication::Run () {
    while (true) {
        try {
            Execute (std::cin, matrices);
        } catch (exit_exc & e)
        {
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
            std::cout << wf.what();
        } 
    }
}
//======================================================================