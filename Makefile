CC = gcc
CFLAGS = -Wall -g
SRC = src/
INCLUDE = include/
BIN = bin/
.PHONY: run
run: $(BIN)/download
$(BIN)/download: $(SRC)/main.c
	$(CC) $(CFLAGS) -o $@ $^
.PHONY: clean
clean:
	rm -rf $(BIN)/download
