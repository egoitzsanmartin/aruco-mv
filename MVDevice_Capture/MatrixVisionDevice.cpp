#include <apps/Common/exampleHelper.h>
#include <mvIMPACT_acquire.h>
#include <mvIMPACT_acquire_helper.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

int getDataType(TImageBufferPixelFormat format);

using namespace mvIMPACT::acquire;
using namespace std;
using namespace cv;

//-----------------------------------------------------------------------------
int main(int argc, char* argv[])
//-----------------------------------------------------------------------------
{
	mvIMPACT::acquire::DeviceManager devMgr;
	if (devMgr.deviceCount() == 0)
	{
		cout << "No device found! Unable to continue!" << endl;
		//char ch = getch();
		return 0;
	}

	cout << "Initializing the device. This might take some time..." << endl;
	// create an interface to the first device found
	mvIMPACT::acquire::Device* pDev = devMgr[0];
	try
	{
		pDev->open();
	}
	catch (mvIMPACT::acquire::ImpactAcquireException& e)
	{
		// this e.g. might happen if the same device is already opened in another process...
		//cout << "An error occurred while opening the device(error code: " << e.errCode()
		//	<< "). Press any key to end the application..." << endl;
		//char ch = getch();
		return 0;
	}

    helper::RequestProvider requestProvider(pDev);
    requestProvider.acquisitionStart();

	while (true) {
		std::shared_ptr<Request> pRequest = requestProvider.waitForNextRequest();
		std::cout << "Image captured: " << pRequest->imageOffsetX.read() << "x" << pRequest->imageOffsetY.read() << "@" << pRequest->imageWidth.read() << "x" << pRequest->imageHeight.read() << std::endl;
		int dataType = getDataType(pRequest->imagePixelFormat.read());
		Mat img = Mat(cv::Size(pRequest->imageWidth.read(), pRequest->imageHeight.read()), dataType, pRequest->imageData.read(), pRequest->imageLinePitch.read());
		imshow("Device", img);
		cv::waitKey(100);
	}


    requestProvider.acquisitionStop();
    return 0;
}

int getDataType(TImageBufferPixelFormat format) {
	int dataType = -1;
	switch (format) {
	case ibpfMono8:
		dataType = CV_8UC1;
		break;
	case ibpfMono10:
	case ibpfMono12:
	case ibpfMono14:
	case ibpfMono16:
		dataType = CV_16UC1;
		break;
	case ibpfMono32:
		dataType = CV_32SC1;
		break;
	case ibpfBGR888Packed:
	case ibpfRGB888Packed:
		dataType = CV_8UC3;
		break;
	case ibpfRGBx888Packed:
		dataType = CV_8UC4;
		break;
	case ibpfRGB101010Packed:
	case ibpfRGB121212Packed:
	case ibpfRGB141414Packed:
	case ibpfRGB161616Packed:
		dataType = CV_16UC3;
		break;
	case ibpfMono12Packed_V1:
	case ibpfMono12Packed_V2:
	case ibpfBGR101010Packed_V2:
	case ibpfRGB888Planar:
	case ibpfRGBx888Planar:
	case ibpfYUV422Packed:
	case ibpfYUV422_10Packed:
	case ibpfYUV422_UYVYPacked:
	case ibpfYUV422_UYVY_10Packed:
	case ibpfYUV422Planar:
	case ibpfYUV444Packed:
	case ibpfYUV444_10Packed:
	case ibpfYUV444_UYVPacked:
	case ibpfYUV444_UYV_10Packed:
	case ibpfYUV444Planar:
	case ibpfYUV411_UYYVYY_Packed:
	default:
		cout << "Data format not available in opencv" << endl;
		//throw CaptureFailedException();
	}
	return dataType;
}