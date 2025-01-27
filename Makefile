# Compiler and flags
CC = g++
CFLAGS = -Isrc/Include -Lsrc/lib -Wall -Wextra
LIBS = -lmingw32 -lSDL2main -lSDL2

# Source files
SRCS = main.c render.c simulation.c
OBJS = $(SRCS:.c=.o)

# Target executable
TARGET = main

# Default target
all: $(TARGET)

# Linking
$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

# Compilation for each .c file
%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

# Clean up generated files
clean:
	rm -f $(OBJS) $(TARGET)
