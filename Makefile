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
	rm -f timestamp.txt pub.txt pipe.txt archlinux-2023.03.01-x86_64.iso crab.mp4 pic1.jpg pic2.png

