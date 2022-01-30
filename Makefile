CC=clang
PKGS=ncurses
CCFLAGS=-Wall -Werror -pedantic -c -g `pkg-config --cflags $(PKGS)` -std=c11
LIBS=`pkg-config --libs $(PKGS)`
BIN=doto
OBJDIR=objs

OBJECTS=$(addprefix $(OBJDIR)/, main.o)

all: $(BIN)

$(BIN): $(OBJECTS)
	$(CC) $^ $(LIBS) -o $@

$(OBJDIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $< $(CCFLAGS) -o $@

.PHONY: clean
clean:
	rm -rf $(OBJDIR) $(BIN)
