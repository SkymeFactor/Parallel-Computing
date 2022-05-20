#------------------------------------------------------------
#		Variables
#------------------------------------------------------------

# Compiler options
CXX=clang++
CXX_FLAGS=-c -Wall -Werror -std=c++17
# CXX_FLAGS_DEBUG=-g -O0 -pg -DDEBUG
LD_FLAGS=-fopenmp

# Get $LIBRARY_PATH from env
ifdef LIBRARY_PATH
	LIB_PATH=-L $(LIBRARY_PATH)
endif

# Directories
SRC_DIR=src/
OBJ_DIR=obj/
BIN_DIR=build/

# Files
SOURCES=$(wildcard $(SRC_DIR)*.cpp)
INCLUDE=-I $(SRC_DIR)include
OBJECTS=$(SOURCES:$(SRC_DIR)%.cpp=$(OBJ_DIR)%.o)
BINARIES=$(BIN_DIR)omp_lab2.elf

# Utilities
RM=rm -rf
MD=mkdir -p

#------------------------------------------------------------
#		Targets
#------------------------------------------------------------

.PHONY: all clean make_dirs

all: make_dirs $(OBJECTS) $(BINARIES)

$(OBJECTS): $(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	$(CXX) $(CXX_FLAGS) $(CXX_FLAGS_DEBUG) $(INCLUDE) $< -o $@ $(LD_FLAGS)

$(BINARIES): $(OBJECTS)
	$(CXX) $(CXX_FLAGS_DEBUG) $(LIB_PATH) $^ -o $@ $(LD_FLAGS)

make_dirs:
	$(MD) $(OBJ_DIR)
	$(MD) $(BIN_DIR)

clean:
	$(RM) $(OBJECTS)
	$(RM) $(BINARIES)