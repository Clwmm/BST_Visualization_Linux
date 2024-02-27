CC = g++
CFLAGS = -lsfml-graphics -lsfml-window -lsfml-system -lGL

all: main.x

main.x: main.cpp
	$(CC) -std=c++11 main.cpp -o main.out include/*.cpp $(CFLAGS)

run:
	./main.out

clean:
	rm -f *.out

install:
	sudo apt-get install libsfml-dev
