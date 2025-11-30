# Compiler
CC = g++
CFLAGS = -std=c++11 -g -Wall

# Detect OS to handle Mac vs Linux paths
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
	# Mac (Homebrew) Paths
	INCLUDES = -I/opt/homebrew/include
	LIBRARIES = -L/opt/homebrew/lib
	LIBS = -lsfml-graphics -lsfml-window -lsfml-system
endif

ifeq ($(UNAME_S),Linux)
	# Linux Standard Paths
	INCLUDES = 
	LIBRARIES = 
	LIBS = -lsfml-graphics -lsfml-window -lsfml-system
endif

TARGET = mandelbrot
SRCS = main.cpp ComplexPlane.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(OBJS) $(LIBRARIES) $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
