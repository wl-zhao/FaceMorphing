#pragma once
#include "MyCV.h"
#include <opencv2/opencv.hpp>
using namespace mycv;
Vec3b nearestInterplote(cv::Mat &img, Point2d &p);
Vec3b bilinearInterplote(cv::Mat &img, Point2d &p);
Vec3b bicubicInterplote(cv::Mat &img, Point2d &p);
double S(double x);
Vec3b interpMethods(cv::Mat &img, Point2d &p, int i = 0);
