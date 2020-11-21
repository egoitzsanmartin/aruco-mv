#include "MV_Device.h"

MV_Device::MV_Device() {
	devMgr = DeviceManager();
}

void MV_Device::init(void) {
	cout << "Begining device initialization..." << endl;

	if (devMgr.deviceCount() == 0) {
		cout << "No device found! Unable to continue!" << endl;
		return;
	}
	cout << "Initializing the device, this might take some time..." << endl;

	pDev = devMgr[0]; //using properties this would be read from file.

	try {
		pDev->open();
	} catch (ImpactAcquireException & ex) {
		cout << "An error occurred while opening the device. Error code: " << ex.getErrorCode() << "." << endl;
		close();
		return;
	}

	fi = new FunctionInterface(pDev);

	if (pDev->acquisitionStartStopBehaviour.read() == assbUser) {
		TDMR_ERROR result = DMR_NO_ERROR;
		if ((result == static_cast<TDMR_ERROR>(fi->acquisitionStart())) != DMR_NO_ERROR) {
			cout << "'FunctionInterface.acquisitionStart' returned with an unexpected result: " << result << "(" << ImpactAcquireException::getErrorCodeAsString(result) << ")" << endl;
			close();
			return;
		}
	}
	cout << "Device initialized." << endl;
}

Mat MV_Device::capture(void) {
	bool valid = false;
	while (!valid) {
		valid = true;
		fi->imageRequestSingle();
		int requestN = fi->imageRequestWaitFor(2000);
		if (!fi->isRequestNrValid(requestN)) {
			cout << "Image Request failed. Timeout value too small?" << endl;
			valid = false;
		}
		const Request* pRequest = fi->getRequest(requestN);
		if (!pRequest->isOK()) {
			cout << "Error, request result: " << pRequest->requestResult.readS() << endl;
			valid = false;
		}

		int dataType = getDataType(pRequest->imagePixelFormat.read());
		lastImg = Mat();
		if (dataType != -1) lastImg = Mat(cv::Size(pRequest->imageWidth.read(), pRequest->imageHeight.read()), dataType, pRequest->imageData.read(), pRequest->imageLinePitch.read());
		else valid = false;

		if (lastImg.data) {
			valid = true;
		}
		fi->imageRequestUnlock(requestN);

		if (!valid) cout << "Capture failed, trying again..." << endl;
	}
	return lastImg.clone();
}

void MV_Device::close(void) {
	cout << "Closing device..." << endl;
	if (pDev != nullptr) pDev->close();
	cout << "Device closed!" << endl;
}

void MV_Device::setParameters(PropertyList list) {
	cout << "Loading Properties into device..." << endl;
	properties = getPropertiesFromDevice();
	for (size_t i = 0; i != list.size(); i++) {
		setProperty(list[i]);
	}
}

PropertyList MV_Device::getParameters(void) {
	properties = getPropertiesFromDevice();
	PropertyList list;
	for (PropertyMap::iterator it = properties.begin(); it != properties.end(); ++it) {
		Prop newProperty(it->first, it->second.readSArray());
		list.push_back(newProperty);
	}
	return list;
}

int MV_Device::getDataType(TImageBufferPixelFormat format) {
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

PropertyMap MV_Device::getPropertiesFromDevice(void) {
	PropertyMap propertyMap;
	DeviceComponentLocator locator(pDev, dltSetting, "Base");
	populatePropertyMap(&propertyMap, ComponentIterator(locator.searchbase_id()).firstChild());

	try {
		locator = DeviceComponentLocator(pDev, dltIOSubSystem);
		populatePropertyMap(&propertyMap, ComponentIterator(locator.searchbase_id()).firstChild());
	} catch (const ImpactAcquireException&) {}

	locator = DeviceComponentLocator(pDev, dltRequest);
	populatePropertyMap(&propertyMap, ComponentIterator(locator.searchbase_id()).firstChild());

	locator = DeviceComponentLocator(pDev, dltSystemSettings);
	populatePropertyMap(&propertyMap, ComponentIterator(locator.searchbase_id()).firstChild(), "SystemSettings");

	locator = DeviceComponentLocator(pDev, dltInfo);
	populatePropertyMap(&propertyMap, ComponentIterator(locator.searchbase_id()).firstChild(), "Info");
	populatePropertyMap(&propertyMap, ComponentIterator(pDev->hDev()).firstChild(), "Device");

	return propertyMap;
}

void MV_Device::populatePropertyMap(PropertyMap* m, ComponentIterator it, const string path) {
	while (it.isValid()) {
		string fullName(path);
		if (fullName != "") fullName += "/";
		fullName += it.name();
		if (it.isList()) populatePropertyMap(m, it.firstChild(), fullName);
		else if (it.isProp()) m->insert(make_pair(fullName, Property(it)));
		++it;
	}
}

void MV_Device::setProperty(Prop prop) {
	PropertyMap::const_iterator it = properties.find(prop.first);

	if (it != properties.end()) {
		Property p = it->second;
		p.writeS(prop.second, 0);
		fi->saveSetting(pDev->serial.read());
	} else {
		cout << "Unknown command or property: " << prop.first << endl;
	}
}