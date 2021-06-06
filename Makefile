program=makuldan
GXX=g++
GFLAGS=-Wall -pedantic -g -fsanitize=address
all: calculator

calculator: compile run clear

compile: $(program)

run:
	./$(program)

$(program): lib/main.cpp src/CApplication.cpp src/CExpr.cpp src/CMatrix.cpp src/CDense.cpp src/CSparse.cpp  src/CToken.cpp src/MatrixFunctions.cpp src/CMatrixCommands.cpp
	$(GXX) $(GFLAGS) $^ -o $@ 

run:
	./$(program)

clear:
	rm $(program)