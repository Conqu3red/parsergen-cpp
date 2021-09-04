# Jalcore1 Emulator makefile.
CXX := g++
CXXFLAGS := -std=c++17 -O3 #-Wall #-fsanitize=address -fno-omit-frame-pointer -g3
INCLUDES := -Iinclude

SRC_DIR := ./src/parsergen
OBJ_DIR := ./build/obj
BIN_DIR := ./bin

SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES)) # %.cpp -> %.o

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

parsergen-cpp: libparsergen
	$(CXX) $(INCLUDES) $(CXXFLAGS) ./src/main.cpp -Lbin -lparsergen -o $(BIN_DIR)/$@.exe

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) -c $< -o $@

fmt:
	$(CXX) $(INCLUDES) $(CXXFLAGS) -c src/format.cc -o $(OBJ_DIR)/format.o


# meta
bootstrap: libparsergen
	$(CXX) $(CXXFLAGS) $(INCLUDES) examples/bootstrap_parser.cpp -Lbin -lparsergen -I. -o $(BIN_DIR)/$@.exe

metagrammar: libparsergen
	$(CXX) $(CXXFLAGS) $(INCLUDES) examples/regen_parser.cpp -Lbin -lparsergen -I. -o $(BIN_DIR)/$@.exe

# Calculator
calc_parser: libparsergen
	$(CXX) $(CXXFLAGS) $(INCLUDES) examples/calc/regenerate.cpp -Lbin -lparsergen -I. -o $(BIN_DIR)/$@.exe

calc: libparsergen
	$(CXX) $(CXXFLAGS) $(INCLUDES) examples/calc/main.cpp -Lbin -lparsergen -I. -o $(BIN_DIR)/$@.exe


# JSON parser tests
json_parser: libparsergen
	$(CXX) $(CXXFLAGS) $(INCLUDES) examples/json/regenerate.cpp -Lbin -lparsergen -I. -o $(BIN_DIR)/$@.exe

json: libparsergen
	$(CXX) $(CXXFLAGS) $(INCLUDES) examples/json/main.cpp -Lbin -lparsergen -I. -o $(BIN_DIR)/$@.exe

test: libparsergen
	$(CXX) $(CXXFLAGS) $(INCLUDES) -Itests tests/main.cpp -Lbin -lparsergen -I. -o $(BIN_DIR)/$@.exe

clean:
	$(RM) $(call FixPath,$(OBJ_DIR)/*.o)
	$(RM) $(call FixPath,$(BIN_DIR)/*.a)
	$(RM) $(call FixPath,$(BIN_DIR)/*.exe)