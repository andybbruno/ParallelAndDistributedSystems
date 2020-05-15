#include <iostream>
#include <vector>
#include <numeric>
#include <string>
#include <functional>
#include "lib/utimer.cpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

using namespace cv;

// export LD_LIBRARY_PATH=/home/marcod/opencv4//lib/:/opt/intel/compilers_and_libraries/linux/mkl/lib/intel64/:$LD_LIBRARY_PATH

int main(int argc, char *argv[])
{
    String path = argv[1];

    std::vector<cv::String> files;
    glob(path, files, false);

    std::vector<int> read_time(files.size());
    std::vector<int> gauss_time(files.size());
    std::vector<int> sobel_time(files.size());
    std::vector<int> write_time(files.size());

    auto start = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    utimer u("Seq");

    for (size_t i = 0; i < files.size(); i++)
    {
        Mat src_gray;
        Mat grad;
        Mat grad_x;
        Mat grad_y;
        Mat abs_grad_x;
        Mat abs_grad_y;
        Mat gauss;

        auto start = std::chrono::high_resolution_clock::now();
        Mat tmp = imread(samples::findFile(files[i]), IMREAD_COLOR);
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        read_time.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count());

        cvtColor(tmp, src_gray, COLOR_BGR2GRAY);

        start = std::chrono::high_resolution_clock::now();
        GaussianBlur(src_gray, gauss, Size(13, 13), 0, 0);
        elapsed = std::chrono::high_resolution_clock::now() - start;
        gauss_time.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count());

        start = std::chrono::high_resolution_clock::now();
        Sobel(gauss, grad_x, CV_16S, 1, 0, 1, 1, 0, BORDER_DEFAULT);
        Sobel(gauss, grad_y, CV_16S, 0, 1, 1, 1, 0, BORDER_DEFAULT);
        convertScaleAbs(grad_x, abs_grad_x);
        convertScaleAbs(grad_y, abs_grad_y);
        addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);
        elapsed = std::chrono::high_resolution_clock::now() - start;
        sobel_time.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count());

        String str = "res/" + std::to_string(i) + ".jpg";

        start = std::chrono::high_resolution_clock::now();
        imwrite(str, grad);
        elapsed = std::chrono::high_resolution_clock::now() - start;
        write_time.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count());
    }

    std::cout << "Processed " << files.size() << " elements" << std::endl;
    std::cout << "Read  in " << std::accumulate(read_time.begin(), read_time.end(), 0LL) / read_time.size() << std::endl;
    std::cout << "Gauss in " << std::accumulate(gauss_time.begin(), gauss_time.end(), 0LL) / gauss_time.size() << std::endl;
    std::cout << "Sobel in " << std::accumulate(sobel_time.begin(), sobel_time.end(), 0LL) / sobel_time.size() << std::endl;
    std::cout << "Write in " << std::accumulate(write_time.begin(), write_time.end(), 0LL) / write_time.size() << std::endl;
}