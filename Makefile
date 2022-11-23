CODEDIR=src
INCDIR=src
OUTDIR=out

PACK = xfedor14

IS_IT_OK_DIR = is_it_ok_test
IS_IT_OK_SCRIPT = ./is_it_ok.sh

DOC_DIR = doc
DOC = dokumentace.pdf

EXECUTABLE=$(OUTDIR)/ifj22

OPT=-O0

PLATFORM=-m64

DEBUGFLAGS=-g3 -DVILE_DEBUG
#WARNINGS=-Wall -Wextra -pedantic
CFLAGS=-std=c11 -I$(INCDIR) $(OPT) $(PLATFORM) $(DEBUGFLAGS)

CFILES=$(wildcard $(CODEDIR)/*.c)
OBJ=$(patsubst $(CODEDIR)/%.c,$(OUTDIR)/%.o,$(CFILES))
HEADERS=$(wildcard $(INCDIR)/*.h)

COMPILE_BIN=$(CC) $(PLATFORM) $^ -o $(strip $@ $(LDFLAGS))

.PHONY: all clean
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJ) 
	$(COMPILE_BIN)

$(CFILES): Makefile
	@touch $@

$(OUTDIR)/%.o: $(CODEDIR)/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: pack
pack: clean $(PACK).tgz

$(PACK).tgz:
	tar -czf $@ $(INCDIR)/*.h $(CODEDIR)/*.c Makefile rozdeleni $(DOC)

.PHONY: clean_pack
clean_pack:
	rm -f $(PACK).tgz
	rm -rf $(PACK)

.PHONY: unpack
unpack:
	mkdir $(PACK)
	tar -xf $(PACK).tgz -C $(PACK)

.PHONY: is_it_ok
is_it_ok: $(PACK).tgz $(IS_IT_OK_SCRIPT)
	chmod +x $(IS_IT_OK_SCRIPT)
	$(IS_IT_OK_SCRIPT) $(PACK).tgz $(IS_IT_OK_DIR)


.PHONY: clean_is_it_ok
clean_is_it_ok:
	rm -rf $(IS_IT_OK_DIR)


.PHONY: clean
clean: clean_pack clean_is_it_ok
	rm -f $(EXECUTABLE) $(wildcard $(OUTDIR)/*.o)

# $(DOC):
# 	cd $(DOC_DIR) && make
# 	cp $(DOC_DIR)/$(DOC) $(DOC)


# .PHONY: clean_doc
# clean_doc:
# 	rm -f $(DOC)
# 	cd $(DOC_DIR) && make clean