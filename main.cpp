#include <SFML/Graphics.hpp>
#include <array>
// #include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <tuple>
#include <vector>

// Configuration
// 44 x 20 recommended for 4:3
// 56 x 20 recommended for 16:9
const int DIGITS_X{44};
const int DIGITS_Y{20};
const int SPACING_X{4};
const int SPACING_Y{4};
const double SCALE{1.0};
const std::string VIDEO_FILE{"videos/bad_apple.mp4"};
const std::string COLOR_MODE{"BINARY"}; // BINARY, MONO, FULL

// Colors
const sf::Color BG(0, 0, 0);
const sf::Color ON(255, 0, 0);
const sf::Color OFF(31, 0, 0);

const int DIGIT_WIDTH{16 + SPACING_X};
const int DIGIT_HEIGHT{28 + SPACING_Y};
const int WIDTH{static_cast<int>((DIGITS_X * DIGIT_WIDTH + SPACING_X) * SCALE)};
const int HEIGHT{static_cast<int>((DIGITS_Y * DIGIT_HEIGHT + SPACING_Y) * SCALE)};

const int SEGMENT_ROWS{7};
const int SEGMENT_COLS{6};
std::array<std::array<sf::Vector2f, 6>, 7> segments{
    {{sf::Vector2f(5, 0), sf::Vector2f(3, 2), sf::Vector2f(5, 4), sf::Vector2f(11, 4),
      sf::Vector2f(13, 2), sf::Vector2f(11, 0)},
     {sf::Vector2f(0, 5), sf::Vector2f(2, 3), sf::Vector2f(4, 5), sf::Vector2f(4, 11),
      sf::Vector2f(2, 13), sf::Vector2f(0, 11)},
     {sf::Vector2f(12, 5), sf::Vector2f(14, 3), sf::Vector2f(16, 5), sf::Vector2f(16, 11),
      sf::Vector2f(14, 13), sf::Vector2f(12, 11)},
     {sf::Vector2f(5, 12), sf::Vector2f(3, 14), sf::Vector2f(5, 16), sf::Vector2f(11, 16),
      sf::Vector2f(13, 14), sf::Vector2f(11, 12)},
     {sf::Vector2f(0, 17), sf::Vector2f(2, 15), sf::Vector2f(4, 17), sf::Vector2f(4, 23),
      sf::Vector2f(2, 25), sf::Vector2f(0, 23)},
     {sf::Vector2f(12, 17), sf::Vector2f(14, 15), sf::Vector2f(16, 17), sf::Vector2f(16, 23),
      sf::Vector2f(14, 25), sf::Vector2f(12, 23)},
     {sf::Vector2f(5, 24), sf::Vector2f(3, 26), sf::Vector2f(5, 28), sf::Vector2f(11, 28),
      sf::Vector2f(13, 26), sf::Vector2f(11, 24)}}};

std::tuple<std::vector<sf::ConvexShape>, std::vector<sf::Vector2f>>
initializeSegments(std::array<std::array<sf::Vector2f, SEGMENT_COLS>, SEGMENT_ROWS> segments,
                   int videoWidth, int videoHeight)
{
    std::vector<sf::ConvexShape> shapes;
    std::vector<sf::Vector2f> videoCoordinates;

    for (int x{0}; x < DIGITS_X; x++)
    {
        for (int y{0}; y < DIGITS_Y; y++)
        {
            for (auto &segment : segments)
            {
                sf::Vector2f segmentMiddle{0, 0};
                sf::ConvexShape segmentShape;
                segmentShape.setPointCount(SEGMENT_COLS);

                for (int i{0}; i < SEGMENT_COLS; i++)
                {
                    const sf::Vector2f pos =
                        sf::Vector2f((segment[i].x + x * DIGIT_WIDTH + SPACING_X) * SCALE,
                                     (segment[i].y + y * DIGIT_HEIGHT + SPACING_Y) * SCALE);

                    segmentMiddle.x += pos.x;
                    segmentMiddle.y += pos.y;

                    segmentShape.setPoint(i, pos);
                }

                segmentMiddle.x /= SEGMENT_COLS;
                segmentMiddle.y /= SEGMENT_COLS;

                const sf::Vector2f videoCoordinate =
                    sf::Vector2f((segmentMiddle.x / WIDTH) * videoWidth,
                                 (segmentMiddle.y / HEIGHT) * videoHeight);

                shapes.push_back(segmentShape);
                videoCoordinates.push_back(videoCoordinate);
            }
        }
    }

    return std::make_tuple(shapes, videoCoordinates);
}

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

    auto [shapes, videoCoordinates] = initializeSegments(segments, videoWidth, videoHeight);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        window.clear(BG);

        video >> frame;
        if (frame.empty())
        {
            break;
        }

        for (int i{0}; i < shapes.size(); i++)
        {
            auto shape{shapes[i]};
            auto videoCoordinate{videoCoordinates[i]};

            const cv::Vec3b pxValue = frame.at<cv::Vec3b>(videoCoordinate.y, videoCoordinate.x);

            sf::Color color;
            if (COLOR_MODE == "BINARY")
            {
                color = (pxValue[2] + pxValue[1] + pxValue[0]) / 3.0 > 127.0 ? ON : OFF;
            }
            else if (COLOR_MODE == "MONO")
            {
                const double value{(pxValue[2] + pxValue[1] + pxValue[0]) / (255.0 * 3.0)};
                color = sf::Color((ON.r - OFF.r) * value + OFF.r, (ON.g - OFF.g) * value + OFF.g,
                                  (ON.b - OFF.b) * value + OFF.b);
            }
            else if (COLOR_MODE == "FULL")
            {
                color = sf::Color(pxValue[2], pxValue[1], pxValue[0]);
            }

            shape.setFillColor(color);
            window.draw(shape);
        }

        window.display();

        const sf::Time elapsed = clock.getElapsedTime();
        const sf::Time sleepTime = sf::seconds(frameTime) - elapsed;

        // For debugging
        // std::cout << sleepTime.asMilliseconds() << std::endl;

        if (sleepTime > sf::Time::Zero)
        {
            sf::sleep(sleepTime);
        }

        clock.restart();
    }

    return 0;
}
