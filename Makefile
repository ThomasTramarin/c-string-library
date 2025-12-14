# run tests

CFLAGS = -Wall -Wextra -Iinclude -g

# src files
SRC = src/sl_string.c
TEST = tests/test_sl_string.c

# out file
TEST_EXE = tests/test_sl_string

all: $(TEST_EXE)

$(TEST_EXE): $(SRC) $(TEST)
	$(CC) $(CFLAGS) $^ -o $@

run: $(TEST_EXE)
	./$(TEST_EXE)

clean:
	rm -f $(TEST_EXE)