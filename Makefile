FLAGS = -W -O3

make: clean
	mkdir -p build
	bison -o build/parser_y.c -d src/parser.y
	flex -o build/scanner_l.c src/scanner.l
	g++ $(FLAGS) -o build/compiler build/parser_y.c build/scanner_l.c -lm 
	cat test/input.txt | ./build/compiler > build/asembler
	./vm/vm build/asembler
exp1:
	mkdir -p build
	g++ -o build/ex1 experiment/experiment1.c
	./build/ex1 > build/output1.txt
	./vm/vm build/output1.txt
exp2:
	mkdir -p build	
	g++ -o build/ex2 experiment/experiment2.c
	./build/ex2 > build/output2.txt
	./vm/vm build/output2.txt
clean:
	rm -rf build/
