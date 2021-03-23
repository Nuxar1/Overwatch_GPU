#include <iostream>
#include <math.h>
#include <queue>
#include <iterator>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <Windows.h>
#include <vector>
#include <future>
#include <thread>


#include <opencv2/core.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/dnn/all_layers.hpp>

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "Globals.h"
#include "Arduino.h"
#include "Capture.h"


namespace Globals
{
    Overlay Over = Overlay();
    bool Menu = false;
}
namespace Settings
{
    float CONFIDENCE_THRESHOLD = 0.4;
    float NMS_THRESHOLD = 0.3;
    int NUM_CLASSES = 1;
    float speed = 6.4;
    float flickSpeed = 1000;
    float flickDistanceFactor = 2;

    float horizontal_offset = 0.5;          // Fractional offset(between 0 and 1) from left side of the detection.
    float vertical_offset = 0.1;            // Fractional offset(between 0 and 1) from top top side of the detection.
    int screen_dimensions[] = { GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
    int input_dimensions[] = { 500, 600 };
    bool degub = false;
    Mode mode = Mode::Track;
    int aimKey = VK_MBUTTON;
}

// colors for bounding boxes
const cv::Scalar colors[] = {
    {0, 255, 255},
    {255, 255, 0},
    {0, 255, 0},
    {255, 0, 0}
};
const auto NUM_COLORS = sizeof(colors) / sizeof(colors[0]);
int roundto127(int a) {
    if (a < -127)
    {
        return -127;
    }
    else if (a > 127)
    {
        return 127;
    }
    else
    {
        return (int)a;
    }
}

void smooth(int target_dx, int target_dy, int * x_out, int * y_out, float speedMultiplier) {
    short x;
    short y;
    if (target_dx > 0)
    {
        x = (short)(target_dx / ((target_dx * (-1 * speedMultiplier) + 100) / 200) / (2 + 3.3 / Settings::speed));
        if (target_dy > 0)
        {
            y = (short)(target_dy / ((target_dy * (-1 * speedMultiplier) + 100) / 200) / (2 + 3.3 / Settings::speed));
        }
        else
        {
            y = (short)((target_dy * -1) / ((((target_dy * -1) * (-1 * speedMultiplier) + 100) * -1) / 200) / (2 + 3.3 / Settings::speed));
        }
    }
    else
    {
        x = (short)((target_dx * -1) / ((((target_dx * -1) * (-1 * speedMultiplier) + 100) * -1) / 200) / (2 + 3.3 / Settings::speed));

        if (target_dy > 0)
        {
            y = (short)(target_dy / ((target_dy * (-1 * speedMultiplier) + 100) / 200) / (2 + 3.3 / Settings::speed));
        }
        else
        {
            y = (short)((target_dy * -1) / ((((target_dy * -1) * (-1 * speedMultiplier) + 100) * -1) / 200) / (2 + 3.3 / Settings::speed));
        }
    }
    *x_out = x;
    *y_out = y;
}
Arduino ard;
struct TargetInfo
{
    int dx;
    int dy;
};
void flick(TargetInfo * info) {
    double delta_t = 0.04;

    double rounding_error_x = 0.0;
    double rounding_error_y = 0.0;

    double mouse_x = (double)info->dx * Settings::flickDistanceFactor;
    double mouse_y = (double)info->dy * Settings::flickDistanceFactor;

    double flick_distance = sqrt(mouse_x * mouse_x + mouse_y * mouse_y);
    double flick_time = flick_distance / Settings::flickSpeed;

    int steps = ceil(flick_time / delta_t);
    for (int i = 0; i < steps; i++) {

        auto start = std::chrono::steady_clock::now();

        double fractional_x = rounding_error_x + mouse_x / steps;
        double fractional_y = rounding_error_y + mouse_y / steps;

        int pixels_x = roundto127(round(fractional_x));
        int pixels_y = roundto127(round(fractional_y));

        rounding_error_x = fractional_x - pixels_x;
        rounding_error_y = fractional_y - pixels_y;

        ard.mouseEvent(pixels_x, pixels_y, 0, 0, 0);

        while (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() < 4000 && i != steps-1);
    }
    ard.mouseEvent(0, 0, 1, 0, 0);
}


static std::mutex s_Vector;
std::vector<std::future<void>> m_Futures;

void GetDetections(cv::Mat * output, cv::Mat * outMat, std::vector<float>* scores, std::vector<cv::Rect>* boxes) {
    const auto num_boxes = output->rows;
    for (int i = 0; i < num_boxes; i++)
    {
        auto x = output->at<float>(i, 0) * outMat->cols;
        auto y = output->at<float>(i, 1) * outMat->rows;
        auto width = output->at<float>(i, 2) * outMat->cols;
        auto height = output->at<float>(i, 3) * outMat->rows;
        cv::Rect rect(x - width / 2, y - height / 2, width, height);

        auto confidence = *output->ptr<float>(i, 5);
        if (confidence >= Settings::CONFIDENCE_THRESHOLD)
        {
            std::lock_guard<std::mutex> lock(s_Vector);
            boxes->push_back(rect);
            scores->push_back(confidence);
        }
    }
}
static std::mutex s_Frame;
void Screenshot(cv::Mat* outMat) {
    Capture screen((char*)"TankWindowClass");
    while (!screen.isWindowRunning())
    {
        std::cout << ".";
        Sleep(1000);
    }
    screen.InitDx();
    screen.initScreenshot();
    cv::Mat frame;
    while (true)
    {
        frame = screen.hwnd2mat().clone(); 
        std::lock_guard<std::mutex> lock(s_Frame);
        cv::cvtColor(frame, *outMat, cv::ColorConversionCodes::COLOR_BGRA2BGR, 3);
    }
}
void DrawOverlay() {
    Globals::Over.xCreateWindow();
    Globals::Over.Loop();
}
void KeyListener() {
    while (true)
    {
        if (GetAsyncKeyState(VK_END))
        {
            Globals::Menu = !Globals::Menu;
            Globals::Over.ChangeClickability();
            while (GetAsyncKeyState(VK_END)) Sleep(10);
        }
        std::this_thread::sleep_for(
            std::chrono::milliseconds(20));
    }
}
int main()
{
    std::string port;
    std::cout << "Enter comport:";
    std::cin >> port;
    ard.Init(s2ws(port));
    std::vector<std::string> class_names;
    {
        std::ifstream class_file("classes.txt");
        if (!class_file)
        {
            std::cerr << "failed to open classes.txt\n";
            return 0;
        }

        std::string line;
        while (std::getline(class_file, line))
            class_names.push_back(line);
    }

    auto net = cv::dnn::readNetFromDarknet("yolov4-tiny-3l-outline.cfg", "yolov4-tiny-3l-outline.weights");
    net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
    net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
    auto output_names = net.getUnconnectedOutLayersNames();

    cv::Mat blob, outMat;
    BYTE* test;
    std::vector<cv::Mat> detections;

    outMat.create(Settings::input_dimensions[0], Settings::input_dimensions[1], CV_8UC3);

    std::thread captureScreen(Screenshot, &outMat);

    std::cout << std::endl << "Overwatch found!!!" << std::endl;

    HANDLE OverlayThread = CreateThread(NULL, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(DrawOverlay), NULL, NULL, NULL);

    HANDLE KeyListenThread = CreateThread(NULL, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(KeyListener), NULL, NULL, NULL);
    float speedMultiplier = (Settings::speed - 10) / (10);
    while (cv::waitKey(1) < 1)
    {
        int capture_left = int(Settings::screen_dimensions[0] / 2 - Settings::input_dimensions[0] / 2);
        int capture_top = int(Settings::screen_dimensions[1] / 2 - Settings::input_dimensions[1] / 2);
        cv::Rect fov(capture_left, capture_top, Settings::input_dimensions[0], Settings::input_dimensions[1]);

        auto total_start = std::chrono::steady_clock::now();
        cv::dnn::blobFromImage(outMat, blob, 0.00392, cv::Size(512, 512), cv::Scalar(), true, false, CV_32F);
        net.setInput(blob);

        auto dnn_start = std::chrono::steady_clock::now();
        net.forward(detections, output_names);
        auto dnn_end = std::chrono::steady_clock::now();

        std::vector<int> indices;
        std::vector<cv::Rect> boxes;
        std::vector<float> scores;

        for (auto& output : detections)
        {
            m_Futures.push_back(std::async(std::launch::async, GetDetections, &output, &outMat, &scores, &boxes));
        }
        m_Futures.clear();

        /*if (boxes.size() <= 0 || scores.size() <= 0)
            continue;*/


        cv::dnn::NMSBoxes(boxes, scores, 0.0, Settings::NMS_THRESHOLD, indices);
        int min_idx = -1;
        int min_distance = 0.0;
        int target_dx = 0.0;
        int target_dy = 0.0;
        int trigger = 0;
        for (size_t i = 0; i < indices.size(); ++i)
        {
            const auto& rect = boxes[indices[i]];

            int dx = rect.x + Settings::horizontal_offset * rect.width - Settings::input_dimensions[0] / 2;
            int dy = rect.y + Settings::vertical_offset * rect.height - Settings::input_dimensions[1] / 2;
            int distance = sqrt(dx * dx + dy * dy);
            if (min_idx < 0 || distance < min_distance) {
                if (distance < rect.width * (Settings::vertical_offset + 0.18))
                    trigger = 1;
                min_idx = i;
                min_distance = distance;
                target_dx = dx;
                target_dy = dy;
            }
            if (Settings::degub) {

                const auto color = colors[1];

                auto idx = indices[i];
                const auto& rect = boxes[idx];
                cv::rectangle(outMat, cv::Point(rect.x, rect.y), cv::Point(rect.x + rect.width, rect.y + rect.height), color, 3);

                std::ostringstream label_ss;
                label_ss << class_names[0] << ": " << std::fixed << std::setprecision(2) << scores[idx];
                auto label = label_ss.str();

                int baseline;
                auto label_bg_sz = cv::getTextSize(label.c_str(), cv::FONT_HERSHEY_COMPLEX_SMALL, 1, 1, &baseline);
                cv::rectangle(outMat, cv::Point(rect.x, rect.y - label_bg_sz.height - baseline - 10), cv::Point(rect.x + label_bg_sz.width, rect.y), color, cv::FILLED);
                cv::putText(outMat, label.c_str(), cv::Point(rect.x, rect.y - baseline - 5), cv::FONT_HERSHEY_COMPLEX_SMALL, 1, cv::Scalar(0, 0, 0));
            }
        }
        if (Settings::mode == Mode::Track) {
            if (GetAsyncKeyState(Settings::aimKey)) {
                int x;
                int y;
                smooth(roundto127(target_dx), roundto127(target_dy), &x, &y, speedMultiplier);
                ard.mouseEvent(roundto127(x), roundto127(y), trigger, 0, 0);
            }
        }
        else if (Settings::mode == Mode::Flick) {
            if (GetAsyncKeyState(Settings::aimKey) && (target_dx != 0 || target_dy != 0)) {
                TargetInfo info;
                info.dx = target_dx;
                info.dy = target_dy;
                flick(&info);
                while (GetAsyncKeyState(Settings::aimKey))
                {
                    Sleep(1);
                }
            }
        }

        if (Settings::degub)
        {

            auto total_end = std::chrono::steady_clock::now();
            float inference_fps = 1000.0 / std::chrono::duration_cast<std::chrono::milliseconds>(dnn_end - dnn_start).count();
            float total_fps = 1000.0 / std::chrono::duration_cast<std::chrono::milliseconds>(total_end - total_start).count();
            std::ostringstream stats_ss;
            stats_ss << std::fixed << std::setprecision(2);
            stats_ss << "Inference FPS: " << inference_fps << ", Total FPS: " << total_fps;
            auto stats = stats_ss.str();

            int baseline;
            auto stats_bg_sz = cv::getTextSize(stats.c_str(), cv::FONT_HERSHEY_COMPLEX_SMALL, 1, 1, &baseline);
            cv::rectangle(outMat, cv::Point(0, 0), cv::Point(stats_bg_sz.width, stats_bg_sz.height + 10), cv::Scalar(0, 0, 0), cv::FILLED);
            cv::putText(outMat, stats.c_str(), cv::Point(0, stats_bg_sz.height + 5), cv::FONT_HERSHEY_COMPLEX_SMALL, 1, cv::Scalar(255, 255, 255));

            cv::namedWindow("output");
            cv::imshow("output", outMat);
        }
    }

    return 0;
}