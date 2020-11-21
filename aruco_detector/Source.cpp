#include "CamCalib.h"
#include <opencv2/aruco.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <fstream>
#include <chrono>

#include <iostream>

using namespace cv;
using namespace std;

int waitTime = 1; // 1 milisecond
bool display = true;  // to activate/deactivate display window


int main() {

   cv::Mat cameraMatrix, distCoeffs;

  /* image = cv::imread("D:\\robotica\\aruco\\imagen.png", IMREAD_COLOR);
    if (image.empty())
    {
        cout << "Could not open or find the image" << endl;
        cin.get(); //wait for any key press
        return -1;
    }
    
    else {
        cout << "Image loaded" << endl;
    }*/


    cv::VideoCapture inputVideo;
    inputVideo.open(0);
    while (inputVideo.grab()) {
        cv::Mat image, outputImage;
        
        inputVideo.retrieve(image);
        outputImage = image.clone();
        image.copyTo(outputImage);

        auto start = std::chrono::high_resolution_clock::now();

        camCalib(&cameraMatrix, &distCoeffs);
    
        std::vector<int> markerIds;
        std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;
        cv::Ptr<cv::aruco::DetectorParameters> parameters = cv::aruco::DetectorParameters::create();
        cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_ARUCO_ORIGINAL);
        cv::aruco::detectMarkers(image, dictionary, markerCorners, markerIds, parameters, rejectedCandidates);

        if (markerIds.size() > 0) {
            if (display) {
               cv::aruco::drawDetectedMarkers(outputImage, markerCorners, markerIds);
            }

            std::vector<cv::Vec3d> rvecs, tvecs;
            cv::aruco::estimatePoseSingleMarkers(markerCorners, 0.05, cameraMatrix, distCoeffs, rvecs, tvecs);
            // draw axis for each marker
            if (display){
                for (int i = 0; i < markerIds.size(); i++) {
                    cv::aruco::drawAxis(outputImage, cameraMatrix, distCoeffs, rvecs[i], tvecs[i], 0.05);
                }
            }

            cv::imwrite("D:/robotica/aruco/result.jpg", outputImage);
            
            auto finish = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = finish - start;
            //std::cout << "Elapsed time: " << elapsed.count() << " s\n";
            //std::cout << "Tvec: " << tvecs[0] << " mm\n";
            std::cout << "Rvec: " << rvecs[0] << " mm\n";
        }
        if (display) {
            imshow("Display window", outputImage);
            char key = (char)cv::waitKey(waitTime);
            if (key == 27)
                break;
        }     
    }

}
