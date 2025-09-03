.PHONY: all build

all: build


build: 
	bison --header=build/parser.tab.h -o build/parser.tab.c misc/parser.y 
	flex -o build/lex.yy.c misc/lex.l 
	gcc -o build/asembler src/main.c build/lex.yy.c build/parser.tab.c -lfl

run: build
	./build/asembler

clean:
	rm -rf build
