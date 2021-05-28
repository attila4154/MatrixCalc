cmake_minimum_required(VERSION 3.19)
project(makuldan)

set(CMAKE_CXX_STANDARD 14)

include_directories(lib)

add_executable(makuldan
        lib/CApplication.h
        lib/CDense.h
        lib/CDiv.h
        lib/CDoubleOp.h
        lib/CExpr.h
        lib/CMatrix.h
        lib/CMul.h
        lib/CNode.h
        lib/CSparse.h
        lib/CSub.h
        lib/CSum.h
        lib/CVariable.h
        lib/main.cpp
        src/CApplication.cpp
        src/CDense.cpp
        src/CDiv.cpp
        src/CDoubleOp.cpp
        src/CExpr.cpp
        src/CMatrix.cpp
        src/CMul.cpp
        src/CNode.cpp
        src/CSparse.cpp
        src/CSub.cpp
        src/CSum.cpp
        src/CVariable.cpp
        src/main.cpp lib/CMemory.h)
