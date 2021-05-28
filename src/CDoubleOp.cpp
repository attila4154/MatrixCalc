#pragma once

#include "../lib/CDoubleOp.h"
#include <memory>

class CDoubleOp : public CNode {

    virtual char GetOperator () const = 0;

  private:
    std::shared_ptr<CNode>  m_left;
    std::shared_ptr<CNode> m_right;
};