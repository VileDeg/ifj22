CODEDIR=src
INCDIR=src
OUTDIR=out

BIN=$(OUTDIR)/ifj22
DEPLIST=$(OUTDIR)/dep.list

CC=gcc
OPT=-O0

PLATFORM=-m64

WARNINGS=-Wall -Wextra -pedantic
CFLAGS=-std=c11 -g3 -I$(INCDIR) $(OPT) $(PLATFORM)
LDFLAGS=

CFILES=$(wildcard $(CODEDIR)/*.c)
OBJ=$(patsubst $(CODEDIR)/%.c,$(OUTDIR)/%.o,$(CFILES))
HEADERS=$(wildcard $(INCDIR)/*.h)

COMPILE_BIN=$(CC) $(PLATFORM) $^ -o $(strip $@ $(LDFLAGS))

.PHONY: all clean
all: $(BIN)

$(BIN): $(OBJ) 
	$(COMPILE_BIN)

$(CFILES): Makefile
	@touch $@

$(OUTDIR)/%.o: $(CODEDIR)/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# $(DEPLIST):
# 	gcc -MM $(CFILES) > $@

clean:
	rm -rf $(BIN) $(wildcard $(OUTDIR)/*.o)

#-include $(DEPLIST)