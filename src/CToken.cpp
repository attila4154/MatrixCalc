#include "../lib/CToken.h"

//CToken class:
        Token::Token   (int t) : type (t) {}
int     Token::GetType (void) const 
{
  return type;
} 
CDense * Token::Value (void) 
{
  return nullptr;
}
std::ostream & operator << (std::ostream & out, const Token & t) { t.Print(out); return out; }
//=========================================================
//Matrix class:
MatrixToken::MatrixToken (const CDense & m) : Token (Token::TokenType::MATRIX), matrix (std::make_shared<CDense>(m)) {}
MatrixToken::MatrixToken (std::shared_ptr<CDense> m) : Token (Token::TokenType::MATRIX), matrix (m) {}
CDense * MatrixToken::Value () { return matrix.get(); }
std::shared_ptr<CDense> MatrixToken::SharedValue () { return matrix; }
void MatrixToken::Print (std::ostream & out) const {
    out << *matrix.get();
}
//=========================================================
//Operator class:
Operator::Operator (char sign, int prec) : 
    Token (Token::TokenType::Operator), op(sign), precedence(prec) {}
void Operator::Print (std::ostream & out) const {
    out << op;
}
int Operator::Precedence () {return precedence;}
std::shared_ptr<MatrixToken> Operator::Calculate (std::shared_ptr<Token> right, std::shared_ptr<Token> left) {
    if (op == '+') return make_shared<MatrixToken>(*left->Value() + *right->Value());
    if (op == '-') return std::make_shared<MatrixToken> (*left->Value() - *right->Value());
    // else if (op == '*') return std::make_shared<Matrix> 
    // (std::dynamic_pointer_cast<Numeric>(left)->Value() * std::dynamic_pointer_cast<Matrix>(right)->Value() );
    // else if  (op == '/') return make_shared<Numeric> (left->Value() / right->Value());
    // else if  (op == '*') return make_shared<Numeric> (left->Value() * right->Value());
    throw ("");
}
//=========================================================
//Brackets class:
Brackets::Brackets (char b) : 
    Token  (b == '(' ? Token::TokenType::LeftBracket : Token::TokenType::RightBracket),
    leftBr (b == '(' ? true : false) {}
void Brackets::Print (std::ostream & out) const {
    out << (leftBr ? '(' : ')');
}
//=========================================================
//Numeric class:
Numeric::Numeric (int n) : Token(Token::TokenType::Number), value(n) {}
void Numeric::Print (std::ostream & out) const {
    out << value;
}
//=========================================================
//Variable class:
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
        evaluatedExpr = make_shared<CDense> (expression->Evaluate(matrices));
    } catch (variable_not_set & var) {
        evaluatedExpr = nullptr;
    }
}
Variable::Variable (const std::string & name, nullptr_t) 
: Token(Token::TokenType::Variable), varName(name), expression(nullptr), evaluatedExpr (nullptr) {}
CDense * Variable::Value (void) {
//    if (!evaluatedExpr && expression) evaluatedExpr = make_shared<CDense>expression->Evaluate
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