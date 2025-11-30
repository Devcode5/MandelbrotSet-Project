CXX = g++
CXXFLAGS = -Wall -std=c++17
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system

TARGET = mandelbrot
OBJS = main.o ComplexPlane.o

$(TARGET): $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LDFLAGS)

main.o: main.cpp ComplexPlane.h
	$(CXX) $(CXXFLAGS) -c main.cpp

ComplexPlane.o: ComplexPlane.cpp ComplexPlane.h
	$(CXX) $(CXXFLAGS) -c ComplexPlane.cpp

clean:
	rm -f $(TARGET) *.o
