.PHONY: all build

all: build


build: clean
	mkdir -p build
	bison --header=build/parser.tab.h -o build/parser.tab.c misc/parser.y 
	flex -o build/lex.yy.c misc/lex.l 
	g++ -std=c++20 -o build/asembler src/assembler/*.cpp build/lex.yy.c build/parser.tab.c -lfl

run: build
	./build/asembler ./misc/test.s

test: build
	@echo "Running assembler on all test files..."
	@find ./tests -type f | while read file; do \
		echo "Testing $$file"; \
		./build/asembler "$$file"; \
		if [ $$? -ne 0 ]; then \
			echo "Error: $$file failed"; \
			exit 1; \
		fi; \
	done

linker: run
	mkdir -p build
	g++ -std=c++20 -o build/linker ./src/linker/*.cpp
	./build/linker --hex --place=data@0x4000F000 --place=text@0x40000000 /home/milos/Projects/SS/resenje/misc/test.o /home/milos/Projects/SS/resenje/misc/test.o

testlinker: test
	mkdir -p build
	g++ -std=c++20 -o build/linker ./src/linker/*.cpp
	./build/linker --hex --relocatable --place=my_code@0x40000000 --place=math@0xF0000000 ./tests/nivo-a/*.o

clean:
	rm -rf build
	rm -rf ./misc/*.o
	rm -rf ./tests/nivo-a/*.o
	rm -rf ./tests/nivo-b/*.o
	rm -rf ./tests/nivo-c/*.o
