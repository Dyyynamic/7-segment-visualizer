TARGET = app

CXX = g++

CXXFLAGS = `pkg-config --cflags opencv4` -std=c++17 -Wall -Wextra -pedantic

SRCS = main.cpp

LIBS = -lsfml-graphics \
			 -lsfml-window \
			 -lsfml-system \
			 -lopencv_core \
			 -lopencv_videoio

main:
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(LIBS)

.PHONY: clean
clean:
	rm -f $(TARGET)
