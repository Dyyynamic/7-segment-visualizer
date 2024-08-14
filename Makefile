main:
	g++ main.cpp -o app -lsfml-graphics -lsfml-window -lsfml-system `pkg-config --cflags --libs opencv4`
