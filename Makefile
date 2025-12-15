CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -Itests/unity -g

SRC = src/sl_string.c

TEST_SRC = tests/test_sl_string.c tests/unity/unity.c
TEST_EXE = tests/test_sl_string

EXP_SRC = tests/experiments/exp.c
EXP_EXE = tests/experiments/exp

all: $(TEST_EXE)

# official build tests
$(TEST_EXE): $(SRC) $(TEST_SRC)
	$(CC) $(CFLAGS) $(SRC) $(TEST_SRC) -o $@

run: $(TEST_EXE)
	./$(TEST_EXE)

valgrind: $(TEST_EXE)
	valgrind --leak-check=full --track-origins=yes ./$(TEST_EXE)

experiments: $(EXP_EXE)

$(EXP_EXE): $(SRC) $(EXP_SRC)
	$(CC) $(CFLAGS) $(SRC) $(EXP_SRC) -o $@

run-exp: $(EXP_EXE)
	./$(EXP_EXE)

valgrind-exp: $(EXP_EXE)
	valgrind --leak-check=full --track-origins=yes ./$(EXP_EXE)

clean:
	rm -f $(TEST_EXE) $(EXP_EXE)