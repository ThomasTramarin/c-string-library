# tests Makefile
CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -g
SRC = strlib.c
TEST = tests/test_strlib.c
OUT = tests/test_strlib

.PHONY: all clean test

all: $(OUT)

$(OUT): $(SRC) $(TEST)
	$(CC) $(CFLAGS) $(SRC) $(TEST) -o $(OUT)

test: $(OUT)
	./$(OUT)

clean:
	rm -f $(OUT)