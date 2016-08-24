.PHONY: all force clean

SOURCES := $(wildcard *.c)
TARGETS := $(patsubst %.c,bin/%,$(SOURCES))

FLAGS   := -ansi -Wall -Wextra -pedantic -Os -D_POSIX_C_SOURCE=200112 -D_XOPEN_SOURCE=500

all: bin $(TARGETS)

bin:
	@echo '[MKDIR] bin'
	@mkdir -p bin

bin/%: %.c
	@echo '[CC] $(@F)'
	@$(CC) $(FLAGS) $(CFLAGS) $(EXTRA_FLAGS) -o $(@F) $<
	@mv $(@F) -t bin

debug:
	@make EXTRA_FLAGS=-g

force: clean all

forcedebug: clean debug

clean:
	@echo '[RMDIR] bin'
	@rm -rf bin

