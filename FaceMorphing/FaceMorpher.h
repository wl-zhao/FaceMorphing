#pragma once
#include <opencv2/opencv.hpp>
#include "MyCV.h"
#include "TPS.h"
#include "B_Spline.h"
#include "Interp.h"
using namespace std;

# define MAX_OUTPUT_SIZE 600.0
# define MAX_MARGIN 50
class FaceMorpher
{
public:
	FaceMorpher();
    void Morph(String srcImgFileName, String tplImgFileName, int morphFunc, int interpMethod, String outputFileName, bool getControlPoints = false, bool drawControlPoints = false);
	~FaceMorpher();
	void setBSplineH(int h) { BSpline_h = h; }
	bool isnan(cv::Mat &m, int &k, bool isrow = true);
	void meshgrid(int x_min, int x_max, int y_min, int y_max, Mat &u, Mat &v);
private:
	vector<Point2d> srcPoints;
	vector<Point2d> dstPoints;

	int BSpline_h;
	void crop(cv::Mat &m);
	cv::Mat m_srcImg;
};

