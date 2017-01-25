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
	@echo '[MKDIR] bin'
	@mkdir -p bin

bin/%: %.c
	@echo '[CC] $(@F)'
	@$(CC) $(FLAGS) $(CFLAGS) -o $@ $<

bin/%: %.s
	@echo '[AS] $(@F)'
	@$(CC) -s -nostdlib -o $@ $<

clean:
	@echo '[RMDIR] bin'
	@rm -rf bin

