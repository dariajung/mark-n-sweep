C  = gcc
CXX = g++

CFLAGS   = -g -Wall  $(INCLUDES)
CXXFLAGS = -g -Wall $(INCLUDES)

LDFLAGS = -g

.PHONY: default
default: garbage

# header dependency
garbage.o: garbage.c

.PHONY: clean
clean:
	rm -f *.o *~ a.out core garbage

.PHONY: all
all: clean default
