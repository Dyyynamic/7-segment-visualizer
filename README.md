# 7 Segment Visualizer in C++

Simple video on 7 segment display visualizer in C++

Inspired by @akumanatt on youtube: https://www.youtube.com/watch?v=qrSkWpVoKHM

## Example Video

https://github.com/user-attachments/assets/142488bc-43f8-41c0-a2d3-1749fcf95501

## Running

Install SFML and OpenCV:

```bash
# Debian/Ubuntu
sudo apt install libsfml-dev libopencv-dev
# Fedora
sudo dnf install SFML-devel opencv-devel
# Arch
sudo pacman -S sfml opencv
```

Change video file in main.cpp:
```c++
const std::string VIDEO_FILE{"your_file_here.mp4"};
```

Compile and run:

```bash
make && ./app
```
