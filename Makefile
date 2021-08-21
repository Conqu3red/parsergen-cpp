# Jalcore1 Emulator makefile.
CC=g++
BFLAGS= -std=c++17# -fsanitize=address -static-libasan -g
CFLAGS= -Iinclude

TARGET= 
PARSERGEN_SRC=src/parsergen/*.cpp
FMT_SRC=src/format.cc

.DEFAULT_GOAL := example

example: $(SRC)
	$(CC) $(BFLAGS) examples/test/main.cpp -I. -o example.exe $(SRC) $(PARSERGEN_SRC) $(FMT_SRC) $(CFLAGS)

bootstrap: $(SRC)
	$(CC) $(BFLAGS) examples/bootstrap_parser.cpp $(SRC) $(PARSERGEN_SRC) $(FMT_SRC) $(CFLAGS)

metagrammar: $(SRC)
	$(CC) $(BFLAGS) examples/regen_parser.cpp $(SRC) $(PARSERGEN_SRC) $(FMT_SRC) $(CFLAGS)

test_parser: $(SRC)
	$(CC) $(BFLAGS) examples/test_parser.cpp $(SRC) $(PARSERGEN_SRC) $(FMT_SRC) $(CFLAGS)