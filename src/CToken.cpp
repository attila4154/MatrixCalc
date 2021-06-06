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
OperatorToken::OperatorToken (char sign, int prec) :
    Token (Token::TokenType::Operator), op(sign), precedence(prec) {}
//-------------------------------------------------------------
void OperatorToken::Print (std::ostream & out) const {
    out << op;
}
//-------------------------------------------------------------
int OperatorToken::Precedence () {
    return precedence;
}
MPtr OperatorToken::Sum (const CMatrix & left, const CMatrix & right) {
    if (!left.is_matrix) {
        if (!right.is_matrix) {
            return (std::make_shared<CDense> (left.GetValue(0,0) + right.GetValue(0,0)));
        }
        else throw WrongFormat("cannot sum by number\n");
    }
    if (!right.is_matrix) throw WrongFormat("cannot sum by number\n");
    if (left.m_m != right.m_m || left.m_n != right.m_n) throw WrongDimensions (left.m_m, left.m_n,
                                                                              right.m_m, right.m_n);  
    std::shared_ptr<CMatrix> temp;
    if ( ProductShouldBeDense (left, right, '+') )
        temp = std::make_shared<CDense>  (left.m_m, left.m_n);
    else 
        temp = std::make_shared<CSparse> (left.m_m, left.m_n);
    float value;
    for (int i = 0; i < left.m_m; i++ ){
        for (int j = 0; j < left.m_n; j++ ) {
            value = left.GetValue (i,j) + right.GetValue (i,j);
            temp->SetValue (value, i, j);
        }
    }
    return (temp);
}
//-------------------------------------------------------------
std::shared_ptr<MatrixToken> OperatorToken::Calculate 
(MPtr right, MPtr left, CMemory & matrices) {
    /*
        returns shared pointer on CMatrix of product of 2 matrices / variables
        - for variables finds value first
    */
    if (op == '+') return std::make_shared<MatrixToken> (Sum (*left, *right));
    if (op == '-') return std::make_shared<MatrixToken> (Sub (*left, *right));
    if (op == '*') return std::make_shared<MatrixToken> (Mult(*left, *right));
    
    throw ("");
}
//-------------------------------------------------------------
MPtr OperatorToken::Sub (const CMatrix & left, const CMatrix & right) {
    if (!left.is_matrix) {
        if (!right.is_matrix) {
            return std::make_shared<CDense> (left.GetValue(0,0) - right.GetValue(0,0));
        }
        else throw WrongFormat("cannot subtract by number\n");
    }
    if (!right.is_matrix) throw WrongFormat("cannot sum by number\n");
    if (left.m_m != right.m_m || left.m_n != right.m_n) throw WrongDimensions();  
    std::shared_ptr<CMatrix> temp;
    if ( ProductShouldBeDense (left, right, '-') )
        temp = std::make_shared<CDense>  (left.m_m, left.m_n);
    else 
        temp = std::make_shared<CSparse> (left.m_m, left.m_n);
    float value;
    for (int i = 0; i < left.m_m; i++ ){
        for (int j = 0; j < left.m_n; j++ ) {
            value = left.GetValue (i,j) - right.GetValue (i,j);
            temp->SetValue (value, i, j);
        }
    }
    return temp;
}
// //----------------------------------------------------------------------------
MPtr OperatorToken::Mult (const CMatrix & left, const CMatrix & right) {
    if (!left.is_matrix) {
        if (!right.is_matrix) {
            return std::make_shared<CDense> (left.GetValue(0,0) * right.GetValue(0,0));
        }
        else {
            MPtr temp;
            if ( dynamic_cast<const CDense *>(&right) ) 
                 temp = std::make_shared<CDense>  (right.m_m, right.m_n);
            else temp = std::make_shared<CSparse> (right.m_m, right.m_n);
            for (int i = 0; i < right.m_m; i ++) {
                for (int j = 0; j < right.m_n; j++) {
                    temp->SetValue (left.GetValue(0,0) * right.GetValue(i,j), i , j);
                }
            }
            return temp;
        }
    }
    if (!right.is_matrix) throw WrongFormat ("cannot multiply by number from right\n");
    if (left.m_n != right.m_m) throw WrongDimensions();  
    std::shared_ptr<CMatrix> temp;
    if ( ProductShouldBeDense (left, right, '*') )
        temp = std::make_shared<CDense>  (left.m_m, right.m_n);
    else 
        temp = std::make_shared<CSparse> (left.m_m, right.m_n);
    float value;
    for (int i = 0; i < left.m_m; i++ ){
        for (int j = 0; j < right.m_n; j++ ) {
            value = 0;
            for (int k = 0; k < left.m_n; k++) {
                value += left.GetValue(i,k) * right.GetValue(k,j);
            } 
            temp->SetValue (value, i,j);
        }
    }
    return temp;
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
std::string * Variable::GetName  () {
    return &varName;
}
//-------------------------------------------------------------
void Variable::Print (std::ostream & out) const {
    out << varName;
}
//=============================================================