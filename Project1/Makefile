# Compiler and Compile options.
CC = g++
CXXFLAGS = -g -Wall -std=c++11 -O3

# Macros specifying path for compile.
SRCS := $(wildcard src/*.cpp)
OBJS := $(SRCS:.cpp=.o)
INC = ./include/
LIB = ./lib/ -lpthread

# Pre-Processor.
CPPFLAGS += -I$(INC)

# Compile command.
TARGET = bin/project1
$(TARGET): $(OBJS)
	$(CC) $(CXXFLAGS) $(CPPFLAGS) -o $(TARGET) $(OBJS) -L$(LIB)

# Delete binary & object files.
clean:
	rm $(BIN)$(TARGET) $(OBJS)
