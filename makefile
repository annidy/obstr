all: obdef
	

obdef: obdef.c obstr.c
	clang -o $@ obdef.c obstr.c

clean:
	rm  -f *.o obdef


.PHONY: all