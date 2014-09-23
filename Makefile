CC := gcc
LD := $(cc)

OUTDIR := build
SRCDIR := src

CFLAGS  += -ggdb
LDFLAGS += -lm

TARGETS  = $(OUTDIR)/evrec
TARGETS  = $(OUTDIR)/evplay

.PHONY: all clean

all: $(TARGETS)

clean:
	rm -rf $(OUTDIR)

$(OUTDIR):
	mkdir -p $@

$(OUTDIR)/%.o: $(SRCDIR)/%.c | $(OUTDIR)
	$(CC) -c $(CFLAGS) -o $@ $^
