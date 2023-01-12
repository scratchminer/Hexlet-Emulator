# Unix makefile for Hexlet

.PHONY: all clean

# Relevant directories
BINDIR = bin
SRCDIR = src
INCDIR = include
OBJDIR = obj
DRVDIR = drivers/sdl2

# Change this to the flags needed for the driver to compile/link
CFLAGS_DRIVER = $(shell sdl2-config --cflags)
LDFLAGS_DRIVER = $(shell sdl2-config --libs)

# DO NOT EDIT BELOW THIS LINE

# macOS arm64 check
ifeq ($(shell uname -mo),Darwin arm64)
CC = gcc-12
else
CC = gcc
endif

BINARY = $(BINDIR)/hexlet

SRC_CORE = $(wildcard $(SRCDIR)/*.c)
OBJ_CORE = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRC_CORE))

CFLAGS = -I $(INCDIR)
LDFLAGS = 

all: $(BINARY)

clean: $(BINDIR) $(OBJDIR)
	rm -rf $^
	mkdir $^ $(OBJDIR)/driver

OBJ_DRIVER = $(patsubst %.c, $(OBJDIR)/driver/%.o, $(notdir $(wildcard $(DRVDIR)/*.c)))

$(OBJ_DRIVER): $(OBJDIR)/driver/%.o: $(DRVDIR)/%.c
	$(CC) -c -o $@ $(CFLAGS_DRIVER) $(CFLAGS) $<

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c -o $@ $(CFLAGS) $<

$(BINARY): $(OBJ_CORE) $(OBJ_DRIVER)
	$(CC) -o $@ $(LDFLAGS_DRIVER) $(LDFLAGS) $^