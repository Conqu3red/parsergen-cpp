# Jalcore1 Emulator makefile.
CC=g++
BFLAGS= 
CFLAGS= -Iinclude

TARGET= 
PARSERGEN_SRC=src/parsergen/*.cpp
FMT_SRC=src/format.cc

.DEFAULT_GOAL := example

example: $(SRC)
	$(CC) $(BFLAGS) examples/test.cpp $(SRC) $(PARSERGEN_SRC) $(FMT_SRC) $(CFLAGS)