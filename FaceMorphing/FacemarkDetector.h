#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include <vector>
#include "MyCV.h"

using namespace cv::face;
using namespace std;

//Detect facemark using face module in opencv-contrib
class FacemarkDetector
{
public:
	FacemarkDetector(String model_name, String cascade_name, String outputImgDir = "/detected", String outputPointDir = "/points");
	~FacemarkDetector();

	void run(cv::Mat &img, String &imgFileName, bool outputImg = false, bool outputPoints = true);
	void run(String &imgFileName, bool outputImg = false, bool outputPoints = true);
	void run(int i, bool outputImg = false, bool outputPoints = true);
	void drawImgWithControlPoints(String imgFileName, String outputDir);
	static void drawImgWithControlPoints(String imgFileName, String outputDir, vector<cv::Point2d> points);

	static vector<cv::Point2d> readControlPoints(String FileName, bool isImgFileName = false, bool detect = false);
private:
	void writeControlPoints(String imgFileName, String outputDir);

private:
	cv::CascadeClassifier m_face_cascade;
	cv::Ptr<Facemark> m_facemark;
	cv::Mat m_img;
	vector<cv::Rect> m_faces;
	vector<vector<cv::Point2f>> m_points;
	String m_outputImgDir;
	String m_outputPointsDir;
};

