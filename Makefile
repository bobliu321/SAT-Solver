#this will compile all file
all: 
	g++ -std=c++11 A5.cpp -fopenmp


TARGET= a.out

clean: 
	rm -f $(TARGET)

run:
	./a.out