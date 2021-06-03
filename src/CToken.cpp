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
    return nullptr;
}
//-------------------------------------------------------------
MPtr Token::Value (CMemory & matrices) 
{
  return nullptr;
}
//-------------------------------------------------------------
void Token::Transpose () {
    ;
}
//-------------------------------------------------------------
std::ostream & operator << (std::ostream & out, const Token & t) { t.Print(out); return out; }
//-------------------------------------------------------------
//=================
//   Matrix class:
//=================
MatrixToken::MatrixToken (std::shared_ptr<CMatrix> m) : Token (Token::TokenType::MATRIX), matrix (m) {
}
//-------------------------------------------------------------
MatrixToken::MatrixToken (float number) : Token (Token::TokenType::Number) {
    matrix = std::make_shared<CDense>(number);
}
//-------------------------------------------------------------
MatrixToken::MatrixToken (CMatrix * m) : Token (Token::TokenType::MATRIX), matrix (std::shared_ptr <CMatrix> (m) ) {}
//-------------------------------------------------------------
MPtr MatrixToken::Value (CMemory & matrices) {
    return matrix;
}
//-------------------------------------------------------------
void MatrixToken::Print (std::ostream & out) const {
    out << *matrix.get();
}
//-------------------------------------------------------------
void MatrixToken::Transpose () {
    matrix->Transpose();
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
int Operator::Precedence () {return precedence;}
//-------------------------------------------------------------
std::shared_ptr<MatrixToken> Operator::Calculate 
(std::shared_ptr<Token> right, std::shared_ptr<Token> left, CMemory & matrices) {
    MPtr leftMatrix, rightMatrix;
    if (left->GetType() == TokenType::Variable) {
        leftMatrix = matrices.find(*left->GetName())->second->Evaluate(matrices);
    }
    else leftMatrix = left->Value(matrices);
    if (right->GetType() == TokenType::Variable) {
        rightMatrix = matrices.find(*right->GetName())->second->Evaluate(matrices);
    }
    else rightMatrix = right->Value(matrices);
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
    Token  (b == '(' ? Token::TokenType::LeftBracket : Token::TokenType::RightBracket),
    leftBr (b == '(' ? true : false) {}
//-------------------------------------------------------------
void Brackets::Print (std::ostream & out) const {
    out << (leftBr ? '(' : ')');
}
//-------------------------------------------------------------
//===================
//   Variable class:
//===================
//-------------------------------------------------------------
Variable::Variable (const  std::string & name, std::shared_ptr<CExpr>  matrix,
    CMemory & matrices) 
    : Token(Token::TokenType::Variable), varName(name),
    expression(matrix), evaluatedExpr (nullptr)  
{
    try {
        evaluatedExpr = MPtr(expression->Evaluate(matrices));
    } catch (variable_not_set & var) {
        std::cout << "variable was not found, value is null" << std::endl;
        evaluatedExpr = nullptr;
    }
}
//-------------------------------------------------------------
Variable::Variable (const std::string & name, nullptr_t) 
: Token(Token::TokenType::Variable), varName(name), expression(nullptr), evaluatedExpr (nullptr) {}
//-------------------------------------------------------------
MPtr Variable::Value (CMemory & matrices) {
    return matrices.find(varName)->second->Evaluate(matrices);
}
//-------------------------------------------------------------
const std::string & Variable::Name () const {
    return varName;
}
//-------------------------------------------------------------
std::string * Variable::GetName  () {
    return &varName;
}
//-------------------------------------------------------------
void Variable::Print (std::ostream & out) const {
    if (evaluatedExpr) {
        out << *evaluatedExpr;
    }
    else {
        out << varName;
    }
}
//=============================================================