CXX = clang++
CXXFLAGS = -Wall -Wextra -std=c++11 -Iinclude

SRCS = main.cc assembler.cc
OBJS = $(SRCS:.cc=.o)
TARGET = assembler

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) out.obj test_main

.PHONY: all clean test

test_main: tests/test_main.cc assembler.o
	$(CXX) $(CXXFLAGS) -o test_main tests/test_main.cc assembler.o

test: test_main
	@echo ./test_main
	@./test_main
