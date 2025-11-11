# Makefile for compressor library and test program
# For any changes, run 'make libcompile' and 'make libbundle' to rebuild the library
# What are library files? They are named _cp_*.* in this directory
# The program files containing main are named without underscore prefix

CC = gcc            # Compiler
CFLAGS = -Wall -O2  # Compiler flags
HEADS = _cp_*.h     # Header files
IMPLS = _cp_*.c     # Implementation files
OBJS = _cp_*.o      # Object files
LIB = libcplib.a    # Static library

test: test.o $(LIB) # Default target
	$(CC) $(CFLAGS) -o test test.o $(LIB)

cp32en: cp32en.o $(LIB) # 32-bit encoder program
	$(CC) $(CFLAGS) -o cp32en cp32en.o $(LIB)

libbundle: $(OBJS)
	ar rcs $(LIB) $(OBJS)

libcompile: $(HEADS) $(IMPLS)
	$(CC) $(CFLAGS) -c $(IMPLS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) myprogram