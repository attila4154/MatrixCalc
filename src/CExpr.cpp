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
CExpr::CExpr (shared_ptr<CMatrix> matrix) {
    tokens.push_back (make_shared<MatrixToken> (matrix));
}
//----------------------------------------------------------------
shared_ptr<Token> CExpr::GetMatrix () { 
    return tokens.back();
}
//----------------------------------------------------------------
void ReadDim (istream & in, int & m, int & n) {
    char c;
    if (!(in >> c) || c != '(') throw wrong_command("1");
    if (!(in >> m) || m < 0)    throw wrong_command("2");
    if (!(in >> n) || n < 0)    throw wrong_command("3");
    if (!(in >> c) || c != ')') throw wrong_command("4");
}
//----------------------------------------------------------------
void ReadCoord (istream & in, int & m, int & n) {
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
void CExpr::ParseExpr (istream & in, CMemory & matrices) {
    // std::string str;
    // getline (in, str);
    // cout << "read expr is " << str << endl;
    char c;
    shared_ptr<Token> token;
    while (in >> c) {
        if      (c == '-') token = make_shared<Operator> ('-', 10);
        else if (c == '+') token = make_shared<Operator> ('+', 10);
        else if (c == '*') token = make_shared<Operator> ('*', 20);
        else if (c == '/') token = make_shared<Operator> ('/', 20);
        else if (c == '(' || c == ')') token = make_shared<Brackets> (c);
        else if (c == '[') {
            in.putback(c);
            shared_ptr<CMatrix> matrix;
            in >> matrix;
            token = make_shared<MatrixToken> (matrix);
        }
        else if (isNumber(c)) {
            in.putback(c);
            float temp;
            in >> temp;
            token = make_shared <Numeric> (temp);
        }
        else if (isChar (c) ) {
            in.putback(c);
            string tempName;
            in >> tempName;
            cout << "reading variable" << endl;  
            if (matrices.find (tempName) != matrices.end()) {
                token = make_shared<Variable> (tempName, matrices.find(tempName)->second, matrices);
                cout << "variable " << tempName << " was found" << endl;
            }
            else token = make_shared<Variable> (tempName, nullptr);
        }
        else throw wrong_command();
        // token->Print(std::cout);
        tokens.push_back(token);
    }
}
//---------------------------------------------------------
void   CExpr::ReadExpr (istream & in, CMemory & matrices) {
    //expression has to look like " = command ..." or " = expression "
    //              for instance :" = merge x y "  or " = [1 2 ; 3 4] + [5 6; 7 8]"
    //so firstly try to read command as string and then find its value 
    //or else read mathematical expression

    std::string str;
    getline (in, str);
    std::istringstream expression (str);
    char temp;
    if (!(expression >> temp) || temp != '=') throw wrong_command();
    int com = NOTSET;
    std::string command;
    if (expression >> command ) {
        if (command == "merge" || command == "MERGE")
            com = MERGE;
        else if (command == "split" || command == "SPLIT")
            com = SPLIT;
    }
    else throw wrong_command(command);
// if read string was not defined as command, then it's maths expression:
// open new stream from original string str and read an expression from it
    if (com == NOTSET) {
        std::istringstream is1 (str);
        is1 >> temp;
        ParseExpr (is1, matrices);   
        TurnToRPN ();
        return;
    }
    else {
        ReadCommand(com, expression, matrices);   
    }
}
//---------------------------------------------------------
void CExpr::ReadCommand (int command, std::istream & in, CMemory & matrices) {
    switch (command) {
        case MERGE : {
            cout << "merging" << endl;
            string var1, var2;
            if (!(in >> var1) || !(in >> var2) ) throw wrong_command();
            if (matrices.find (var1) == matrices.end()) throw variable_not_set (var1);
            if (matrices.find (var2) == matrices.end()) throw variable_not_set (var2);
            cout << "12" << endl;
            auto matrix1 = matrices.find(var1)->second->GetMatrix()->Value();
            auto matrix2 = matrices.find(var2)->second->GetMatrix()->Value(); 
            std::shared_ptr<CMatrix> merged = Merge (*matrix1, *matrix2);
            cout << *merged << endl;
            auto token = std::make_shared<MatrixToken> (merged);
            tokens.push_back (token);
            break;
        } 
        case SPLIT : {
            cout << "splitting" << endl;
            string var;
            if (!(in >> var) ) throw wrong_command();
            if (matrices.find (var) == matrices.end()) throw variable_not_set(var);
            int m, n, c_m, c_n;
            ReadDim (in, m, n);
            ReadCoord (in, c_m, c_n);
            auto matrix = matrices.find(var)->second->GetMatrix()->Value();
            auto splitted = Split (*matrix, m, n, c_m, c_n);
            tokens.push_back (std::make_shared <MatrixToken> (splitted));
            break;
        }
        default :
            break;
    }
    
}
//---------------------------------------------------------
void CExpr::TurnToRPN (void) {
    queue <shared_ptr<Token>> outputQueue;
    stack <shared_ptr<Token>> operatorStack;

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
                    shared_ptr<Token> o2 = operatorStack.top();
                    //and ((the operator at the top of the operator stack has greater precedence)
                    if (o2->GetType() == Token::TokenType::Operator &&
                    dynamic_pointer_cast<Operator>(o2)->Precedence() >=
                    dynamic_pointer_cast<Operator>(token)->Precedence() ) {
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
            if (operatorStack.empty()) throw ("no opening bracket");
            while (operatorStack.top()->GetType() != Token::TokenType::LeftBracket) {
                outputQueue.push(operatorStack.top());
                operatorStack.pop();
                if (operatorStack.empty()) throw ("no opening bracket");
            }
            operatorStack.pop();
        }
    }

    while (!operatorStack.empty()) {
        if (operatorStack.top()->GetType() == Token::TokenType::LeftBracket ||
            operatorStack.top()->GetType() == Token::TokenType::RightBracket)
            throw ("mismatched paranthesis");
        outputQueue.push(operatorStack.top());
        operatorStack.pop();
    }

    vector <shared_ptr<Token>> RPNExpr;

    while (!outputQueue.empty()) {
        RPNExpr.push_back(outputQueue.front());
        outputQueue.pop();
    }
    tokens = RPNExpr;
}
//----------------------------------------------------------------
CMatrix * CExpr::Evaluate (CMemory & matrices) {
    stack <shared_ptr<Token>> opStack;
    for (auto i = tokens.begin(); i != tokens.end(); ++i) {
    //   if token is variable and it doesn't have value yet, try to find it 
        if ((*i)->GetType() == Token::TokenType::Variable 
         && (*i)->Value() == nullptr) {
            shared_ptr<Variable> tempVar = dynamic_pointer_cast<Variable>(*i);
            if (matrices.find(tempVar->Name()) == matrices.end()) 
                throw variable_not_set(); 
            else *i = make_shared<Variable>
            (tempVar->Name(),matrices.find(tempVar->Name())->second, matrices);
            cout << "variable was found and its value was set" << endl;
        }
        if ((*i)->GetType() == Token::TokenType::MATRIX ||
            (*i)->GetType() == Token::TokenType::Number ||
            (*i)->GetType() == Token::TokenType::Variable ) 
                opStack.push(*i);
        else {
            if (opStack.size() < 2) throw ("Too few arguments");
            shared_ptr<Token> topFirst =  opStack.top();
            opStack.pop();
            shared_ptr<Token> topSecond = opStack.top();
            opStack.pop();
            opStack.push((dynamic_pointer_cast<Operator>(*i))->Calculate(topFirst, topSecond));
        }
    }
    if (opStack.size() == 1) 
        {
            // auto p = opStack.top()->Value();
            return opStack.top()->Value()->Clone();
        }
    throw wrong_command("too many values");
}
//----------------------------------------------------------------
std::ostream & operator << (std::ostream & out, const CExpr & expr) {
    for (auto & token : expr.tokens) {
        token->Print(out);
    }
    return out;
}