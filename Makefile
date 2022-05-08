#------------------------------------------------------------
#		Variables
#------------------------------------------------------------

# Compiler options
CXX=clang++
CXX_FLAGS=-c -O3 -g -Wall -std=c++17 -DDEBUG
# CXX_FLAGS_DEBUG=-g -O0 -pg -DDEBUG
LD_FLAGS=-lOpenCL # -fsanitize=address,thread

# Directories
SRC_DIR=src/
OBJ_DIR=obj/
BIN_DIR=build/

# Files
SOURCES=$(shell find $(SRC_DIR) -type f -name '*.cpp')
INCLUDE=-I $(SRC_DIR)include
OBJECTS=$(SOURCES:$(SRC_DIR)%.cpp=%.o)
BINARIES=$(BIN_DIR)cl_lab1.elf

# Utilities
RM=rm -rf
MD=mkdir -p

#------------------------------------------------------------
#		Targets
#------------------------------------------------------------

.PHONY: all clean make_dirs

all: make_dirs $(OBJECTS) $(BINARIES)

$(OBJECTS): %.o: $(SRC_DIR)%.cpp
	@if [ ! -d "$(OBJ_DIR)$(@D)" ]; then mkdir -p $(OBJ_DIR)$(@D); fi
	$(CXX) $(CXX_FLAGS) $(CXX_FLAGS_DEBUG) $(INCLUDE) $< -o $(OBJ_DIR)$@

$(BINARIES): $(OBJECTS)
	$(CXX) $(CXX_FLAGS_DEBUG) $(addprefix $(OBJ_DIR),$(filter $^%,$(OBJECTS))) -o $@ $(LD_FLAGS)

make_dirs:
	$(MD) $(OBJ_DIR)
	$(MD) $(BIN_DIR)

clean:
	$(RM) $(addprefix $(OBJ_DIR),$(OBJECTS))
	$(RM) $(BINARIES)