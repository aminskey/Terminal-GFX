# Makefile for terminalgfx library with test
CC = cc
CFLAGS = -Wall -Wextra -O2 -Iinclude
SRC = src/terminalgfx.c src/tgfx_input.c src/tgfx_framebuffer.c src/tgfx_settings.c
OBJDIR = lib
SODIR = shared_lib
S_OBJ = $(SODIR)/terminalgfx.o $(SODIR)/tgfx_input.o $(SODIR)/tgfx_framebuffer.o $(SODIR)/tgfx_settings.o

OBJ = $(OBJDIR)/terminalgfx.o $(OBJDIR)/tgfx_input.o $(OBJDIR)/tgfx_framebuffer.o $(OBJDIR)/tgfx_settings.o

LIB = $(OBJDIR)/libterminalgfx.a
SLIB = $(SODIR)/libterminalgfx.so
ILIB = /usr/lib/libterminalgfx.so
CLIB = /usr/include/terminalgfx.h
HLIB = include/terminalgfx.h

# Test program
.PHONY: all clean

all: $(LIB)
shared: $(SLIB)
install: 
	sudo cp $(SLIB) $(ILIB)
	sudo cp $(HLIB) $(CLIB)
	echo "Changing permissions for lib"
	sudo chmod 0755 $(ILIB)
	sudo ldconfig

$(SLIB): $(S_OBJ)
	gcc -shared -o $@ $^

$(SODIR)/%.o: src/%.c
	@mkdir -p $(SODIR)
	$(CC) $(CFLAGS) -c -fpic $< -o $@

$(LIB): $(OBJ)
	ar rcs $@ $^

$(OBJDIR)/%.o: src/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(LIB) $(S_OBJ)

# examples
examples: $(LIB)
	$(CC) $(CFLAGS) -g examples/colourful.c -L$(OBJDIR) -lterminalgfx -lm -o bin/colourful
	$(CC) $(CFLAGS) -g examples/hello.c -L$(OBJDIR) -lterminalgfx -lm -o bin/hello
	$(CC) $(CFLAGS) -g examples/anim_example.c examples/llama.c -L$(OBJDIR) -lterminalgfx -lm -o bin/animatedLlama
