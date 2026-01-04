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

.PHONY: all clean

all: $(LIB)

$(LIB): $(OBJ)
	ar rcs $@ $^

$(OBJDIR)/%.o: src/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(LIB) $(TEST_BIN)

# examples
examples: $(LIB)
	$(CC) $(CFLAGS) -g examples/colourful.c -L$(OBJDIR) -lterminalgfx -lm -o bin/colourful
	$(CC) $(CFLAGS) -g examples/hello.c -L$(OBJDIR) -lterminalgfx -lm -o bin/hello
	$(CC) $(CFLAGS) -g examples/anim_example.c examples/llama.c -L$(OBJDIR) -lterminalgfx -lm -o bin/animatedLlama
