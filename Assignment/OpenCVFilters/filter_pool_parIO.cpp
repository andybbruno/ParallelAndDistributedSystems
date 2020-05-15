#include <iostream>
#include <vector>
#include <numeric>
#include <string>
#include <algorithm>
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
    int nw = atoi(argv[2]);
    
    std::vector<cv::String> files;
    glob(path, files, false);

    nw = std::min(nw, (int)files.size());

    auto start = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    utimer u("POOL");

    std::vector<std::pair<int, int>> ranges(nw);
    int delta = files.size() / nw;
    int mod = files.size() % nw;
    for (int i = 0; i < nw; i++)
    {
        int a = (i != 0 ? ranges[i - 1].second + 1 : 0);
        int b = (i - mod < 0 ? a + delta : a + delta - 1);
        ranges[i] = std::make_pair(a, b);
        // std::cout << "ranges[" << i << "] " << ranges[i].first << " , "
        //           << ranges[i].second << std::endl;
    }

    std::vector<std::thread> tids;
    for (int i = 0; i < nw; i++)
    {
        auto tmp_pair = ranges[i];
        tids.push_back(std::thread([tmp_pair, files]() {
            for (int j = tmp_pair.first; j <= tmp_pair.second; j++)
            {
                Mat src_gray;
                Mat grad;
                Mat grad_x;
                Mat grad_y;
                Mat abs_grad_x;
                Mat abs_grad_y;
                Mat gauss;

                Mat tmp = imread(files[j], IMREAD_COLOR);
                cvtColor(tmp, src_gray, COLOR_BGR2GRAY);
                GaussianBlur(src_gray, gauss, Size(13, 13), 0, 0);
                Sobel(gauss, grad_x, CV_16S, 1, 0, 1, 1, 0, BORDER_DEFAULT);
                Sobel(gauss, grad_y, CV_16S, 0, 1, 1, 1, 0, BORDER_DEFAULT);
                convertScaleAbs(grad_x, abs_grad_x);
                convertScaleAbs(grad_y, abs_grad_y);
                addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);
                String str = "res/" + std::to_string(j) + ".jpg";

                imwrite(str, grad);
            }
        }));
    }

    for (std::thread &t : tids)
    { // await thread termination
        t.join();
    }
    std::cout << "Processed " << files.size() << " elements" << std::endl;
}