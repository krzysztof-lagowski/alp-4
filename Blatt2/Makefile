CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic -g

A1 = A1
A1SRC = $(A1).c
A1OBJ = $(A1SRC:.c=.o)

A2 = A2
A2SRC = $(A2).c
A2OBJ = $(A2SRC:.c=.o)

A3 = A3
A3SRC = $(A3).c
A3OBJ = $(A3SRC:.c=.o)

TARGETS = $(A1) $(A2) $(A3)

all: $(TARGETS)

$(A1): $(A1OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(A2): $(A2OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(A3): $(A3OBJ)
	$(CC) $(CFLAGS) -o $@ $^

run_a1: $(A1)
	./$(A1)

run_a2: $(A2)
	./$(A2)

run_a3: $(A3)
	./$(A3)

clean:
	$(RM) -v $(TARGETS) *.o

.PHONY: clean all run_a1 run_a2 run_a3
