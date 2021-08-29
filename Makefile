# Jalcore1 Emulator makefile.
CXX := g++
CXXFLAGS := -std=c++17 -O3
INCLUDES := -Iinclude

SRC_DIR := ./src/parsergen
OBJ_DIR := ./build/obj
BIN_DIR := ./bin

SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
FMT_OBJ := $(OBJ_DIR)/format.o


.DEFAULT_GOAL := example

libparsergen: $(OBJ_FILES) lib_fmt
	ar rcs bin/libparsergen.a $(OBJ_FILES) $(FMT_OBJ)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) -c $< -o $@

lib_fmt:
	$(CXX) $(INCLUDES) $(CXXFLAGS) -c src/format.cc -o $(OBJ_DIR)/format.o


# meta
bootstrap: libparsergen
	$(CXX) $(CXXFLAGS) $(INCLUDES) examples/bootstrap_parser.cpp -Lbin -lparsergen -I. -o $(BIN_DIR)/$@.exe

metagrammar: libparsergen
	$(CXX) $(CXXFLAGS) $(INCLUDES) examples/regen_parser.cpp -Lbin -lparsergen -I. -o $(BIN_DIR)/$@.exe

# Calculator
test_parser: libparsergen
	$(CXX) $(CXXFLAGS) $(INCLUDES) examples/test_parser.cpp -Lbin -lparsergen -I. -o $(BIN_DIR)/$@.exe

example: libparsergen
	$(CXX) $(CXXFLAGS) $(INCLUDES) examples/test/main.cpp -Lbin -lparsergen -I. -o $(BIN_DIR)/$@.exe


# JSON parser tests
json_parser: libparsergen
	$(CXX) $(CXXFLAGS) $(INCLUDES) examples/json/generate_parser.cpp -Lbin -lparsergen -I. -o $(BIN_DIR)/$@.exe

json_main: libparsergen
	$(CXX) $(CXXFLAGS) $(INCLUDES) examples/json/main.cpp -Lbin -lparsergen -I. -o $(BIN_DIR)/$@.exe