#include "../lib/CToken.h"

//=================
//   CToken class:
//=================
        Token::Token   (int t) : type (t) {}
//-------------------------------------------------------------
int     Token::GetType (void) const 
{
  return type;
} 
//-------------------------------------------------------------
std::string * Token::GetName () {
    // overrided only by variable class
    return nullptr;
}
//-------------------------------------------------------------
MPtr Token::Value (CMemory & matrices) 
{
    // overrided by variable and matrix classes
    return nullptr;
}
//-------------------------------------------------------------
std::ostream & operator << (std::ostream & out, const Token & t) { 
    t.Print(out); return out;
}
//-------------------------------------------------------------
//=================
//   Matrix class:
//=================
//-------------------------------------------------------------
MatrixToken::MatrixToken (std::shared_ptr<CMatrix> m) : 
        Token (Token::TokenType::MATRIX), matrix (m) {}
//-------------------------------------------------------------
//            float number constructor
MatrixToken::MatrixToken (float number) : Token (Token::TokenType::Number) {
    matrix = std::make_shared<CDense>(number);
}
//-------------------------------------------------------------
MatrixToken::MatrixToken (CMatrix * m) : Token (Token::TokenType::MATRIX),
                                 matrix (std::shared_ptr <CMatrix> (m) ) {}
//-------------------------------------------------------------
MPtr MatrixToken::Value (CMemory & matrices) {
    return matrix;
}
//-------------------------------------------------------------
void MatrixToken::Print (std::ostream & out) const {
    out << *matrix.get();
}
//-------------------------------------------------------------
//====================
//    Operator class:
//====================
//-------------------------------------------------------------
Operator::Operator (char sign, int prec) :
    Token (Token::TokenType::Operator), op(sign), precedence(prec) {}
//-------------------------------------------------------------
void Operator::Print (std::ostream & out) const {
    out << op;
}
//-------------------------------------------------------------
int Operator::Precedence () {
    return precedence;
}
//-------------------------------------------------------------
std::shared_ptr<MatrixToken> Operator::Calculate 
(std::shared_ptr<Token> right, std::shared_ptr<Token> left, CMemory & matrices) {
    /*
        returns shared pointer on CMatrix of product of 2 matrices / variables
        - for variables finds value first
    */
    MPtr leftMatrix, rightMatrix;
    leftMatrix = left->Value (matrices);
    rightMatrix = right->Value (matrices);
    if (op == '+') return std::make_shared<MatrixToken> (*leftMatrix + *rightMatrix);
    if (op == '-') return std::make_shared<MatrixToken> (*leftMatrix - *rightMatrix);
    if (op == '*') return std::make_shared<MatrixToken> (*leftMatrix * *rightMatrix);
    
    throw ("");
}
//================---------------------------------------------
//====================
//    Brackets class:
//====================
//================---------------------------------------------
Brackets::Brackets (char b) : 
    Token    (b == '(' ? TokenType::LeftBracket : TokenType::RightBracket),
    isLeftBr (b == '(' ? true : false) {}
//-------------------------------------------------------------
void Brackets::Print (std::ostream & out) const {
    // won't be used because expressions in RPN notation do not have paranthesis 
    ;
}
//-------------------------------------------------------------
//===================
//   Variable class:
//===================
//-------------------------------------------------------------
Variable::Variable (const std::string & name) : Token (TokenType::Variable), varName(name) {}
//-------------------------------------------------------------
MPtr Variable::Value (CMemory & matrices) {
    return matrices.find(varName)->second->Evaluate(matrices);
}
//-------------------------------------------------------------
std::string * Variable::GetName  () {
    return &varName;
}
//-------------------------------------------------------------
void Variable::Print (std::ostream & out) const {
    out << varName;
}
//=============================================================