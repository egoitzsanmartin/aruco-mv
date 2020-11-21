#include "CamCalib.h"
#include "MatrixVisionDevice.h"
#include "UDP_Server.h"
#include "Parameters.h"
#include "WriteFiles.h"

using namespace cv;
using namespace std;

void runProgram(mvIMPACT::acquire::Device* pDev);

int waitTime = 1; // 1 milisecond
bool display = true;  // to activate/deactivate display window
bool writeInFile = true;  // to activate/deactivate writing parameters in files
float markerSize = 50;

int main() {
    mvIMPACT::acquire::DeviceManager devMgr;

    if (devMgr.deviceCount() == 0)
    {
        cout << "No device found! Unable to continue!" << endl;
        cin.get(); //wait for any key press
    }
    mvIMPACT::acquire::Device* pDev = getDeviceFromUserInput(devMgr, isDeviceSupportedBySample);
    initializeDevice(pDev);

    runProgram(pDev);
}

void runProgram(mvIMPACT::acquire::Device* pDev) {
    ofstream allOutfile;
    allOutfile.open("D:/robotica/aruco/pose.txt");
    helper::RequestProvider requestProvider(pDev);
    requestProvider.acquisitionStart();

    while (true) {
        cv::Mat outputImage, cameraMatrix, distCoeffs;
        auto start = std::chrono::high_resolution_clock::now();
        std::shared_ptr<Request> pRequest = requestProvider.waitForNextRequest();
        outputImage = getImage(pRequest);

        camCalib(&cameraMatrix, &distCoeffs);

        std::vector<int> markerIds;
	
        std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;
        cv::Ptr<cv::aruco::DetectorParameters> parameters = cv::aruco::DetectorParameters::create();
        cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_ARUCO_ORIGINAL);
        
        bool e = readDetectorParameters("detector_params.yml", parameters);

        cv::aruco::detectMarkers(outputImage, dictionary, markerCorners, markerIds, parameters, rejectedCandidates);

        if (markerIds.size() > 0) {
            if (display) {
                cv::aruco::drawDetectedMarkers(outputImage, markerCorners, markerIds);
            }

            std::vector<cv::Vec3d> rvecs, tvecs;
            Mat m33(3, 3, CV_64F), Degree_euler;

            cv::aruco::estimatePoseSingleMarkers(markerCorners, markerSize, cameraMatrix, distCoeffs, rvecs, tvecs);

            cv::Rodrigues(rvecs, m33);
            // Degree_euler = m33 * 180 / CV_PI;

            if (display) {
                for (int i = 0; i < markerIds.size(); i++) {
                    cv::aruco::drawAxis(outputImage, cameraMatrix, distCoeffs, rvecs[i], tvecs[i], 25);
                    
                    //std::cout << "r(euler): " << Degree_euler << " s\n";
                    std::cout << "r: " << m33 << " s\n";
                    std::cout << "t: " << tvecs[i] << " s\n";

                    if (writeInFile) {
                        getRobotPose(&allOutfile);
                        writeArucoPoseInFile(rvecs[i], tvecs[i], outputImage, &allOutfile);
                    }
                }
            }

            auto finish = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = finish - start;
            std::cout << "Capture: " << elapsed.count() << " s\n";
        }
        if (display) {
            namedWindow("Display window", WINDOW_NORMAL);
            imshow("Display window", outputImage);
            char key = (char)cv::waitKey(waitTime);
            if (key == 27)
                break;
        }
    }
    allOutfile.close();
    requestProvider.acquisitionStop();
}