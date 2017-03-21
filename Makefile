.PHONY: all force clean

C_SOURCES := $(wildcard *.c)
C_TARGETS := $(patsubst %.c,bin/%,$(C_SOURCES))

S_SOURCES := $(wildcard *.s)
S_TARGETS := $(patsubst %.s,bin/%,$(S_SOURCES))

TARGETS   := $(C_TARGETS) $(S_TARGETS)

FLAGS     := -ansi -pedantic -Wall -Wextra -Wshadow -Wmissing-prototypes

all: CFLAGS += -Os
all: bin $(TARGETS)

debug: CFLAGS += -g
debug: bin $(TARGETS)

bin:
	@echo '[MD] bin'
	@mkdir -p bin

bin/%: %.c
	@if [[ ! -L $< ]]; then \
		echo '[CC] $(@F)'; \
		$(CC) $(FLAGS) $(CFLAGS) -o $@ $<; \
	else \
		echo '[LN] $(@F)'; \
		source="$$(readlink '$<' | cut -d. -f1)"; \
		cd bin; \
		ln -sf $${source} $(@F); \
	fi

bin/%: %.s
	@echo '[AS] $(@F)'
	@$(CC) -s -nostdlib -o $@ $<

clean:
	@echo '[RM] bin'
	@rm -rf bin

