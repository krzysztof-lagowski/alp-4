CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic -g

A1 = Aufgabe1
A1SRC = $(A1).c
A1OBJ = $(A1SRC:.c=.o)

TARGETS = $(A1)

all: $(TARGETS)

$(A1): $(A1OBJ)
	$(CC) $(CFLAGS) -o $@ $^

run_a1: $(A1)
	./$(A1)

clean:
	$(RM) -v $(TARGETS) *.o

.PHONY: clean all run_a1
