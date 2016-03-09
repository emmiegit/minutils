.PHONY: all force clean

SOURCES = $(wildcard *.c)
TARGETS = $(patsubst %.c,bin/%,$(SOURCES))

all: bin $(TARGETS)

bin:
	@echo '[MKDIR] bin'
	@mkdir -p bin

bin/%: %.c
	@echo '[CC] $(@F)'
	@gcc $< -o $(@F)
	@mv $(@F) -t bin

force: clean all

clean:
	@echo '[RMDIR] bin'
	@rm -rf bin

