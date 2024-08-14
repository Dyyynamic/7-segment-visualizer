#include <SFML/Graphics.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>

// Configuration
// 44 x 20 recommended for 4:3
// 56 x 20 recommended for 16:9
const int DIGITS_X{44};
const int DIGITS_Y{20};
const double SCALE{1.5};
const std::string VIDEO_FILE{"bad_apple.mp4"};

// Colors
const sf::Color BG(0, 0, 0);
const sf::Color ON(255, 0, 0);
const sf::Color OFF(31, 0, 0);

const int DIGIT_WIDTH{20};
const int DIGIT_HEIGHT{32};
const int WIDTH{static_cast<int>(DIGITS_X * DIGIT_WIDTH * SCALE + 4 * SCALE)};
const int HEIGHT{static_cast<int>(DIGITS_Y * DIGIT_HEIGHT * SCALE + 4 * SCALE)};

sf::Vector2f segments[7][6]{
    {sf::Vector2f(5, 0), sf::Vector2f(3, 2), sf::Vector2f(5, 4), sf::Vector2f(11, 4), sf::Vector2f(13, 2), sf::Vector2f(11, 0)},
    {sf::Vector2f(0, 5), sf::Vector2f(2, 3), sf::Vector2f(4, 5), sf::Vector2f(4, 11), sf::Vector2f(2, 13), sf::Vector2f(0, 11)},
    {sf::Vector2f(12, 5), sf::Vector2f(14, 3), sf::Vector2f(16, 5), sf::Vector2f(16, 11), sf::Vector2f(14, 13), sf::Vector2f(12, 11)},
    {sf::Vector2f(5, 12), sf::Vector2f(3, 14), sf::Vector2f(5, 16), sf::Vector2f(11, 16), sf::Vector2f(13, 14), sf::Vector2f(11, 12)},
    {sf::Vector2f(0, 17), sf::Vector2f(2, 15), sf::Vector2f(4, 17), sf::Vector2f(4, 23), sf::Vector2f(2, 25), sf::Vector2f(0, 23)},
    {sf::Vector2f(12, 17), sf::Vector2f(14, 15), sf::Vector2f(16, 17), sf::Vector2f(16, 23), sf::Vector2f(14, 25), sf::Vector2f(12, 23)},
    {sf::Vector2f(5, 24), sf::Vector2f(3, 26), sf::Vector2f(5, 28), sf::Vector2f(11, 28), sf::Vector2f(13, 26), sf::Vector2f(11, 24)}};

int main()
{
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "7 Segment Visualizer");

    cv::VideoCapture video(VIDEO_FILE);

    if (!video.isOpened())
    {
        std::cerr << "Error: Could not open video." << std::endl;
        return -1;
    }

    cv::Mat frame;
    const double videoWidth = video.get(cv::CAP_PROP_FRAME_WIDTH);
    const double videoHeight = video.get(cv::CAP_PROP_FRAME_HEIGHT);
    const double videoFramerate = video.get(cv::CAP_PROP_FPS);

    sf::Clock clock;
    const double frameTime{1 / videoFramerate};

    for (int i{0}; i < 7; i++)
    {
        for (int j{0}; j < 6; j++)
        {
            segments[i][j] = sf::Vector2f(segments[i][j].x * SCALE, segments[i][j].y * SCALE);
        }
    }

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();

        video >> frame;
        if (frame.empty())
        {
            break;
        }

        for (int x{0}; x < DIGITS_X; x++)
        {
            for (int y{0}; y < DIGITS_Y; y++)
            {
                for (const auto &segment : segments)
                {
                    sf::Vector2f segmentMiddle{0, 0};
                    sf::ConvexShape segmentShape;
                    segmentShape.setPointCount(6);

                    for (int i{0}; i < 6; i++)
                    {
                        const sf::Vector2f pos = sf::Vector2f(
                            segment[i].x + x * DIGIT_WIDTH * SCALE + 4 * SCALE,
                            segment[i].y + y * DIGIT_HEIGHT * SCALE + 4 * SCALE);

                        segmentMiddle.x += pos.x;
                        segmentMiddle.y += pos.y;

                        segmentShape.setPoint(i, pos);
                    }

                    segmentMiddle.x /= 6;
                    segmentMiddle.y /= 6;

                    const int pxX = (segmentMiddle.x / WIDTH) * videoWidth;
                    const int pxY = (segmentMiddle.y / HEIGHT) * videoHeight;

                    const cv::Vec3b pxValue = frame.at<cv::Vec3b>(pxY, pxX);
                    const double average = (pxValue[0] + pxValue[1] + pxValue[2]) / 3.0;

                    segmentShape.setFillColor(average > 127.0 ? ON : OFF);
                    window.draw(segmentShape);
                }
            }
        }

        window.display();

        const sf::Time elapsed = clock.getElapsedTime();
        const sf::Time sleepTime = sf::seconds(frameTime) - elapsed;

        if (sleepTime > sf::Time::Zero)
        {
            sf::sleep(sleepTime);
        }

        clock.restart();
    }

    return 0;
}
