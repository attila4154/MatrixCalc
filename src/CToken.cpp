#include "hdr/CToken.h"
//=================
//   CToken class:
//=================
        CToken::CToken   (int t) : type (t) {}
//-------------------------------------------------------------
int     CToken::GetType (void) const 
{
  return type;
} 
//-------------------------------------------------------------
std::string * CToken::GetName () {
    return nullptr;
}
//-------------------------------------------------------------
MPtr CToken::Value () 
{
    return nullptr;
}
//-------------------------------------------------------------
int CToken::Precedence () const {
    return 0;
}
//-------------------------------------------------------------
std::ostream & operator << (std::ostream & out, const CToken & t) { 
    t.Print(out); return out;
}
//-------------------------------------------------------------
//=================
//   Matrix class:
//=================
//-------------------------------------------------------------
CMatrixToken::CMatrixToken (std::shared_ptr<CMatrix> m) : 
        CToken (TokenType::Matrix), matrix (m) {}
//-------------------------------------------------------------
//            float number constructor
CMatrixToken::CMatrixToken (float number) : CToken (TokenType::Number) {
    matrix = std::make_shared<CDense>(number);
}
//-------------------------------------------------------------
CMatrixToken::CMatrixToken (CMatrix * m) : CToken (TokenType::Matrix),
                                 matrix (std::shared_ptr <CMatrix> (m) ) {}
//-------------------------------------------------------------
MPtr CMatrixToken::Value () {
    return matrix;
}
//-------------------------------------------------------------
void CMatrixToken::Print (std::ostream & out) const {
    out << *matrix.get();
}
//-------------------------------------------------------------
//====================
//    Operator class:
//====================
//-------------------------------------------------------------
COperatorToken::COperatorToken (char sign, int prec) :
    CToken (TokenType::Operator), op(sign), precedence(prec) {}
//-------------------------------------------------------------
void COperatorToken::Print (std::ostream & out) const {
    out << op;
}
//-------------------------------------------------------------
int COperatorToken::Precedence () const {
    return precedence;
}
//-------------------------------------------------------------
MPtr COperatorToken::Sum (const CMatrix & left, const CMatrix & right) {
    if (!left.is_matrix) {
        if (!right.is_matrix) {
            return (std::make_shared<CDense> (left.GetValue(0,0) + right.GetValue(0,0)));
        }
        else throw WrongFormat("cannot sum by number\n");
    }
    if (!right.is_matrix) throw WrongFormat("cannot sum by number\n");
    // std::cout << "left matrix is " left.m_m
    if (left.m_m != right.m_m || left.m_n != right.m_n) throw WrongDimensions ();  
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
std::shared_ptr<CMatrixToken> COperatorToken::Calculate 
(MPtr right, MPtr left, CMemory & matrices) {
    /*
        returns shared pointer on CMatrix of product of 2 matrices / variables
        - for variables finds value first
    */
    if (op == '+') return std::make_shared<CMatrixToken> (Sum (*left, *right));
    if (op == '-') return std::make_shared<CMatrixToken> (Sub (*left, *right));
    if (op == '*') return std::make_shared<CMatrixToken> (Mult(*left, *right));
    
    throw ("");
}
//-------------------------------------------------------------
MPtr COperatorToken::Sub (const CMatrix & left, const CMatrix & right) {
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
MPtr COperatorToken::Mult (const CMatrix & left, const CMatrix & right) {
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
CBracketsToken::CBracketsToken (char b) : 
    CToken    (b == '(' ? TokenType::LeftBracket : TokenType::RightBracket),
    isLeftBr  (b == '(' ? true : false) {}
//-------------------------------------------------------------
void CBracketsToken::Print (std::ostream & out) const {
    // won't be used because expressions in RPN notation do not have paranthesis 
    ;
}
//-------------------------------------------------------------
//===================
//   Variable class:
//===================
//-------------------------------------------------------------
CVariableToken::CVariableToken (const std::string & name) : CToken (TokenType::Variable), varName(name) {}
//-------------------------------------------------------------
std::string * CVariableToken::GetName  () {
    return &varName;
}
//-------------------------------------------------------------
void CVariableToken::Print (std::ostream & out) const {
    out << varName;
}
//=============================================================