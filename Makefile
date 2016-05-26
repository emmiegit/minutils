.PHONY: all force clean

SOURCES = $(wildcard *.c)
TARGETS = $(patsubst %.c,bin/%,$(SOURCES))

all: bin $(TARGETS)

bin:
	@echo '[MKDIR] bin'
	@mkdir -p bin

bin/%: %.c
	@echo '[CC] $(@F)'
	@gcc $< $(EXTRA_FLAGS) -o $(@F)
	@mv $(@F) -t bin

debug:
	@make EXTRA_FLAGS=-g

force: clean all

forcedebug: clean debug

clean:
	@echo '[RMDIR] bin'
	@rm -rf bin

