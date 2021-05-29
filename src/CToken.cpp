#include "../lib/CToken.h"

//=================
//   CToken class:
//=================
        Token::Token   (int t) : type (t) {}
int     Token::GetType (void) const 
{
  return type;
} 
CMatrix * Token::Value (void) 
{
  return nullptr;
}
std::ostream & operator << (std::ostream & out, const Token & t) { t.Print(out); return out; }
//=================
//   Matrix class:
//=================
// MatrixToken::MatrixToken (const CMatrix & m) : Token (Token::TokenType::MATRIX), matrix (std::make_shared<CMatrix>(m)) {

// }
MatrixToken::MatrixToken (std::shared_ptr<CMatrix> m) : Token (Token::TokenType::MATRIX), matrix (m) {

}
CMatrix * MatrixToken::Value () {
    return matrix.get();
}
// std::shared_ptr<CMatrix> MatrixToken::SharedValue () { 
//     return matrix; 
// }
void MatrixToken::Print (std::ostream & out) const {
    out << *matrix.get();
}
//====================
//    Operator class:
//====================
Operator::Operator (char sign, int prec) : 
    Token (Token::TokenType::Operator), op(sign), precedence(prec) {}
void Operator::Print (std::ostream & out) const {
    out << op;
}
int Operator::Precedence () {return precedence;}
std::shared_ptr<MatrixToken> Operator::Calculate (std::shared_ptr<Token> right, std::shared_ptr<Token> left) {
    if (op == '+') return std::make_shared<MatrixToken> (*left->Value() + *right->Value());
    if (op == '-') return std::make_shared<MatrixToken> (*left->Value() - *right->Value());
    
    throw ("");
}
//====================
//    Brackets class:
//====================
Brackets::Brackets (char b) : 
    Token  (b == '(' ? Token::TokenType::LeftBracket : Token::TokenType::RightBracket),
    leftBr (b == '(' ? true : false) {}
void Brackets::Print (std::ostream & out) const {
    out << (leftBr ? '(' : ')');
}
//==================
//   Numeric class:
//==================
Numeric::Numeric (int n) : Token(Token::TokenType::Number), value(n) {}
void Numeric::Print (std::ostream & out) const {
    out << value;
}
//===================
//   Variable class:
//===================

// Variable::Variable (const std::string & name, std::shared_ptr<CDense> matrix)
//   : Token(Token::TokenType::Variable), varName(name) {
//     // expression = std::make_shared<CExpr> (matrix);
//     evaluatedExpr = matrix;
// }

Variable::Variable (const  std::string & name, std::shared_ptr<CExpr>  matrix,
    CMemory & matrices) 
    : Token(Token::TokenType::Variable), varName(name),
    expression(matrix), evaluatedExpr (nullptr)  
{
    try {
        // auto t = expression->Evaluate(matrices);
        // evaluatedExpr = dynamic_pointer_cast<CMatrix>(make_shared<CDense>(expression->Evaluate(matrices)));
        evaluatedExpr = shared_ptr<CMatrix>(expression->Evaluate(matrices));  
    } catch (variable_not_set & var) {
        evaluatedExpr = nullptr;
    }
}
Variable::Variable (const std::string & name, nullptr_t) 
: Token(Token::TokenType::Variable), varName(name), expression(nullptr), evaluatedExpr (nullptr) {}
CMatrix * Variable::Value (void) {
   return evaluatedExpr.get();  
}
const std::string & Variable::Name () const {
    return varName;
}
void Variable::Print (std::ostream & out) const {
    if (evaluatedExpr != nullptr) out << *evaluatedExpr;
    else out << varName;
}
//--------------------------------------------------------