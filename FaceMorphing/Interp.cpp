#include "Interp.h"

Vec3b nearestInterplote(cv::Mat & img, Point2d & p)
{
	try
	{
		return img.at<Vec3b>(floor(p.y + 0.5), floor(p.x + 0.5));
	}
	catch (const std::exception&)
	{
		return Vec3b(0, 0, 0);
	}
}

Vec3b bilinearInterplote(cv::Mat & img, Point2d & p)
{
	try
	{
		int x1 = floor(p.x);
		int x2 = x1 + 1;
		int y1 = floor(p.y);
		int y2 = y1 + 1;
		return	img.at<Vec3b>(y1, x1) * (x2 - p.x) * (y2 - p.y) + \
			img.at<Vec3b>(y1, x2) * (p.x - x1) * (y2 - p.y) + \
			img.at<Vec3b>(y2, x1) * (x2 - p.x) * (p.y - y1) + \
			img.at<Vec3b>(y2, x2) * (p.x - x1) * (p.y - y1);
	}
	catch (const std::exception&)
	{
		return Vec3b(0, 0, 0);
	}
}

Vec3b bicubicInterplote(cv::Mat & img, Point2d & p)
{
	try
	{
		Vec3b res;
		auto x1 = floor(p.x);
		auto y1 = floor(p.y);
		auto u = p.x - x1;
		auto v = p.y - y1;
		double tmp;

		vector<double> v_A = { S(u + 1), S(u) ,S(u - 1), S(u - 2) };
		Mat A(1, 4, v_A);
		vector<double> v_C = { S(v + 1), S(v), S(v - 1), S(v - 2) };
		Mat C(1, 4, v_C);
		Mat B[3] = { Mat::zeros(4, 4, CV_64F), Mat::zeros(4, 4, CV_64F), Mat::zeros(4, 4, CV_64F) };
		for (int ch = 0; ch < 3; ch++)
		{
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					B[ch].at<double>(i, j) = img.at<Vec3b>(y1 - 1 + i, x1 - 1 + j)(ch);
				}
			}
			//solve out of range problem
			tmp = (A * B[ch] * C.t()).at<double>(0, 0);
			tmp = (tmp > 255) ? 255 : tmp;
			tmp = (tmp < 0) ? 0 : tmp;
			res[ch] = tmp;
		}
		return res;
	}
	catch (const std::exception&)
	{
		return Vec3b(0, 0, 0);
	}
}

Vec3b interpMethods(cv::Mat & img, Point2d & p, int i)
{
	switch (i)
	{
	case 0:
		return nearestInterplote(img, p);
	case 1:
		return bilinearInterplote(img, p);
	case 2:
		return bicubicInterplote(img, p);
	default:
		return Vec3b();
		break;
	}
}

double S(double x)
{
	auto abs_x = fabs(x);
	if (abs_x >= 2)
		return 0.0;
	auto abs_x2 = abs_x * abs_x;
	auto abs_x3 = abs_x2 * abs_x;

	if (abs_x <= 1)
	{
		return 1 - 2 * abs_x2 + abs_x3;
	}
	else if (abs_x < 2)
	{
		return 4 - 8 * abs_x + 5 * abs_x2 - abs_x3;
	}
	return 0.0;
}