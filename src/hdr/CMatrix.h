#pragma once

#include "CExceptions.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <iomanip>
#include <math.h>
#define MPtr std::shared_ptr<CMatrix>


/** function that compares left and right float values\n
 *  *also used by friend CCommand class  
 */
bool Compare (float left, float right);

/**
  CMatrix class:\n
  - has only 'pomocné?' matrix functions/operations\n
  - has 2 derived classes: CDense and CSparse\n
    which differ mainly in storing and getting matrix values
*/
class CMatrix {
  public:
    CMatrix (int m, int n, bool is_matrix);
    /**
     * pure virtual method that returns copy of the matrix
     */
    virtual CMatrix * Clone () const = 0;
    /**
     * return true if matrix is set
     */
    bool IsSet () const;
    /**
     * virtual method that returns pointer to transposed matrix\n
     * differently implemented by CDense and CSparse classes
     */
    virtual MPtr Transpose () const = 0;
    /**
     *  virtual destructor for abstract class
     */
    virtual ~CMatrix () {}
    /**
     *  method that reads matrix from input stream
     */
    void Read      (std::istream & in );
    void WriteToFile (const std::string & fileName);
    friend std::ostream & operator << 
     (std::ostream & out, const CMatrix & matrix);

  private:
  // functions used in main methods:
    virtual float GetValue (int m, int n) const = 0;
    virtual void  SetValue (float value, int m, int n) = 0;

    /// returns true if whole column starting with @starting_row is zero
    bool  ColumnIsZero    (int starting_row, int column) const;

    /// returns first row where value (row, @column) is not zero 
    int   ColumnNotZero   (int starting_row, int column) const;

    /// sets all values in @column under @starting_row to 0 
    float RowsMinus       (int starting_row, int column);

    /**
     * returns max value in column
     * @param[in] col: searched column
     * @param[in] starting_row: finds max value under this row 
     */ 
    int   FindMaxValInCol (int col, int starting_row) const;

    /// multiplies whole *row* by *factor* 
    void  MultRow         (int row, float factor);
    
    /// adds *i* row multiplied by *factor* to *j* row
    void  MultAdd         (int i, int j, float factor);

    /**
     * returns true if left half of matrix is identical\n
     * * used in inverse function
     */
    bool  leftHalfIsId    () const;
    /**
     *  virtual method that swaps rows _row1_ and _row2_\n
     *  * differently implemetned in CSparse and CDense classes
     */ 
    virtual void SwapRows (int row1, int row2) = 0;

    /// returns true if resulting matrix is Dense or Sparse
    friend bool ProductShouldBeDense (const CMatrix & left, const CMatrix & right, char op);

    /// returns how many values are not 0 for sum/subtraction
    friend int  CountSum             (const CMatrix & left, const CMatrix & right, char op);
    /// returns how many values are not 0 for multiplication
    friend int  CountMult            (const CMatrix & left, const CMatrix & right);
    friend bool MergeShouldBeDense   (const CMatrix & left, const CMatrix & right);
    friend bool SplitShouldBeDense   (const CMatrix & matrix, int m, int n, int c_m, int c_n);
    

  protected:
    friend class CCommands;
    friend class COperatorToken;
    int m_m;
    int m_n;
    ///flag if matrix is float value (which is represented as matrix)
    bool is_matrix;
};