#pragma once
#include "MyCV.h"
#include <opencv2/opencv.hpp>
#include <vector>
using namespace std;
using namespace mycv;

class B_Spline
{
public:
	B_Spline(cv::Mat srcImg, vector<cv::Point2d> &srcPoints, vector<cv::Point2d> &dstPoints, int interpMethod = 0);
	~B_Spline();

	double G(int i, double t);
	double w(int k, int l, double s, double t);
	void FFD(double h, cv::Mat &res);
	void adjustPoints();
	cv::Mat _srcImg;
	vector<Point2d> _srcPoints;
	vector<Point2d> _dstPoints;
	int _interpmethod;

private:
	int sign(double x) { return x > 0; };
};

