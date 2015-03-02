BIN=marksweep

.PHONY : clean 

$(BIN) : ms.c
	gcc ms.c -o $(BIN)

clean :
	rm -f $(BIN) *~

run : $(BIN)
	valgrind  --leak-check=yes $(BIN)