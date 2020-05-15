#include <iostream>
#include <omp.h>
#include "lib/utimer.cpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <ff/utils.hpp>
#include <ff/parallel_for.hpp>

using namespace cv;

// export LD_LIBRARY_PATH=/home/marcod/opencv4//lib/:/opt/intel/compilers_and_libraries/linux/mkl/lib/intel64/:$LD_LIBRARY_PATH

int main(int argc, char *argv[])
{

    String path = argv[1];
    int nw = atoi(argv[2]);

    std::vector<cv::String> files;
    glob(path, files, false);

    utimer u("FF");
    ff::ParallelFor pf(nw);

    std::vector<Mat> v;
    for (int i = 0; i < files.size(); i++)
    {
        Mat tmp = imread(samples::findFile(files[i]), IMREAD_COLOR);
        v.push_back(tmp);
    }

    pf.parallel_for(0, files.size(), [&](const long i) {
        Mat src_gray;
        Mat grad;
        Mat grad_x;
        Mat grad_y;
        Mat abs_grad_x;
        Mat abs_grad_y;
        Mat gauss;

        cvtColor(v[i], src_gray, COLOR_BGR2GRAY);
        GaussianBlur(src_gray, gauss, Size(13, 13), 0, 0);
        Sobel(gauss, grad_x, CV_16S, 1, 0, 1, 1, 0, BORDER_DEFAULT);
        Sobel(gauss, grad_y, CV_16S, 0, 1, 1, 1, 0, BORDER_DEFAULT);
        convertScaleAbs(grad_x, abs_grad_x);
        convertScaleAbs(grad_y, abs_grad_y);
        addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);
    });

    for (int i = 0; i < v.size(); i++)
    {
        String str = "res/" + std::to_string(i) + ".jpg";
        imwrite(str, v[i]);
    }
}