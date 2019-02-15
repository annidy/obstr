all: obdef
	

obdef: obdef.c obstr.c
	clang -g -o $@ obdef.c obstr.c

clean:
	rm  -f *.o obdef


.PHONY: all