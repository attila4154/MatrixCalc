#include "hdr/CApplication.h"


//=======================================================================
void CApplication::Execute (std::istream & in) {
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
    if (command == "CLEAR"     || command == "clear")     return commands::CLEAR;
    if (command == "PRINT"     || command == "print")     return commands::PRINT;
    if (command == "DELETE"    || command == "delete")    return commands::DELETE;
    if (command == "GEM"       || command == "gem")       return commands::GEM;
    if (command == "SCAN"      || command == "scan")      return commands::SCAN;
    if (command == "EXIT"      || command == "exit")      return commands::EXIT;
    if (command == "TRANSPOSE" || command == "transpose") return commands::TRANSPOSE;
    if (command == "evaluate"  || command == "EVALUATE")  return commands::EVALUATE;
    if (command == "rank"      || command == "RANK")      return commands::RANK;
    if (command == "inverse"   || command == "INVERSE")   return commands::INVERSE;
    if (command == "help"      || command == "HELP")      return commands::HELP;
    if (command =="determinant" || command == "DETERMINANT")
                                                          return commands::DETERMINANT;
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
        case CLEAR : {
            system ("clear");
            break;
        }
        case DELETE : {
            Delete (is);
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
    if (!(in >> var) || var == "")
        throw WrongFormat ("could not read variable name\n");
    return var;
}
//----------------------------------------------------------------------
void CApplication::Print (std::istream & in) {
    std::string varName = ReadVar(in);
    if (matrices.find (varName) == matrices.end() ) throw variable_not_set (varName);
    std::string flag;
    if (!in.eof()) {
        in >> flag;
        if (flag == "-f") {
            std::string fileName;
            if (!in.eof() ) in >> fileName; 
            if (fileName.empty() ) throw WrongFormat ("no output file\n"); 
            WriteToFile (varName, fileName);
            return;
        }
        else if (!flag.empty() ) throw WrongFormat ("unknown flag\n");
    }

    std::cout << *(matrices.find(varName)->second) << std::endl;
}
//----------------------------------------------------------------------
void CApplication::WriteToFile 
(const std::string & varName, const std::string & fileName) {
    std::fstream f (fileName);
    matrices.find(varName)->second->Evaluate(matrices)->WriteToFile (fileName);
}
//----------------------------------------------------------------------
void CApplication::Scan (std::istream & in){
    std::string varName = ReadVar (in);
    std::string str;
    
    std::string flag;
    if (!in.eof()) {
        in >> flag;
        if (flag == "-f") {
            std::string fileName;
            if (!in.eof() ) in >> fileName; 
            if (fileName.empty() ) throw WrongFormat ("no output file\n"); 
            ReadFromFile (varName, fileName);
            return;
        }
        else if (!flag.empty() ) throw WrongFormat ("unknown flag\n");
    }

    std::istringstream is (str);
    std::shared_ptr<CMatrix> matrix;
    std::cout << "scanning:" << std::endl;
    ReadMatrix (std::cin, matrix);
    if (!std::cin.eof()) getline (std::cin, str);
    
    //  every matrix is stored as expression with 1 token:
    std::shared_ptr <CExpr> expr = std::make_shared<CExpr> (matrix);
    if (matrices.find (varName) != matrices.end() ) {
        std::cout << "variable " << varName << " was rewritten" << std::endl;
        matrices.erase (varName);
    }
    matrices.emplace (varName, expr);
}
//----------------------------------------------------------------------
void CApplication::ReadFromFile (const std::string & varName, const std::string & fileName) {
    std::fstream f (fileName);
    if (!f.is_open()) throw WrongFormat ("can not open file\n");
    std::shared_ptr<CDense> matrix = std::make_shared<CDense> (0,1);
    float value;
    int m = 0, cnt_n = 0, n = 0;
    // reading first row:
    while (f.peek () != '\n') {
        if (!(f >> value)) throw WrongFormat ("error while reading matrix\n");
        matrix->ReadRowValue(m, value);
        n++;
    }

    matrix->SetCols (n); 
    m++;

    while (f >> value) {
        matrix->ReadRowValue(m, value);
        cnt_n++;
        if (f.peek() == '\n') {
            if (cnt_n > n) throw WrongFormat ("expected less columns\n");
            else if (cnt_n < n) throw WrongFormat ("expected more columns\n");
            cnt_n = 0;
            m++;
        }
    }
    
    f.close();

    if (matrices.find (varName) != matrices.end()) {
        matrices.erase (varName);
        std::cout << "matrix \'" << varName << "\' was rewritten" << std::endl;
    }

    matrices.emplace (varName, std::make_shared<CExpr> (matrix) );
}
//----------------------------------------------------------------------
void CApplication::Delete      (std::istream & in) {
    std::string varName = ReadVar (in);
    if (matrices.find(varName) == matrices.end() ) throw variable_not_set (varName);
    matrices.erase (varName);
    std::cout << "matrix \'" << varName << "\' was deleted" << std::endl; 
}
//----------------------------------------------------------------------
void CApplication::ReadExpr(std::istream & in) {
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
    std::string varName = ReadVar(in);
    if (matrices.find(varName) == matrices.end())
        throw variable_not_set (varName);
    std::cout << *(matrices.find(varName)->second->Evaluate(matrices)) 
                                                         << std::endl;
}
//----------------------------------------------------------------------
void CApplication::Gem  (std::istream & in){
    std::string varName = ReadVar (in);
    if (matrices.find (varName) != matrices.end() ) {
        std::cout << "matrix \'" << varName << "\' in row-echelon from is:\n";
        std::cout << *CCommands::GEM(*matrices.find(varName)->second->Evaluate(matrices)).first
                                             << std::endl;
    }
    else throw variable_not_set (varName);
}
//----------------------------------------------------------------------
void CApplication::Transpose (std::istream & in) {
    std::string varName = ReadVar(in);
    if (matrices.count(varName) == 0)
        throw variable_not_set (varName);
    std::cout << *CCommands::Transpose(*matrices.find(varName)->second->Evaluate(matrices))
                                                                   << std::endl;
}
//----------------------------------------------------------------------
void CApplication::Rank (std::istream & in) {
    std::string varName = ReadVar(in);
    if (matrices.find(varName) == matrices.end())
        throw variable_not_set (varName);
    auto matrix = (matrices.find(varName)->second->Evaluate(matrices));
    std::cout << "Rank of matrix \'" << varName << "\' is " << CCommands::Rank(*matrix) << std::endl; 
}
//----------------------------------------------------------------------
void CApplication::Inverse (std::istream & in) {
    std::string varName = ReadVar(in);
    if (matrices.find(varName) == matrices.end())
        throw variable_not_set (varName);
    auto matrix = CCommands::Inverse (*matrices.find(varName)->second->Evaluate(matrices));
    if (matrix) std::cout << *matrix << std::endl;
    else std::cout << "matrix is singular\n";
}
//----------------------------------------------------------------------
void CApplication::Determinant (std::istream & in) {
    std::string varName = ReadVar(in);
    if (matrices.find(varName) == matrices.end())
        throw variable_not_set (varName);
    float det = CCommands::Determinant(*matrices.find (varName)->second->
                                        Evaluate(matrices));
    std::cout << "determinant of \'" << varName << "\' is ";
    std::cout << det << std::endl;
}
//----------------------------------------------------------------------
void CApplication::Help () {
    // std::ifstream f ("help.txt");
    // char c;
    // while (!f.eof()) {
    //     f.read (&c, sizeof(char));
    //     std::cout.write (&c, sizeof(char));
    // }
    system ("less assets/help.txt");
}
//----------------------------------------------------------------------
void CApplication::Run () {
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
            std::cout << wf.what();
        } 
        catch (variable_not_set & var) {
            std::cout << "variable " << var.what () << " not set\n";
        }
    }
}
//======================================================================