all: compile

compile: server.cpp run
	g++ -pthread server.cpp -o server.out
run: server.out
	./server.out

clean:
	rm *.out *.o
