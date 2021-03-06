#include "WriteFiles.h"

int nImage = 0;
String imgPath = "D:/robotica/aruco/img/img";
String imgExtension = ".jpg";
String posePath = "D:/robotica/aruco/arucoPose/arucoPose";
String poseExtension = ".csv";
String UDPPath = "D:/robotica/aruco/robotPose/robotPose";
String UDPExtension = ".csv";

void writeArucoPoseInFile(Vec3d rvecs, Vec3d tvecs, Mat outputImage, std::ofstream* allOutfile) {
   /* std::ofstream outfile;
    outfile.open(posePath + std::to_string(nImage) + poseExtension);
    outfile << tvecs[0] << "\t" << tvecs[1] << "\t" << tvecs[2] << "\t" << rvecs[0] << "\t" << rvecs[1] << "\t" << rvecs[2] << "\n" << std::endl;
    outfile.close();*/
    *allOutfile << tvecs[0] << ';' << tvecs[1] << ';' << tvecs[2] << ';' << rvecs[0] << ';' << rvecs[1] << ';' << rvecs[2] << std::endl;

    imwrite((imgPath + std::to_string(nImage) + imgExtension), outputImage);
    nImage++;
}

void writeRobotPoseInFile(char buf[], std::ofstream* allOutfile) {
	
	/*static const char* xml = 
		"<?xml version=\"1.0\"?>"
		"<Rob Type=\"KUKA\">"
			"<X>615.0</X>"
			"<Y>140.0</Y>"
			"<Z>420.0</Z>"
			"<A>-180.0</A>"
			"<B>0.0</B>"
			"<C>-90.0</C>"
			"<IPOC>361233514</IPOC>"
		"</Rob>";*/
	std::string result = buf;
	XMLDocument doc;
	doc.Parse(buf);

	const char* X = doc.FirstChildElement("Rob")->FirstChildElement("X")->GetText();
	const char* Y = doc.FirstChildElement("Rob")->FirstChildElement("Y")->GetText();
	const char* Z = doc.FirstChildElement("Rob")->FirstChildElement("Z")->GetText();
	const char* A = doc.FirstChildElement("Rob")->FirstChildElement("A")->GetText();
	const char* B = doc.FirstChildElement("Rob")->FirstChildElement("B")->GetText();
	const char* C = doc.FirstChildElement("Rob")->FirstChildElement("C")->GetText();

	printf("X: %s\n", X);
	printf("Y: %s\n", Y);
	printf("Z: %s\n", Z);
	printf("A: %s\n", A);
	printf("B: %s\n", B);
	printf("C: %s\n", C);

	result = X;
	result.append(";");
	result.append(Y);
	result.append(";");
	result.append(Z);
	result.append(";");
	result.append(A);
	result.append(";");
	result.append(B);
	result.append(";");
	result.append(C);
	result.append(";");

	*allOutfile << result;

	
	/*printf("Z: %s\n", Z);
	printf("A: %s\n", A);
	printf("B: %s\n", B);
	printf("C: %s\n", C);*/


	//return doc.ErrorID();
	//getValuesFromXML(buf, newBuf, toGetSize.size());
	
}

void getValuesFromXML(char buf[], char destBuf[], size_t size) {
	int resultPos = 0;

	for (int i = 0; i < size; i++) {
		if (buf[i] == '\"' && buf[i + 1] != 'K') {
			int j = i + 1;
			while (buf[j] != '\"') {
				destBuf[resultPos++] = buf[j];
			}
			destBuf[resultPos++] = ';';
		}
	}
}