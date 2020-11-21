#include "CamCalib.h"


void camCalib(cv::Mat *cameraMatrix, cv::Mat *distCoeffs) {

	*cameraMatrix = (cv::Mat_<double>(3, 3) << 1338.97731731994, 0.0, 743.760887441795, //col1
		0.0, 1339.45727469457, //col2
		565.265832642684, 0.0, 0.0, 1.0); //col3

	*distCoeffs = (cv::Mat_<double>(5, 1) << -0.333797878516752, 0.266840692457182, -9.894537709656793e-04, -4.351439934274853e-04, -0.219091140819117);


}