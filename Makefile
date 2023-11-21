CODEDIR=src
INCDIR=src
OUTDIR=.

LOGIN=xfedor14

IS_IT_OK_DIR=is_it_ok_test
IS_IT_OK_SCRIPT=./is_it_ok.sh

DOC_DIR=doc
DOC=dokumentace.pdf

EXEC=$(OUTDIR)/ifj22

#OPT=-O0

#PLATFORM=-m64

#DEBUGFLAGS=-g3 -DVILE_DEBUG
#WARNINGS=-Wall -Wextra #-pedantic
CC=gcc
CFLAGS=-std=c11 $(WARNINGS) $(OPT) $(PLATFORM) $(DEBUGFLAGS)

SRCS=$(CODEDIR)/main.c $(CODEDIR)/codegen.c $(CODEDIR)/debug.c $(CODEDIR)/expression.c \
	$(CODEDIR)/parser.c $(CODEDIR)/scanner.c $(CODEDIR)/string_t.c $(CODEDIR)/symtable.c
OBJS:=$(SRCS:c=o)
HDRS=builtins.h codegen.h debug.h errors.h expression.h internals.h \
	marcos.h parser.h scanner.h string_t.h symtable.h vector_t.h

.PHONY: all pack unpack is_it_ok clean
all: $(EXEC)

$(EXEC): $(OBJS) 
	$(CC) $(PLATFORM) $^ -o $@

%.o: %.c Makefile $(HDRS)
	$(CC) -c $< $(CFLAGS)

pack:
	tar -czf $@ $(HDRS) $(SRCS) Makefile rozdeleni $(DOC_DIR)/$(DOC)

unpack:
	mkdir $(LOGIN)
	tar -xf $(LOGIN).tgz -C $(LOGIN)

is_it_ok: $(LOGIN).tgz $(IS_IT_OK_SCRIPT)
	chmod +x $(IS_IT_OK_SCRIPT)
	$(IS_IT_OK_SCRIPT) $(LOGIN).tgz $(IS_IT_OK_DIR)

clean:
	rm -rf $(EXEC) $(OBJS) $(LOGIN).tgz $(LOGIN) $(IS_IT_OK_DIR)

