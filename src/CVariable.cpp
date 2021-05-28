#pragma once 

#include "../lib/CVariable.h"
#include <string>
#include <memory>


//=====================================================
class CVariable : public CNode {
  public:
    CVariable (std::string & name, CMatrix * matrix);
    CVariable (CMatrix * matrix);

  private:
    //nemusi mit jmeno
    std::string m_name;
    std::shared_ptr<CMatrix> m_matrix_pointer;
};