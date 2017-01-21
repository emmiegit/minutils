.PHONY: all force clean

SOURCES := $(wildcard *.c)
TARGETS := $(patsubst %.c,bin/%,$(SOURCES))

FLAGS   := -ansi -pedantic -Wall -Wextra -Wshadow -Wmissing-prototypes

all: CFLAGS += -Os
all: bin $(TARGETS)

debug: CFLAGS += -g
debug: bin $(TARGETS)

bin:
	@echo '[MKDIR] bin'
	@mkdir -p bin

bin/%: %.c
	@echo '[CC] $(@F)'
	@$(CC) $(FLAGS) $(CFLAGS) -o $(@F) $<
	@mv $(@F) -t bin

clean:
	@echo '[RMDIR] bin'
	@rm -rf bin

