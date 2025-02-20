 all: main.cc
	g++ -std=c++17 main.cc ./lib/webserver/*.cpp -I ./lib/ -I ./test/ -I ./lib/safe_ringbuffer -o app
 
clean:
	$(RM) app ./log/*.log ./log/logger.out
