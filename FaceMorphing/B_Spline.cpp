#include "B_Spline.h"
#include <QDebug>
#include <iostream>
#include <algorithm>
#include <vector>
#include "Interp.h"
using namespace std;

B_Spline::B_Spline(cv::Mat srcImg, vector<Point2d>& srcPoints, vector<Point2d>& dstPoints, int interpmethod) \
	: _srcImg(srcImg), _srcPoints(srcPoints), _dstPoints(dstPoints), _interpmethod(interpmethod)
{
}

B_Spline::~B_Spline()
{
}

double B_Spline::G(int i, double t)
{
	double t2 = t * t;
	double t3 = t * t2;
	switch (i)
	{
	case 0:
		return (-t3 + 3 * t2 - 3 * t + 1) / 6;
	case 1:
		return (3 * t3 - 6 * t2 + 4) / 6;
	case 2:
		return (-3 * t3 + 3 * t2 + 3 * t + 1) / 6;
	case 3:
		return t3 / 6;
	default:
		return 0.0;
	}
}

// Free-Form Deformation
void B_Spline::FFD(double h, cv::Mat & res)
{
	adjustPoints();
	auto &P = _dstPoints;
	auto &Q = _srcPoints;
	int m = ceil((_srcImg.cols - 1) / h) + 1;
	int n = ceil((_srcImg.rows - 1) / h) + 1;
	res = cv::Mat::zeros(_srcImg.size(), CV_8UC3);
	//init
	Mat delta_phi_x = Mat::zeros(n + 2, m + 2, CV_64F);
	Mat delta_phi_y = Mat::zeros(n + 2, m + 2, CV_64F);
	Mat delta_x = Mat::zeros(n + 2, m + 2, CV_64F);
	Mat delta_y = Mat::zeros(n + 2, m + 2, CV_64F);
	Mat omega = Mat::zeros(n + 2, m + 2, CV_64F);

	int i, j, u, v, k, l, a, b;
	double s, t, sum_w2, x, y;

	//calculate \Phi
	for (int index = 0; index < P.size(); index++)
	{
		auto p = P[index];
		auto q = Q[index];
		u = p.x;
		v = p.y;
		i = floor(u / h) - 1;
		j = floor(v / h) - 1;
		s = u / h - floor(u / h);
		t = v / h - floor(v / h);
		sum_w2 = 0;
		for (a = 0; a < 4; a++)
		{
			for (b = 0; b < 4; b++)
			{
				sum_w2 += pow(w(a, b, s, t), 2);
			}
		}
		auto delta_q = (q - p) / h;
		for (k = 0; k < 4; k++)
		{
			for (l = 0; l < 4; l++)
			{
				double w_kl = w(k, l, s, t);
				Point2d Delta_phi_kl = w_kl * delta_q/ sum_w2;
				double w_kl2 = w_kl * w_kl;
				Point2d delta_kl = w_kl2 * Delta_phi_kl;
				delta_x.at<double>(j + l + 1, i + k + 1) += delta_kl.x;
				delta_y.at<double>(j + l + 1, i + k + 1) += delta_kl.y;
				omega.at<double>(j + l + 1, i + k + 1) += w_kl2;
			}
		}
	}

	for (i = 0; i < m + 2; i++)
	{
		for (j = 0; j < n + 2; j++)
		{
			if (abs(omega.at<double>(j, i)) > 1e-6)
			{
				auto dpx = delta_x.at<double>(j, i) / omega.at<double>(j, i);
				auto dpy = delta_y.at<double>(j, i) / omega.at<double>(j, i);
				auto norm_inf = (abs(dpx) > abs(dpy)) ? abs(dpx) : abs(dpy);
				if (norm_inf > 0.48)
				{
					dpx = dpx / norm_inf * 0.48;
					dpy = dpy / norm_inf * 0.48;
				}
				delta_phi_x.at<double>(j, i) = dpx;
				delta_phi_y.at<double>(j, i) = dpy;
			}
			else
			{
				delta_phi_x.at<double>(j, i) = 0;
				delta_phi_y.at<double>(j, i) = 0;
			}
		}
	}

	//inverse interpolation
	for (u = 0; u < _srcImg.cols; u++)
	{
		for (v = 0; v < _srcImg.rows; v++)
		{
			i = floor(u / h) - 1;
			j = floor(v / h) - 1;
			s = u / h - floor(u / h);
			t = v / h - floor(v / h);
			double delta_x = 0;
			double delta_y = 0;
			for (k = 0; k < 4; k++)
			{
				for (l = 0; l < 4; l++)
				{
					delta_x += w(k, l, s, t) * delta_phi_x.at<double>(j + l + 1, i + k + 1) * h;
					delta_y += w(k, l, s, t) * delta_phi_y.at<double>(j + l + 1, i + k + 1) * h;
				}
			}
			x = u + delta_x;
			y = v + delta_y;	
			try
			{
				res.at<Vec3b>(v, u) = interpMethods(_srcImg, Point2d(x, y), _interpmethod);
			}
			catch (const std::exception&)
			{

			}
		}
	}
}

void B_Spline::adjustPoints()
{
	// find bounding box for both src and dst Points
	// top bottom left right
	double src_bound[4];
	double dst_bound[4];
	auto cmpx = [](const Point2d &p1,const Point2d &p2) {
		return p1.x < p2.x;
	};
	auto cmpy = [](const Point2d &p1, const Point2d &p2) {
		return p1.y < p2.y;
	};
	src_bound[0] = (*min_element(_srcPoints.begin(), _srcPoints.end(), cmpy)).y;
	src_bound[1] = (*max_element(_srcPoints.begin(), _srcPoints.end(), cmpy)).y;
	src_bound[2] = (*min_element(_srcPoints.begin(), _srcPoints.end(), cmpx)).x;
	src_bound[3] = (*max_element(_srcPoints.begin(), _srcPoints.end(), cmpy)).x;

	dst_bound[0] = (*min_element(_dstPoints.begin(), _dstPoints.end(), cmpy)).y;
	dst_bound[1] = (*max_element(_dstPoints.begin(), _dstPoints.end(), cmpy)).y;
	dst_bound[2] = (*min_element(_dstPoints.begin(), _dstPoints.end(), cmpx)).x;
	dst_bound[3] = (*max_element(_dstPoints.begin(), _dstPoints.end(), cmpy)).x;

	auto src_middle = Point2d((src_bound[2] + src_bound[3]) / 2, (src_bound[0] + src_bound[1]) / 2);
	auto dst_middle = Point2d((dst_bound[2] + dst_bound[3]) / 2, (dst_bound[0] + dst_bound[1]) / 2);
	//scale to src length
	auto scale_ratio = (src_bound[1] - src_bound[0]) / (dst_bound[1] - dst_bound[0]);

	for (auto &dst : _dstPoints)
	{
		dst = (dst - dst_middle) * scale_ratio + src_middle;
	}
}

double B_Spline::w(int k, int l, double s, double t)
{
	return G(k, s) * G(l, t);
}