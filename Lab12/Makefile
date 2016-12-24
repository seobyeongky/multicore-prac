CXX = g++

LIBNAME = -lpthread -lrt
CXXFLAGS = -std=c++11 -g -W -Wall

all: nvm0lfqueue

nvm0lfqueue: ./test_lfqueue.cc ./lfqueue.cc
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBNAME)

clean:
	rm nvm0lfqueue
