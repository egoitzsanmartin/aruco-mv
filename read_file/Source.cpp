#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;
int main(int argc, char** argv)
{
    //std::locale::global(std::locale(""));
    String imageName("c:\\pipo.bmp"); // by default
    if (argc > 1)
    {
        imageName = argv[1];
    }
    Mat image;
    //image.create(300, 300, CV_32FC3);
    //image.setTo(cv::Scalar(0.0f, 2.0f, 5.0f));
    image = cv::imread("d:\\img\\pipo.bmp"); // Read the file
    if (image.empty())                      // Check for invalid input
    {
        cout << "Could not open or find the image" << std::endl;
        return -1;
    }
    //namedWindow("Display window", cv::WINDOW_AUTOSIZE); // Create a window for display.
    imshow("Display window", image);                // Show our image inside it.
    int k = waitKey(10000); // Wait for a keystroke in the window
    return 0;
}