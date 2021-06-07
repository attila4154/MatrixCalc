#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <queue>
#include <stack>

#include "CDense.h"
#include "../src/CDense.cpp"

using namespace std;

class Token {
  public:

     enum TokenType {
        Nothing,
        Operator,
        MATRIX,
        LeftBracket,
        RightBracket
    };

    Token (int t) : type (t) {}
    int GetType (void) const {return type;}
    virtual void Print (ostream & out) const = 0; 
//    virtual double Calculate (void) const = 0;
    // virtual const CDense & Value () const = 0;
    friend ostream & operator << (ostream & out, const Token & t) {t.Print(out); return out;}

  protected:
    int type = TokenType::Nothing;
};
//=========================================================
class Matrix : public Token {
  public:
    Matrix (const CDense & m) : Token (Token::TokenType::MATRIX), matrix (m) {}
    const CDense & Value () const { return matrix; }
    void Print (ostream & out) const {
        out << matrix;
    }
  private:
    CDense matrix;
};
//=========================================================
class Operator : public Token {
public:
    Operator (char sign, int prec) : Token (Token::TokenType::Operator), op(sign), precedence(prec) {}
    void Print (ostream & out) const {
        out << op;
    }
    int Precedence () {return precedence;}
    // const CDense & Value () const { CDense m; return m; }
    shared_ptr<Matrix> Calculate (shared_ptr<Token> left, shared_ptr<Token> right);
private:
    char op;
    int precedence;
};
//=========================================================
class Brackets : public Token {
  public:
    Brackets (char b) : Token(b == '(' ? Token::TokenType::LeftBracket : Token::TokenType::RightBracket),
                        leftBr (b == '(' ? true : false) {}
    void Print (ostream & out) const {
        out << (leftBr ? '(' : ')');
    }
    // const CDense & Value () const {return CDense();}

  private:
    bool leftBr;
};

shared_ptr<Matrix> Operator::Calculate (shared_ptr<Token> right, shared_ptr<Token> left) {
    if (op == '+') return make_shared<Matrix>
    (dynamic_pointer_cast<Matrix>(left)->Value() + dynamic_pointer_cast<Matrix>(right)->Value() );
    // else if  (op == '-') return make_shared<Numeric> (left->Value() - right->Value());
    // else if  (op == '/') return make_shared<Numeric> (left->Value() / right->Value());
    // else if  (op == '*') return make_shared<Numeric> (left->Value() * right->Value());
    throw ("");
}
//=========================================================
vector<shared_ptr<Token>> Parse (string & str) {
    vector<shared_ptr<Token>> tempStor;
    char c;
    shared_ptr<Token> token;
    istringstream is (str);
    while (is >> c) {
        if      (c == '-') token = make_shared<Operator> ('-', 10);
        else if (c == '+') token = make_shared<Operator> ('+', 10);
        else if (c == '*') token = make_shared<Operator> ('*', 20);
        else if (c == '/') token = make_shared<Operator> ('/', 20);
        else if (c == '(' || c == ')') token = make_shared<Brackets> (c);
        else if (c == '[') {
            is.putback(c);
            CDense matrix;
            is >> matrix;
            // cout << "value is " << matrix << endl;
            token = make_shared<Matrix> (matrix);
        }
        else throw exception();
        tempStor.push_back(token);
    }
    return tempStor;
}
//---------------------------------------------------------
void Print (ostream & out, const vector <shared_ptr<Token>> & vec) {
    for (auto & x : vec) {
        out << *x;
    }
    out << endl;
}
//---------------------------------------------------------
CDense EvaluateExpression (vector <shared_ptr<Token>> & expr) {
    stack <shared_ptr<Token>> opStack;
    for (auto i = expr.begin(); i != expr.end(); ++i) {
        if ((*i)->GetType() == Token::TokenType::MATRIX)
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
    if (opStack.size() == 1) return dynamic_pointer_cast<Matrix>(opStack.top())->Value();
    throw ("too many values");
}
//---------------------------------------------------------
CDense Compute (vector <shared_ptr<Token>> & vec) {
    queue <shared_ptr<Token>> outputQueue;
    stack <shared_ptr<Token>> operatorStack;

    for (auto & token : vec) {
//        cout << "current token is " << *token << endl;
        if (token->GetType() == Token::TokenType::MATRIX) {
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



    vector <shared_ptr<Token>> out;

    while (!outputQueue.empty()) {
        out.push_back(outputQueue.front());
        outputQueue.pop();
    }

    // cout << "RPN expression is : ";
    // Print(cout, out);

    return EvaluateExpression(out);
}

//---------------------------------------------------------
int main () {
    string str;
    vector <shared_ptr<Token>> storage;
    while (getline(cin,str)) {
        storage = Parse(str);
        // cout << "the expression is: ";
        // Print (cout, storage);
        try {
            CDense res = Compute(storage);
            cout << "the result is:\n" << res << endl;

        } catch (exception & e) {
            cerr << e.what();
        }
    }

    return 0;
}
