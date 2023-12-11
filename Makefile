CC=gcc
CFLAGS=-I.

# Path to the source files
SRC_DIR=src/

# Source files
SRC=$(wildcard $(SRC_DIR)/*.c)

# Object files
OBJ=$(SRC:.c=.o)

# Executable
TARGET=download

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f $(OBJ) $(TARGET)
