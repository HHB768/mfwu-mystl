 all: main.cc
	g++ -std=c++17 main.cc -I ./lib/ -I ./test/ -o app
 
clean:
	$(RM) app
