program=makuldan
CXX=g++
CFLAGS=-Wall -pedantic


SRCDIR=src
OBJDIR=tmp

_SRC = main.cpp CApplication.cpp CExpr.cpp CMatrix.cpp CDense.cpp CSparse.cpp  CToken.cpp MatrixFunctions.cpp CMatrixCommands.cpp
_OBJ = $(patsubst %.cpp,%.o,$(_SRC))
HDR = hdr/CApplication.h hdr/CExpr.h hdr/CMatrix.h hdr/CDense.h hdr/CSparse.h  hdr/CToken.h hdr/MatrixFunctions.h hdr/CMatrixCommands.h

SRC = $(patsubst %,$(SRCDIR)/%,$(_SRC))
OBJ = $(patsubst %,$(OBJDIR)/%,$(_OBJ))

all: compile doc run clean

doc: $(SRC) $(HDR) Doxyfile
	doxygen

compile: $(program)

run:
	./$(program)

$(program): $(OBJ)
	$(CXX) $^ -o $@ 

$(OBJDIR)/%.o : $(SRCDIR)/%.cpp $(HDR)
	@mkdir -p $(@D)
	$(CXX) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR)
	rm -f $(program)
	rm -rf doc