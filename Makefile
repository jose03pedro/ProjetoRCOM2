CC = gcc
CFLAGS = -Wall -g
SRC = src/
INCLUDE = include/
BIN = bin/
.PHONY: run
run: $(BIN)/download
$(BIN)/download: $(SRC)/clientTCP.c $(SRC)/getip.c
	$(CC) $(CFLAGS) -o $@ $^
.PHONY: clean
clean:
	rm -rf $(BIN)/download
