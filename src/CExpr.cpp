#include "hdr/CExpr.h"
//================================================================
CExpr::CExpr () = default;
//----------------------------------------------------------------
CExpr::CExpr (MPtr matrix) {
    tokens.push_back (std::make_shared<CMatrixToken> (matrix));
}
//----------------------------------------------------------------
int CExpr::Size () const {
    return tokens.size();
}
//----------------------------------------------------------------
void   CExpr::ReadExpr (std::istream & in, CMemory & matrices) {
    /*
        expression has to look like " = command ..." 
                                 or " = expression "
                      for instance :" = merge x y "  
                                 or " = ([1 2 ; 3 4] + [5 6; 7 8]) * ..."
        so firstly try to read command as string
        or else as mathematical expression 
    */
    std::string initialCommand;
    getline (in, initialCommand);
    std::istringstream expression (initialCommand);
    int com = TryToReadCommand (expression);

    // if read string was not defined as command, then it's maths expression:
    // open new stream from original string str and read an expression from it
    if (com == NOTSET) {
        std::istringstream is1 (initialCommand);
        char temp;
        is1 >> temp;            //ignoring '=' sign, because it was already read
        ParseExpr (is1, matrices);   
        TurnToRPN ();
        return;
    }
    else {
        ExecuteCommand (com, expression, matrices);   
    }
}
//---------------------------------------------------------
bool isNumber (char c) {
    return (c >= 48 && c <= 57);
}
//----------------------------------------------------------------
bool isChar (char c) {
    return ((c >= 65 && c <= 90) || (c >= 97 && c <= 122) );
}
//----------------------------------------------------------------
int CExpr::TryToReadCommand (std::istream & expression) {
    char temp;
    if (!(expression >> temp) || temp != '=') throw wrong_command();
    int com = NOTSET;
    std::string command;
    if (expression >> command ) {
        if (command == "merge" || command == "MERGE")
            com = MERGE;
        else if (command == "split" || command == "SPLIT")
            com = SPLIT;
        else if (command == "transpose" || command == "TRANSPOSE")
            com = TRANSPOSE;
        else if (command == "inverse" || command == "INVERSE") 
            com = INVERSE;
        else if (command[0] == '[' || isNumber (command[0]) || isChar(command[0]) || command[0] == '(') ;
        else throw WrongFormat();
    }
    return com;
}
//---------------------------------------------------------
void CExpr::ParseExpr (std::istream & in, CMemory & matrices) {
    char c;
    int tokenInt = CToken::Nothing;
    std::shared_ptr<CToken> token;
    while (in >> c) {
        if      (c == '-')
            token = std::make_shared<COperatorToken> ('-', 10);
        else if (c == '+') 
            token = std::make_shared<COperatorToken> ('+', 10);
        else if (c == '*')
            token = std::make_shared<COperatorToken> ('*', 20);
        else if (c == '/')
            token = std::make_shared<COperatorToken> ('/', 20);
        else if (c == '(' || c == ')') 
            token = std::make_shared<CBracketsToken> (c);
        else if (c == '[') {
            in.putback(c);
            std::shared_ptr<CMatrix> matrix;
            ReadMatrix (in, matrix);
            token = std::make_shared<CMatrixToken> (matrix);
        }
        else if (isNumber(c)) {
            //all number are represented as dense matrix with 1 value
            in.putback(c);
            float temp;
            in >> temp;
            token = std::make_shared<CMatrixToken> (temp);
        }
        else if (isChar (c) ) {
            in.putback(c);
            std::string tempName;
            tempName.resize(1);
            in >> tempName[0];
            token = std::make_shared<CVariableToken> (tempName);
        }
        else throw WrongFormat("Unknown character\n");
        tokens.push_back(token);
        // stop reading if there were 2 same tokens after each other unless it's paranthesis:
        if (! (tokenInt == CToken::LeftBracket || tokenInt == CToken::RightBracket))
        if (token->GetType() == tokenInt) throw WrongFormat ("same tokens after each other\n");
        tokenInt = token->GetType();
    }
}
//---------------------------------------------------------
void CExpr::TurnToRPN (void) {
    std::queue <std::shared_ptr<CToken>> outputQueue;
    std::stack <std::shared_ptr<CToken>> operatorStack;

    for (auto & token : tokens) {
        if (token->GetType() == CToken::TokenType::Matrix ||
            token->GetType() == CToken::TokenType::Number ||
            token->GetType() == CToken::TokenType::Variable ) {
            outputQueue.push (token);
        }
        else if (token->GetType() == CToken::TokenType::Operator)
            {
                while (!operatorStack.empty()) {
                    std::shared_ptr<CToken> o2 = operatorStack.top();
                    if (o2->GetType() == CToken::TokenType::Operator &&
                    std::dynamic_pointer_cast<COperatorToken>(o2)->Precedence() >=
                    std::dynamic_pointer_cast<COperatorToken>(token)->Precedence() ) {
                        outputQueue.push(o2);
                        operatorStack.pop();
                    }
                    else {
                        break;
                    }
                }
                operatorStack.push(token);
        }
        else if (token->GetType() == CToken::TokenType::LeftBracket) {
            operatorStack.push(token);
        }
        else if (token->GetType() == CToken::TokenType::RightBracket) {
            if (operatorStack.empty()) throw WrongFormat("no opening bracket\n");
            while (operatorStack.top()->GetType() != CToken::TokenType::LeftBracket) {
                outputQueue.push(operatorStack.top());
                operatorStack.pop();
                if (operatorStack.empty()) throw WrongFormat("no opening bracket\n");
            }
            operatorStack.pop();
        }
    }

    while (!operatorStack.empty()) {
        if (operatorStack.top()->GetType() == CToken::TokenType::LeftBracket ||
            operatorStack.top()->GetType() == CToken::TokenType::RightBracket)
            throw WrongFormat("mismatched paranthesis");
        outputQueue.push(operatorStack.top());
        operatorStack.pop();
    }

    std::vector <std::shared_ptr<CToken>> RPNExpr;

    while (!outputQueue.empty()) {
        RPNExpr.push_back(outputQueue.front());
        outputQueue.pop();
    }
    tokens = RPNExpr;
}
//----------------------------------------------------------------
void CExpr::ExecuteCommand (int command, std::istream & in, CMemory & matrices) {
    switch (command) {
        case MERGE : {
            MergeExpr (in, matrices);
            break;
        } 
        case SPLIT : {
            SplitExpr (in, matrices);
            break;
        }
        case TRANSPOSE : {
            TransposeExpr (in, matrices);
            break;
        }
        case INVERSE :{
            InverseExpr (in, matrices);
            break;
        }   
        default :
            break;
    }
}
//----------------------------------------------------------------
void CExpr::MergeExpr (std::istream & in, CMemory & matrices) {
    std::string var1, var2;
    if (!(in >> var1) || !(in >> var2) ) throw wrong_command();
    if (matrices.find (var1) == matrices.end()) throw variable_not_set (var1);
    if (matrices.find (var2) == matrices.end()) throw variable_not_set (var2);
    auto matrix1 = matrices.find(var1)->second->Evaluate(matrices);
    auto matrix2 = matrices.find(var2)->second->Evaluate(matrices); 
    std::shared_ptr<CMatrix> merged = CCommands::Merge (*matrix1, *matrix2);
    auto token = std::make_shared<CMatrixToken> (merged);
    tokens.push_back (token);
}
//---------------------------------------------------------
void CExpr::SplitExpr (std::istream & in, CMemory & matrices) {
    std::cout << "splitting" << std::endl;
    std::string var;
    if (!(in >> var) ) throw wrong_command();
    if (matrices.find (var) == matrices.end()) throw variable_not_set(var);
    int m, n, c_m, c_n;
    ReadSize  (in, m, n);
    ReadCoord (in, c_m, c_n);
    auto matrix = matrices.find(var)->second->Evaluate(matrices);
    auto splitted = CCommands::Split (*matrix, m, n, c_m, c_n);
    tokens.push_back (std::make_shared <CMatrixToken> (splitted));
}
//---------------------------------------------------------
void CExpr::TransposeExpr (std::istream & in, CMemory & matrices) {
    std::string var;
    if (!(in >> var) ) throw wrong_command();
    if (matrices.find (var) == matrices.end()) throw variable_not_set(var);
    auto matrix = matrices.find (var)->second->Evaluate(matrices);
    auto transposed = matrix->Transpose();
    tokens.push_back (std::make_shared <CMatrixToken> (transposed));
}
//---------------------------------------------------------
void CExpr::InverseExpr (std::istream & in, CMemory & matrices) {
    std::string var;
    if (!(in >> var) ) throw wrong_command();
    if (matrices.find (var) == matrices.end()) throw variable_not_set(var);
    auto matrix = matrices.find (var)->second->Evaluate(matrices);
    auto inversed = CCommands::Inverse(*matrix);
    if (inversed)
        tokens.push_back  (std::make_shared <CMatrixToken> (inversed));
    else 
        std::cout << "matrix is singular\n";
}
//---------------------------------------------------------
MPtr CExpr::Evaluate (CMemory & matrices) {
    std::stack <std::shared_ptr<CToken>> opStack;
    for (auto i : tokens) {
        if ((i)->GetType() == CToken::TokenType::Variable &&
            matrices.find (*(i)->GetName()) == matrices.end() )
            throw variable_not_set (*(i)->GetName() );
        if ((i)->GetType() == CToken::TokenType::Matrix ||
            (i)->GetType() == CToken::TokenType::Number ||
            (i)->GetType() == CToken::TokenType::Variable ) 
                opStack.push(i);
        else {
            if (opStack.size() < 2) throw ("Too few arguments");
            std::shared_ptr<CToken> topFirst =  opStack.top();
            opStack.pop();
            std::shared_ptr<CToken> topSecond = opStack.top();
            opStack.pop();
            MPtr left, right;
            if (topFirst->GetType() == CToken::TokenType::Variable)
                left = matrices.find (*topFirst->GetName())->second->Evaluate(matrices);
            else left = topFirst->Value();
            if (topSecond->GetType() == CToken::TokenType::Variable)
                right = matrices.find (*topSecond->GetName())->second->Evaluate(matrices);
            else right = topSecond->Value();
            opStack.push((std::dynamic_pointer_cast<COperatorToken>(i))
            ->Calculate(left, right, matrices));
        }
    }
    if (opStack.size() == 1) 
        return MPtr(opStack.top()->Value()->Clone());
    
    throw WrongFormat ("too many values\n");
}
//----------------------------------------------------------------
bool CExpr::Contains (const std::string & varName) const {
    for (const auto & t: tokens) {
        if (t->GetType() == CToken::Variable) {
            auto d = std::dynamic_pointer_cast<CVariableToken> (t);
            if (*(t->GetName())
                 == varName) return true;
        }
    }
    return false;
}
//----------------------------------------------------------------
bool CExpr::ContainsVariable () const {
    for (const auto & t: tokens) {
        if (t->GetType() == CToken::Variable) return true;
    }
    return false;
}
//----------------------------------------------------------------
std::ostream & operator << (std::ostream & out, const CExpr & expr) {
    for (long unsigned int i = 0; i < expr.tokens.size(); i++) {
        expr.tokens[i]->Print(out);
        if (i != expr.tokens.size() - 1) out << std::endl;
    }
    return out;
}
//================================================================