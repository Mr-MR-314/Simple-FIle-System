# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11 -g -D_POSIX_C_SOURCE=200809L

# Source files and target executable
SRC = main.c
TARGET = filesystem

# Test script
TEST_SCRIPT = test_filesystem.sh

# Default target
all: $(TARGET)

# Compile the executable
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) -lz

# Run tests
test: $(TARGET)
	./$(TEST_SCRIPT)

# Run Valgrind for memory checks
valgrind: $(TARGET)
	valgrind --leak-check=full --track-origins=yes ./$(TARGET)

# Clean up compiled files and test artifacts
clean:
	rm -f $(TARGET)
	rm -rf test_dir test_dir.gz test_decompressed valgrind.log

# Rebuild everything
rebuild: clean all

# Phony targets
.PHONY: all test valgrind clean rebuild
