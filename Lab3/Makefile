CC = g++
CXXFLAGS = -g -Wall -std=c++11

SRCS := lock.cpp
OBJS := $(SRCS:.cpp=.o)
LIB = ./lib/ -lpthread

TARGET = bin/lock
$(TARGET): $(OBJS)
	mkdir -p bin
	$(CC) $(CXXFLAGS) -o $(TARGET) $(OBJS) -L$(LIB)

clean:
	rm $(BIN)$(TARGET) $(OBJS)

