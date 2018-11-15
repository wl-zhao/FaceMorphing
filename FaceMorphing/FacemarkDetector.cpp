#include "FacemarkDetector.h"
#include <exception>
#include <direct.h>

using namespace std;
using namespace cv;

FacemarkDetector::FacemarkDetector(String model_name, String cascade_name, String outputImgDir, String outputPointsDir)
{
	m_outputImgDir = outputImgDir;
	m_outputPointsDir = outputPointsDir;
	m_face_cascade.load(cascade_name);
	m_facemark = createFacemarkKazemi();
	m_facemark->loadModel(model_name);
}


FacemarkDetector::~FacemarkDetector()
{
}

void FacemarkDetector::run(Mat &img, String &imgFileName, bool outputImg, bool outputPoints)
{
	Mat gray;
	if (img.channels() > 1) {
		cvtColor(img, gray, COLOR_BGR2GRAY);
	}
	else {
		gray = img.clone();
	}
	equalizeHist(gray, gray);
	m_face_cascade.detectMultiScale(gray, m_faces, 1.1, 3, 0, Size(30, 30));

	if (m_faces.size() < 1)
	{
        throw runtime_error(imgFileName.c_str());
		return;
	}
	if (m_facemark->fit(img, m_faces, m_points))
	{
		if (m_faces.size() > 1)//the little boy example
		{
			Rect roi(0, 0, img.cols, m_img.rows * 0.55);
			Mat tmpImg = img(roi);
			m_faces.clear();
			m_points.clear();
			run(tmpImg, imgFileName, outputImg, outputPoints);
			return;
		}
		if (outputImg)
			drawImgWithControlPoints(imgFileName, m_outputImgDir);
		if (outputPoints)
			writeControlPoints(imgFileName, m_outputPointsDir);
		cout << imgFileName << ": detected" << endl;
	}
}

/*
 * run the detector
 * write control points into img*.txt
 * if outputImg=true, write control points to img for visualization
 */
void FacemarkDetector::run(String &imgFileName, bool outputImg, bool outputPoints)
{
	cout << "run detector for" << imgFileName << endl;
	m_img = imread(imgFileName);
	// variable to store faces and control points
	run(m_img, imgFileName, outputImg, outputPoints);
}

void FacemarkDetector::run(int i, bool outputImg, bool outputPoints)
{
	String defaultImgFileName = "../picture/" + to_string(i) +".jpg";
	run(defaultImgFileName, outputImg);
}

void FacemarkDetector::drawImgWithControlPoints(String imgFileName, String outputDir)
{
	size_t idx1 = imgFileName.find_last_of("/");
	size_t idx2 = imgFileName.find_last_of(".");
	String outputImgFileName = imgFileName.substr(0, idx1) + outputDir + imgFileName.substr(idx1, idx2 - idx1) + imgFileName.substr(idx2, imgFileName.length() - idx2);

	for (int i = m_faces.size() - 1; i >= 0; i--)
	{
		for (unsigned long k = 0; k < m_points[i].size(); k++)
		{
			circle(m_img, m_points[i][k], 3, cv::Scalar(k * 255 / 67, (67 - k) * 255 / 67, (2 * k - 67) * 255 / 67), FILLED);
		}
	}
	for (int i = m_faces.size() - 1; i >= 0; i--)
	{
		rectangle(m_img, m_faces[i], Scalar(255, 0, 0));
	}
	imwrite(outputImgFileName, m_img);
}

void FacemarkDetector::drawImgWithControlPoints(String imgFileName, String outputDir, vector<Point2d> points)
{
	Mat img = imread(imgFileName);
	size_t idx1 = imgFileName.find_last_of("/");
	size_t idx2 = imgFileName.find_last_of(".");
	String outputImgFileName = imgFileName.substr(0, idx1) + outputDir + imgFileName.substr(idx1, idx2 - idx1) + imgFileName.substr(idx2, imgFileName.length() - idx2);
	for (auto &point : points)
	{
		circle(img, point, 3, cv::Scalar(0, 0, 255), FILLED);
	}
	imwrite(outputImgFileName, img);
}

void FacemarkDetector::writeControlPoints(String imgFileName, String outputDir)
{
	size_t idx1 = imgFileName.find_last_of("/");
	size_t idx2 = imgFileName.find_last_of(".");
	auto dir = imgFileName.substr(0, idx1) + "/points";
	_mkdir(dir.c_str());
	String outputPointsFileName = dir + imgFileName.substr(idx1, idx2 - idx1) + ".txt";
	ofstream output(outputPointsFileName);
	cout << "point size = " << m_points[0].size() << endl;
	for (size_t i = m_faces.size() - 1; i >= 0; i--)
	{
		for (unsigned long k = 0; k < m_points[i].size(); k++)
		{
			cv::circle(m_img, m_points[i][k], 5, cv::Scalar(0, 0, 255), FILLED);
			output << m_points[i][k].x << " " << m_points[i][k].y << " " << endl;
		}
		output << endl;
		break;
	}
	output.close();
}

/*
 * read control points
 * if isImgFileName is true, FileName is the name of an image(i.e. *.jpg)
 * or FileName is the name of controlpoints file(i.e. *.txt)
 */
vector<cv::Point2d> FacemarkDetector::readControlPoints(String FileName, bool isImgFileName, bool detect)
{
	if (detect)
	{
		FacemarkDetector fd("../model/face_landmark_model.dat", "../model/lbpcascade_frontalface_improved.xml");
		fd.run(FileName, true, true);
	}
	size_t idx1 = FileName.find_last_of("/");
	size_t idx2 = FileName.find_last_of(".");
	if (isImgFileName)
	{
		FileName = FileName.substr(0, idx1) + "/points" + FileName.substr(idx1, idx2 - idx1) + ".txt";
	}
	ifstream input(FileName);
	if (!input)//not exist
	{
        throw(runtime_error("Can't read control points"));
	}
	double x, y;
	cv::Point2d point;
	vector<cv::Point2d> points;
	while (!input.eof())
	{
		if (input >> x && input >> y)
		{
			point = Point2d(x, y);
			points.push_back(point);
		}
	}
	if (points.size() != 68)
	{
        throw(runtime_error("Invalid control points"));
	}
	return points;
}
