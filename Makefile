#Nicholas Gibbs G01482635
#CS 262, 001
#Project 1
CC = gcc
CFLAGS = -Wall -pedantic-errors -std=c99
TARGET = p1_ngibbs4_001
all: $(TARGET)
$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) $< -o $@
clean:
	rm -f $(TARGET)
