#include <iostream>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

using namespace cv;

int main(int argc, char *argv[])
{
    String img_path = argv[1];

    Mat image = imread(samples::findFile(img_path), IMREAD_COLOR); // Load an image

    if (image.empty())
    {
        printf("Error opening image: %s\n", img_path.c_str());
        return EXIT_FAILURE;
    }

    Mat src_gray;
    cvtColor(image, src_gray, COLOR_BGR2GRAY);
    
    Mat gauss;
    GaussianBlur(src_gray, gauss, Size(13, 13), 0, 0);


    Mat grad;
    Mat grad_x;
    Mat grad_y;
    Mat abs_grad_x;
    Mat abs_grad_y;


    Sobel(gauss, grad_x, CV_16S, 1, 0, 1, 1, 0, BORDER_DEFAULT);
    Sobel(gauss, grad_y, CV_16S, 0, 1, 1, 1, 0, BORDER_DEFAULT);

    convertScaleAbs(grad_x, abs_grad_x);
    convertScaleAbs(grad_y, abs_grad_y);
    addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);

    imwrite("sobel.jpg", grad);
}