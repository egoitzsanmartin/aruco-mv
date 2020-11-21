#pragma once
#ifndef MV_Device_CLASS
#define MV_Device_CLASS
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <apps/Common/exampleHelper.h>
#include <mvIMPACT_CPP/mvIMPACT_acquire_helper.h>

using cv::Mat;
using std::string;
using std::vector;
using std::map;
using std::pair;
using std::cout;
using std::endl;
using namespace mvIMPACT::acquire;

using Prop = pair<string, string>;
using PropertyList = vector<Prop>;
using PropertyMap = map<string, Property>;
class MV_Device {

public:
	MV_Device();
	void init();
	void setParameters(PropertyList properties);
	PropertyList getParameters();
	int getDataType(TImageBufferPixelFormat format);
	Mat capture();
	void close();
private:
	DeviceManager devMgr;
	Device* pDev;
	FunctionInterface* fi;
	PropertyMap properties;
	Mat lastImg;

	//private method to get image's format.
	//private methods to get properties from device
	PropertyMap getPropertiesFromDevice();
	void populatePropertyMap(PropertyMap* m, ComponentIterator it, const string path = "");
	void setProperty(Prop prop);

};

#endif