#pragma once
#include "../lib/CApplication.h"
//----------------------------------------------------------------------
int CApplication::GetCommand (std::istream & in) {
    std::string command;
    getline (in, command, ' ');
    if (command == "PRINT"|| command == "print") return commands::PRINT;
    if (command == "GEM"  || command == "gem")   return commands::GEM;
    if (command == "SCAN" || command == "scan")  return commands::SCAN;
    if (command == "EXIT" || command == "exit")  return commands::EXIT;
    else {
        char temp;
        if (!(in >> temp) || temp != '=') throw custom_exception(); 
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
        throw custom_exception ();
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
void CApplication::ReadMatrix (std::istream & in, CDense & matrix) {
    std::string buff;
    try {
        getline (in, buff);
    } catch (...) {
        throw custom_exception();
    }
    std::istringstream is (buff);
    if (!(is >> matrix)) throw custom_exception();
    if (is.rdbuf()->in_avail() != 0) throw custom_exception();
}
//----------------------------------------------------------------------
void CApplication::Execute (const std::string & src, std::map <std::string, CDense> & matrices) {
    std::istringstream is (src);
    int command = GetCommand(is);
    switch (command) {
        case commands::PRINT : 
        {
            Print(is);
            break;
        }
        case commands::GEM : 
        {
            Gem(is);
            break;
        }
        case commands::SCAN : 
        {
            Scan (is);
            break;
        }
        case commands::EXIT : 
        {
            throw custom_exception();
        case commands::READEXPR :
            ReadExpr (is);
            break;
        }
        default:
            throw wrong_command();
    }
}
//----------------------------------------------------------------------
void CApplication::Print (std::istream & in) {
    string varName = ReadVar(in);
    if (matrices.count(varName) == 0)
        std::cout << "variable " << varName << " does not exist" << std::endl;
    else
        std::cout << "printing:\n" << matrices.find(varName)->second << std::endl;
}
void CApplication::Gem (std::istream & in) {
    std::cout << "GEMing" << std::endl;
}
void CApplication::Scan (std::istream & in) {
    std::cout << "scanning:" << std::endl;
    string varName = ReadVar (in);
    int m, n;
    ReadSize (in, m, n);
    CDense matrix (m, n);
    ReadMatrix (std::cin, matrix);
    matrices.emplace (varName, matrix);
}
void CApplication::ReadExpr(std::istream & in) {
    CExpr expression;
    string varName = ReadVar(in);
    expression.ReadExpr(in, matrices);
    cout << "result is:\n" << expression.Evaluate() << endl;
    matrices.emplace (varName, expression.Evaluate());
}
//----------------------------------------------------------------------
void CApplication::ReadCommand (std::istream & in, std::map <std::string, CDense> & matrices) {
    if (in.eof()) throw custom_exception();
    std::string str;
    getline (in, str);
    if (in.eof()) throw custom_exception();
    Execute (str, matrices);
}
//----------------------------------------------------------------------
void CApplication::Run () {
    while (true) {
        try {
            ReadCommand(std::cin, matrices);
        } catch (custom_exception & e)
        {
            std::cout << "exiting" << std::endl;
            break;
        }
        catch (wrong_command & e) {
            std::cout << "wrong command" << std::endl;
        }
        catch (WrongFormat & wf) {
            std::cout << wf.what();
        } 
        catch (WrongDimensions & wf) {
            std::cout << wf.what();
        } 
    }
}