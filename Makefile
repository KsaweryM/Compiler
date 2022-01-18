make: clean
	rm -rf build
	mkdir  build
	bison -o build/parser_y.c -d src/parser.y
	flex -o build/scanner_l.c src/scanner.l
	g++ -o build/kompilator build/parser_y.c build/scanner_l.c -lm 

test: build	
	rm -f build/asembler
	./build/kompilator test/input.txt build/asembler
	./vm/vm build/asembler

clean:
	rm -rf build/
