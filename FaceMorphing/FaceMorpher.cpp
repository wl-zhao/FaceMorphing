#include "FaceMorpher.h"
#include "FacemarkDetector.h"
#include <direct.h>

using mycv::Size;
using cv::imread;
using cv::imwrite;

FaceMorpher::FaceMorpher()
{
}

void FaceMorpher::Morph(String srcImgFileName, String tplImgFileName, int morphFunc, int interpMethod, String outputFileName, bool getControlPoints, bool drawControlPoints)
{
	if (morphFunc == 0)
	{
		TPS tps1(srcImgFileName, tplImgFileName, getControlPoints);
		tps1.solveParams();

		TPS tps2(tplImgFileName, srcImgFileName);
		tps2.solveParams();

		m_srcImg = imread(srcImgFileName);
		vector<double> x_bound;
		vector<double> y_bound;
		Point2d p;
		for (int i = 0; i < m_srcImg.rows; i++)
		{
			p = tps1.calcTPS(0, i);
			x_bound.push_back(p.x);
			y_bound.push_back(p.y);
			p = tps1.calcTPS(m_srcImg.cols - 1, i);
			x_bound.push_back(p.x);
			y_bound.push_back(p.y);
		}
		for (int j = 0; j < m_srcImg.cols; j++)
		{
			p = tps1.calcTPS(j, 0);
			x_bound.push_back(p.x);
			y_bound.push_back(p.y);
			p = tps1.calcTPS(j, m_srcImg.rows - 1);
			x_bound.push_back(p.x);
			y_bound.push_back(p.y);
		}
		int x_min = *min_element(x_bound.begin(), x_bound.end());
		int x_max = *max_element(x_bound.begin(), x_bound.end());
		int y_min = *min_element(y_bound.begin(), y_bound.end());
		int y_max = *max_element(y_bound.begin(), y_bound.end());


		Mat u, v;
		meshgrid(x_min - MAX_MARGIN, x_max + MAX_MARGIN, y_min - MAX_MARGIN, y_max + MAX_MARGIN, u, v);
		cv::Mat dst = cv::Mat::zeros(u.rows, u.cols, CV_8UC3);
		for (int i = 0; i < u.rows; i++)
		{
			for (int j = 0; j < u.cols; j++)
			{
				p = tps2.calcTPS(u.at<double>(i, j), v.at<double>(i, j));
				dst.at<Vec3b>(i, j) = interpMethods(m_srcImg, p, interpMethod);
			}
		}

		crop(dst);
		imwrite(outputFileName, dst);
	}
	else
	{
		srcPoints = FacemarkDetector::readControlPoints(srcImgFileName, true, getControlPoints);
		dstPoints = FacemarkDetector::readControlPoints(tplImgFileName, true, getControlPoints);
		m_srcImg = imread(srcImgFileName);
		B_Spline bs(m_srcImg, srcPoints, dstPoints, interpMethod);
		cv::Mat dst;
		bs.FFD(BSpline_h, dst);
		imwrite(outputFileName, dst);
	}
}


FaceMorpher::~FaceMorpher()
{
}

bool FaceMorpher::isnan(cv::Mat & m, int &k, bool isrow)
{
	Vec3b nan = Vec3b(0, 0, 0);
	if (isrow)
	{
		try
		{
			for (int j = 0; j < m.cols; j++)
			{
				if (m.at<Vec3b>(k, j) != nan)
					return false;
			}
			return true;
		}
		catch (const std::exception&)
		{
			return false;
		}
	}
	else
	{
		try
		{
			for (int i = 0; i < m.rows; i++)
			{
				if (m.at<Vec3b>(i, k) != nan)
					return false;
			}
			return true;
		}
		catch (const std::exception&)
		{
			return false;
		}
	}
	return false;
}

void FaceMorpher::crop(cv::Mat &m)
{
	int row1 = 0;
	int row2 = m.rows - 1;
	int col1 = 0;
	int col2 = m.cols - 1;
	while (isnan(m, row1, true))
		row1++;
	while (isnan(m, row2, true))
		row2--;
	while (isnan(m, col1, false))
		col1++;
	while (isnan(m, col2, false))
		col2--;
	Rect roi = Rect(col1, row1, col2 - col1, row2 - row1);
	m = m(roi);
}

void FaceMorpher::meshgrid(int x_min, int x_max, int y_min, int y_max, Mat & u, Mat & v)
{
	auto size_x = x_max - x_min + 1;
	auto size_y = y_max - y_min + 1;
	auto max_size = (size_x > size_y) ? size_x : size_y;
	float ratio = (max_size > MAX_OUTPUT_SIZE) ? (max_size / MAX_OUTPUT_SIZE) : 1;
	u = Mat::zeros(Size(size_x / ratio, size_y / ratio), CV_64F);
	v = Mat::zeros(Size(size_x / ratio, size_y / ratio), CV_64F);
	for (int i = 0; i < u.rows; i++)
	{
		for (int j = 0; j < u.cols; j++)
		{
			u.at<double>(i, j) = j * ratio + x_min;
			v.at<double>(i, j) = i * ratio + y_min;
		}
	}
}



