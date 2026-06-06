CXX = g++
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
	rm -f $(OBJS) $(TARGET) out.obj

.PHONY: all clean
