.PHONY: all build

all: build


build: 
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

clean:
	rm -rf build
