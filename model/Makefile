C  = gcc
CXX = g++

CFLAGS   = -g -Wall  $(INCLUDES)
CXXFLAGS = -g -Wall $(INCLUDES)

LDFLAGS = -g

.PHONY: garbage

.PHONY: all

all: garbage

garbage: 
	gcc -Wall -o garbage garbage.c

# slist:
# 	gcc -Wall -o slist slist.c

clean:
	rm -f *.o *~ a.out core garbage