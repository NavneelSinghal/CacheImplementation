all:
	g++ -o main main.cpp
verbose:
	g++ -DVERBOSE -o main main.cpp
mem:
	g++ -DMEM -o main main.cpp
memverbose:
	g++ -DMEM -DVERBOSE -o main main.cpp
run:
	@./main ./tests/in > out
matrix:
	@./main ./tests/matrix > out
matrix2:
	@./main ./tests/matrix2 > out
random:
	@./main ./tests/random > out


