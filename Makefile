CC = gcc
# The -Iinclude flag tells the compiler to look in the include/ folder for .h files
CFLAGS = -Wall -Wextra -g -Iinclude

# Directory Definitions
SRC_DIR = src
INC_DIR = include
BIN_DIR = bin

# File Definitions
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:.c=.o)
EXEC = $(BIN_DIR)/tarsau

# Default target
all: $(BIN_DIR) $(EXEC)

# Ensure the bin directory exists before compiling
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Link the object files into the final executable inside the bin/ folder
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile each C file into an object file
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files (object files, binaries, and test archives)
clean:
	rm -rf $(SRC_DIR)/*.o $(BIN_DIR) *.sau