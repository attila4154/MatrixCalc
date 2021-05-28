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
CExpr::CExpr (shared_ptr<CDense> matrix) {
    tokens.push_back (make_shared<MatrixToken> (matrix));
}
shared_ptr<Token> CExpr::GetMatrix () { 
    return tokens.back();
}
void CExpr::ParseExpr (istream & in, CMemory & matrices) {
    char c;
    shared_ptr<Token> token;
    // istringstream is (str);
    while (in >> c) {
        if      (c == '-') token = make_shared<Operator> ('-', 10);
        else if (c == '+') token = make_shared<Operator> ('+', 10);
        else if (c == '*') token = make_shared<Operator> ('*', 20);
        else if (c == '/') token = make_shared<Operator> ('/', 20);
        else if (c == '(' || c == ')') token = make_shared<Brackets> (c);
        else if (c == '[') {
            in.putback(c);
            CDense matrix;
            in >> matrix;
            // cout << "value is " << matrix << endl;
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
        else throw exception();
        // token->Print(std::cout);
        tokens.push_back(token);
    }
}
//---------------------------------------------------------
void   CExpr::ReadExpr (istream & in, CMemory & matrices) {
    char temp;
    if (!(in >> temp) 
    || temp != '=') throw WrongFormat();
    ParseExpr (in, matrices);
    TurnToRPN ();
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
CDense CExpr::Evaluate (CMemory & matrices) {
    stack <shared_ptr<Token>> opStack;
    for (auto i = tokens.begin(); i != tokens.end(); ++i) {
      //if token is variable and it doesn't have value yet, try to find it 
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
    if (opStack.size() == 1) return 
    *(opStack.top())->Value();
    throw wrong_command("too many values");
}
//----------------------------------------------------------------
std::ostream & operator << (std::ostream & out, const CExpr & expr) {
    for (auto & token : expr.tokens) {
        token->Print(out);
    }
    return out;
}

