CC=clang
CCFLAGS=-Wall -Werror -pedantic -c -g -std=c11
BIN=doto
OBJDIR=objs

OBJECTS=$(addprefix $(OBJDIR)/, main.o)

all: $(BIN)

$(BIN): $(OBJECTS)
	$(CC) $^ -o $@

$(OBJDIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $< $(CCFLAGS) -o $@

.PHONY: clean
clean:
	rm -rf $(OBJDIR) $(BIN)
