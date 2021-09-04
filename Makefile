# Jalcore1 Emulator makefile.
CXX := g++
CXXFLAGS := -std=c++17 -O3 #-Wall #-fsanitize=address -fno-omit-frame-pointer -g3
INCLUDES := -Iinclude

SRC_DIR := ./src/parsergen
OBJ_DIR := ./build/obj
BIN_DIR := ./bin
BIN_EXT := .exe

SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES)) # %.cpp -> %.o
FMT_OBJ := $(OBJ_DIR)/format.o

PARSERGEN_NAME := parsergen-cpp
PARSERGEN := $(BIN_DIR)/$(PARSERGEN_NAME)

ifeq ($(OS),Windows_NT) # windows like
	RM := del /F /Q
	FixPath = $(subst /,\,$1)
else # unix like
	RM := rm -f
	FixPath = $1
endif

.DEFAULT_GOAL := example

libparsergen: $(OBJ_FILES)
	ar rcs bin/libparsergen.a $(OBJ_FILES) $(FMT_OBJ)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) -c $< -o $@

$(PARSERGEN): libparsergen
	$(CXX) $(INCLUDES) $(CXXFLAGS) ./src/main.cpp -L$(BIN_DIR) -lparsergen -o $(BIN_DIR)/$(PARSERGEN_NAME)$(BIN_EXT)

fmt:
	$(CXX) $(INCLUDES) $(CXXFLAGS) -c src/format.cc -o $(FMT_OBJ)


# meta
bootstrap: libparsergen
	$(CXX) $(CXXFLAGS) $(INCLUDES) examples/bootstrap_parser.cpp -L$(BIN_DIR) -lparsergen -I. -o $(BIN_DIR)/$@$(BIN_EXT)

metagrammar: libparsergen $(PARSERGEN)
	$(PARSERGEN) $(SRC_DIR)/metagrammar.gram -o include/parsergen/grammar_parser.hpp

# Calculator
calc: libparsergen $(PARSERGEN)
	$(PARSERGEN) examples/calc/calc.gram -o examples/calc/calc.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) examples/calc/main.cpp -L$(BIN_DIR) -lparsergen -I. -o $(BIN_DIR)/$@$(BIN_EXT)

# JSON parser
json: libparsergen $(PARSERGEN)
	$(PARSERGEN) examples/json/json.gram -o examples/json/json_parser.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) examples/json/main.cpp -L$(BIN_DIR) -lparsergen -I. -o $(BIN_DIR)/$@$(BIN_EXT)

test: libparsergen
	$(CXX) $(CXXFLAGS) $(INCLUDES) -Itests tests/main.cpp -L$(BIN_DIR) -lparsergen -I. -o $(BIN_DIR)/$@$(BIN_EXT)

clean:
	$(RM) $(call FixPath,$(OBJ_DIR)/*.o)
	$(RM) $(call FixPath,$(BIN_DIR)/*.a)
	$(RM) $(call FixPath,$(BIN_DIR)/*$(BIN_EXT))