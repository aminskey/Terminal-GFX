# Makefile for terminalgfx library with test

CC = cc
CFLAGS = -Wall -Wextra -O2 -Iinclude
SRC = src/terminalgfx.c src/tgfx_input.c src/tgfx_framebuffer.c src/tgfx_settings.c
OBJDIR = lib
OBJ = $(OBJDIR)/terminalgfx.o $(OBJDIR)/tgfx_input.o $(OBJDIR)/tgfx_framebuffer.o $(OBJDIR)/tgfx_settings.o
LIB = $(OBJDIR)/libterminalgfx.a

# Test program
TEST_SRC = hello.c
TEST_BIN = hello

.PHONY: all clean test

all: $(LIB)

$(LIB): $(OBJ)
	ar rcs $@ $^

$(OBJDIR)/%.o: src/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(LIB) $(TEST_BIN)

# Test target
test: $(LIB)
	@mkdir -p test
	$(CC) $(CFLAGS) $(TEST_SRC) -L$(OBJDIR) -lterminalgfx -o $(TEST_BIN)
