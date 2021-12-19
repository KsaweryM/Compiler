FLAGS = -W -O3

.PHONY = all clean cleanall

make: cleanall calc.y calc.l
	bison -o calc_y.c -d calc.y
	flex -o calc_l.c calc.l
	g++ $(FLAGS) -o calc calc_y.c calc_l.c -lm 
	cat input.txt | ./calc > output.txt
	./mw output.txt
number:
	g++ -o test test.c
	rm -f n
	./test > n
	./mw n

clean:
	rm -rf build/

cleanall: clean
	rm -f calc
