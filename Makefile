CODEDIR=src
INCDIR=src
OUTDIR=out

BIN=$(OUTDIR)/ifj21
DEPLIST=$(OUTDIR)/dep.list

CC=gcc
OPT=-O4

PLATFORM=-m64

WARNINGS=-Wall -Wextra -pedantic
CFLAGS=-std=c11 -g3 $(WARNINGS) -I$(INCDIR) $(OPT) $(PLATFORM)
LDFLAGS=

CFILES=$(wildcard $(CODEDIR)/*.c)
OBJ=$(patsubst $(CODEDIR)/%.c,$(OUTDIR)/%.o,$(CFILES))

COMPILE_BIN=$(CC) $(PLATFORM) $^ -o $(strip $@ $(LDFLAGS))



.PHONY: all clean 
all: $(BIN) out/dep.list

$(BIN): $(OBJ)
	$(COMPILE_BIN)

$(CFILES): Makefile
	@touch $@

$(OUTDIR)/%.o: $(CODEDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

out/dep.list:
	$(CC) -MM $(CFILES) > out/dep.list

clean:
	rm -rf $(BIN) $(OBJ)

-include out/dep.list