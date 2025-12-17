# Makefile for compressor library and test program
# For any changes, run 'make libcompile' and 'make libbundle' or just 'make libs'
# What are library files? They are named _cp_*.* in this directory
# The program files containing main are named without underscore prefix

CC = gcc            # Compiler
CFLAGS = -Wall -O2  # Compiler flags
HEADS = _cp_*.h     # Header files
IMPLS = _cp_*.c     # Implementation files
OBJS = _cp_*.o      # Object files
LIB = libcplib.a    # Static library

ct: libs run # Compile and run tests

run: test    # Run the test program
	./test

test: test.o $(LIB) # Default target
	$(CC) $(CFLAGS) -o test test.o $(LIB)

cpobjen: cpobjen.o $(LIB) # Object encoder program
	$(CC) $(CFLAGS) -o cpobjen cpobjen.o $(LIB)

cp32en: cp32en.o $(LIB) # 32-bit encoder program
	$(CC) $(CFLAGS) -o cp32en cp32en.o $(LIB)

cp32de: cp32de.o $(LIB) # 32-bit decoder program
	$(CC) $(CFLAGS) -o cp32de cp32de.o $(LIB)

libbundle: $(OBJS)
	ar rcs $(LIB) $(OBJS)

libcompile: $(HEADS) $(IMPLS)
	$(CC) $(CFLAGS) -c $(IMPLS)

libs:
	$(CC) $(CFLAGS) -c $(IMPLS)
	ar rcs $(LIB) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) myprogram