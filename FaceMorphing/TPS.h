#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>
#include "MyCV.h"
#include "FacemarkDetector.h"

using namespace std;
using namespace mycv;

class TPS
{
public:
	TPS(String srcImgFileName, String dstImgFileName, bool detect = false);
	void solveParams();
	Point2d calcTPS(double x, double y);
	Point2d calcTPS(Point2d &p);
	vector<Point2d> dstPoints() { return m_dstPoints; }
	~TPS();

private:
	void initMatrix();

	//Mat m_srcImg;
	vector<Point2d> m_srcPoints;
	vector<Point2d> m_dstPoints;

	//TPS variables
	int _n;
	//nxn
	Mat _K;
	//nx3
	Mat _P;
	/*
	_L = [[K    P],
		  [P^T, 0]]
	(n+3)x(n+3)
	*/
	Mat _L;
	Mat _V;//2xn
	Mat _Y;//(n+3)x2
	Mat _a;//2xn
	Mat _a1;//2x1
	Mat _ax;//2x1
	Mat _ay;//2x1
	Mat _W;//(n+3)x2

	Mat _I;//input related
	Mat _F;//output

	double U(double r2);
	double U(Point2d &p1, Point2d &p2);
};

