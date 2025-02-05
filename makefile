 all: main.cc
	g++ -std=c++17 main.cc ./lib/webserver/*.cpp -I ./lib/ -I ./test/ -o app
 
clean:
	$(RM) app
