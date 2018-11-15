#include "TPS.h"
#include "math.h"

/*
 * Thin Plate Spline
 * srcImgFileName: source Image File Name, e.g. "../picture/1.jpg"
 * dstImgFileName: destination Image File Name
 * detect: decide whether to detect the control points, by default `detect = false`;
 */
TPS::TPS(String srcImgFileName, String dstImgFileName, bool detect)
{
	if (detect)
	{
		FacemarkDetector fd("../model/face_landmark_model.dat", "../model/lbpcascade_frontalface_improved.xml");
		fd.run(srcImgFileName, true, true);
		fd.run(dstImgFileName, true, true);
	}

	m_srcPoints = FacemarkDetector::readControlPoints(srcImgFileName, true);
	m_dstPoints = FacemarkDetector::readControlPoints(dstImgFileName, true);
}

void TPS::solveParams()
{
	initMatrix();
	_W = (_L.inv() * _Y);
}

Point2d TPS::calcTPS(double x, double y)
{
	Point2d p(x, y);
	return calcTPS(p);
}

Point2d TPS::calcTPS(Point2d &p)
{
	//calculate I
	_I = Mat(1, _n + 3, CV_64F);
	for (int i = 0; i < _n; i++)
	{
		_I.at<double>(0, i) = U(p, m_srcPoints[i]);
	}
	_I.at<double>(0, _n) = 1;
	_I.at<double>(0, _n + 1) = p.x;
	_I.at<double>(0, _n + 2) = p.y;

	_F = _I * _W;
	return Point2d(_F.at<double>(0, 0), _F.at<double>(0, 1));
}

TPS::~TPS()
{
}

void TPS::initMatrix()
{
	_n = m_srcPoints.size();
	assert(m_srcPoints.size() == m_dstPoints.size());

	//calculate K
	_K = Mat::zeros(_n, _n, CV_64F);
	for (int i = 0; i < _n; i++)
	{
		for (int j = 0; j < _n; j++)
		{
			if (i == j)
				continue;
			_K.at<double>(i, j) =U(m_srcPoints[i], m_srcPoints[j]);
		}
	}

	//calculate P
	Mat P1 = Mat::ones(_n, 1, CV_64F);
	Mat P2(m_srcPoints.size(), 2, CV_64F, m_srcPoints.data());
	hconcat(P1, P2, _P);

	//calculate L
	Mat L1, L2;
	hconcat(_K, _P, L1);
	hconcat(_P.t(), Mat::zeros(3, 3, CV_64F), L2);
	vconcat(L1, L2, _L);

	//calculate Y
	vconcat(Mat(m_dstPoints.size(), 2, CV_64F, m_dstPoints.data()), Mat::zeros(3, 2, CV_64F), _Y);
}

double TPS::U(double r2)
{
	return r2 > 1e-6 ? r2 * log(r2) : 0.0;
}

double TPS::U(Point2d & p1, Point2d & p2)
{
	Point2d delta = p1 - p2;
	return U(delta.x * delta.x + delta.y * delta.y);
}
