all:
	g++ -o main main.cpp
verbose:
	g++ -DVERBOSE -o main main.cpp
run:
	@./main ./tests/in > out
matrix:
	@./main ./tests/matrix > out
