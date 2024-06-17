# Nikhil Solanki

CXX=g++
CXXFLAGS=-std=c++17 -lpthread  
BIN=bin
OBJ=obj
SRC=main.cpp FileSearch.cpp
EXE=fileSearch

all: $(EXE)

# Compile and link the program
$(EXE): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(EXE) $(SRC)

# Clean compiled objects and executable
clean:
	rm -rf $(BIN)
	rm -rf $(EXE)
	rm -r -f bin
	rm -r -f obj
	rm -r -f html
	rm -r -f latex

.PHONY: doc
doc:
	doxygen config
	cd latex && make
