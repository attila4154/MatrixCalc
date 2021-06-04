#pragma once

#include "../lib/CExpr.h"
//================================================================
bool isNumber (char c) {
    return (c >= 48 && c <= 57);
}
//----------------------------------------------------------------
bool isChar (char c) {
    return ((c >= 65 && c <= 90) || (c >= 97 && c <= 122) );
}
//----------------------------------------------------------------
CExpr::CExpr (MPtr matrix) {
    tokens.push_back (std::make_shared<MatrixToken> (matrix));
}
//----------------------------------------------------------------
std::shared_ptr<Token> CExpr::GetMatrix () { 
    return tokens.back();
}
//----------------------------------------------------------------
void ReadDim (std::istream & in, int & m, int & n) {
    char c;
    if (!(in >> c) || c != '(') throw wrong_command("1");
    if (!(in >> m) || m < 0)    throw wrong_command("2");
    if (!(in >> n) || n < 0)    throw wrong_command("3");
    if (!(in >> c) || c != ')') throw wrong_command("4");
}
//----------------------------------------------------------------
void ReadCoord (std::istream & in, int & m, int & n) {
    char c;
    if (!(in >> c) || c != '(') throw wrong_command("5");
    if (!(in >> m) || m < 0)    throw wrong_command("6");
    if (!(in >> n) || n < 0)    throw wrong_command("7");
    if (!(in >> c) || c != ')') throw wrong_command("8");
}
//----------------------------------------------------------------
void CExpr::Transpose () {
    if (tokens.size() != 1) throw WrongDimensions();
    else tokens.back()->Transpose();
}
//----------------------------------------------------------------
void CExpr::ParseExpr (std::istream & in, CMemory & matrices) {
    char c;
    std::shared_ptr<Token> token;
    while (in >> c) {
        if      (c == '-') token = std::make_shared<Operator> ('-', 10);
        else if (c == '+') token = std::make_shared<Operator> ('+', 10);
        else if (c == '*') token = std::make_shared<Operator> ('*', 20);
        else if (c == '/') token = std::make_shared<Operator> ('/', 20);
        else if (c == '(' || c == ')') token = std::make_shared<Brackets> (c);
        else if (c == '[') {
            in.putback(c);
            std::shared_ptr<CMatrix> matrix;
            in >> matrix;
            token = std::make_shared<MatrixToken> (matrix);
        }
        else if (isNumber(c)) {
            in.putback(c);
            float temp;
            in >> temp;
            token = std::make_shared<MatrixToken> (temp);
        }
        else if (isChar (c) ) {
            in.putback(c);
            std::string tempName;
            tempName.resize(1);
            in >> tempName[0];
            std::cout << "reading variable" << std::endl;  
            if (matrices.find (tempName) != matrices.end()) {
                token = std::make_shared<Variable> (tempName, matrices.find(tempName)->second, matrices);
                std::cout << "variable " << tempName << " was found" << std::endl;
            }
            else token = std::make_shared<Variable> (tempName, nullptr);
        }
        else throw wrong_command();
        tokens.push_back(token);
    }
}
//---------------------------------------------------------
void   CExpr::ReadExpr (std::istream & in, CMemory & matrices) {
    //expression has to look like " = command ..." or " = expression "
    //              for instance :" = merge x y "  or " = ([1 2 ; 3 4] + [5 6; 7 8]) * ..."
    //so firstly try to read command as string and then find its value 
    //or else read mathematical expression

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
        ReadFullCommand(com, expression, matrices);   
    }
}
//---------------------------------------------------------
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
        else if (command == "gem"   || command == "GEM")
            com = GEM;
        else if (command == "transpose" || command == "TRANSPOSE")
            com = TRANSPOSE;
        else if (command[0] == '[' || isNumber (command[0]) || isChar(command[0]) || command[0] == '(') ;
        else throw wrong_command (command);
    }
    // else throw wrong_command();
    return com;
}
//---------------------------------------------------------
void CExpr::ReadFullCommand (int command, std::istream & in, CMemory & matrices) {
    switch (command) {
        case MERGE : {
            MergeCom(in, matrices);
            break;
        } 
        case SPLIT : {
            SplitCom (in, matrices);
            break;
        }
        case GEM : {
            std::string var;
            if (!(in >> var) ) throw wrong_command("here\n");
            if (matrices.find (var) == matrices.end()) throw variable_not_set(var);
            auto matrix = matrices.find(var)->second->Evaluate(matrices);
            auto gemmed = std::shared_ptr<CMatrix>(matrix->GEM().first);
            tokens.push_back (std::make_shared <MatrixToken> (gemmed));
            break;
        }
        case TRANSPOSE : {
            TransposeCom (in, matrices);
            break;
        }
        default :
            break;
    }
    
}
//---------------------------------------------------------
void CExpr::MergeCom (std::istream & in, CMemory & matrices) {
    std::string var1, var2;
    if (!(in >> var1) || !(in >> var2) ) throw wrong_command();
    if (matrices.find (var1) == matrices.end()) throw variable_not_set (var1);
    if (matrices.find (var2) == matrices.end()) throw variable_not_set (var2);
    auto matrix1 = matrices.find(var1)->second->Evaluate(matrices);
    auto matrix2 = matrices.find(var2)->second->Evaluate(matrices); 
    std::shared_ptr<CMatrix> merged = Merge (*matrix1, *matrix2);
    std::cout << *merged << std::endl;
    auto token = std::make_shared<MatrixToken> (merged);
    tokens.push_back (token);
}
//---------------------------------------------------------
void CExpr::SplitCom (std::istream & in, CMemory & matrices) {
    std::cout << "splitting" << std::endl;
    std::string var;
    if (!(in >> var) ) throw wrong_command();
    if (matrices.find (var) == matrices.end()) throw variable_not_set(var);
    int m, n, c_m, c_n;
    ReadDim (in, m, n);
    ReadCoord (in, c_m, c_n);
    auto matrix = matrices.find(var)->second->GetMatrix()->Value(matrices);
    auto splitted = Split (*matrix, m, n, c_m, c_n);
    tokens.push_back (std::make_shared <MatrixToken> (splitted));
}
//---------------------------------------------------------
void CExpr::TransposeCom (std::istream & in, CMemory & matrices) {
    std::string var;
    if (!(in >> var) ) throw wrong_command();
    if (matrices.find (var) == matrices.end()) throw variable_not_set(var);
    auto matrix = matrices.find (var)->second->Evaluate(matrices);
    auto transposed = matrix->Transpose();
    tokens.push_back (std::make_shared <MatrixToken> (transposed));
}
//---------------------------------------------------------
void CExpr::TurnToRPN (void) {
    std::queue <std::shared_ptr<Token>> outputQueue;
    std::stack <std::shared_ptr<Token>> operatorStack;

    for (auto & token : tokens) {
//        cout << "current token is " << *token << endl;
        if (token->GetType() == Token::TokenType::MATRIX ||
            token->GetType() == Token::TokenType::Number ||
            token->GetType() == Token::TokenType::Variable ) {
            outputQueue.push (token);
        }
        else if (token->GetType() == Token::TokenType::Operator)
            {
                while (!operatorStack.empty()) {
                    std::shared_ptr<Token> o2 = operatorStack.top();
                    //and ((the operator at the top of the operator stack has greater precedence)
                    if (o2->GetType() == Token::TokenType::Operator &&
                    std::dynamic_pointer_cast<Operator>(o2)->Precedence() >=
                    std::dynamic_pointer_cast<Operator>(token)->Precedence() ) {
                        outputQueue.push(o2);
                        operatorStack.pop();
                    }
                    else {
                        break;
                    }
                }
                operatorStack.push(token);
        }
        else if (token->GetType() == Token::TokenType::LeftBracket) {
            operatorStack.push(token);
        }
        else if (token->GetType() == Token::TokenType::RightBracket) {
            if (operatorStack.empty()) throw WrongFormat("no opening bracket");
            while (operatorStack.top()->GetType() != Token::TokenType::LeftBracket) {
                outputQueue.push(operatorStack.top());
                operatorStack.pop();
                if (operatorStack.empty()) throw WrongFormat("no opening bracket");
            }
            operatorStack.pop();
        }
    }

    while (!operatorStack.empty()) {
        if (operatorStack.top()->GetType() == Token::TokenType::LeftBracket ||
            operatorStack.top()->GetType() == Token::TokenType::RightBracket)
            throw WrongFormat("mismatched paranthesis");
        outputQueue.push(operatorStack.top());
        operatorStack.pop();
    }

    std::vector <std::shared_ptr<Token>> RPNExpr;

    while (!outputQueue.empty()) {
        RPNExpr.push_back(outputQueue.front());
        outputQueue.pop();
    }
    tokens = RPNExpr;
}
//----------------------------------------------------------------
MPtr CExpr::Evaluate (CMemory & matrices) {
    std::stack <std::shared_ptr<Token>> opStack;
    for (auto i : tokens) {
        if ((i)->GetType() == Token::TokenType::Variable &&
            matrices.find (*(i)->GetName()) == matrices.end() )
            throw variable_not_set (*(i)->GetName() );
    //   if token is variable and it doesn't have value yet, try to find it 
        // if ((*i)->GetType() == Token::TokenType::Variable 
        //  && (*i)->Value() == nullptr) {
        //     std::shared_ptr<Variable> tempVar = std::dynamic_pointer_cast<Variable>(*i);
        //     if (matrices.find(tempVar->Name()) == matrices.end()) 
        //         throw variable_not_set (tempVar->Name()); 
        //     else *i = std::make_shared<Variable>
        //     (tempVar->Name(),matrices.find(tempVar->Name())->second, matrices);
        //     std::cout << "variable was found and its value was set" << std::endl;
        // }
        if ((i)->GetType() == Token::TokenType::MATRIX ||
            (i)->GetType() == Token::TokenType::Number ||
            (i)->GetType() == Token::TokenType::Variable ) 
                opStack.push(i);
        else {
            if (opStack.size() < 2) throw ("Too few arguments");
            std::shared_ptr<Token> topFirst =  opStack.top();
            opStack.pop();
            std::shared_ptr<Token> topSecond = opStack.top();
            opStack.pop();
            opStack.push((std::dynamic_pointer_cast<Operator>(i))->Calculate(topFirst, topSecond, matrices));
        }
    }
    if (opStack.size() == 1) 
        return MPtr(opStack.top()->Value(matrices)->Clone());
    
    throw WrongFormat ("too many values\n");
}
//----------------------------------------------------------------
std::ostream & operator << (std::ostream & out, const CExpr & expr) {
    for (int i = 0; i < expr.tokens.size(); i++) {
        expr.tokens[i]->Print(out);
        if (i != expr.tokens.size() - 1) out << std::endl;
    }
    return out;
}